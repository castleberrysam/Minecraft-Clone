#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "chunk.h"
#include "shader.h"
#include "matrix.h"
#include "texture.h"

static GLuint buffer = 0;
static GLuint vao = 0;
static GLuint program = 0;

void chunk_init(Chunk *chunk, Vector3i *pos)
{
  if(buffer == 0) {
    GLuint shaders[3];
    shaders[0] = load_shader("res/shader/chunk.vert", GL_VERTEX_SHADER);
    shaders[1] = load_shader("res/shader/chunk.geom", GL_GEOMETRY_SHADER);
    shaders[2] = load_shader("res/shader/chunk.frag", GL_FRAGMENT_SHADER);
    program = compile_program(3, shaders);
    
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*4096, NULL, GL_STATIC_DRAW);
    GLfloat *data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for(int x=0;x<16;++x) {
      for(int y=0;y<16;++y) {
	for(int z=0;z<16;++z) {
	  int index = (x + (y * 16) + (z * 256)) * 3;
	  data[index] = (GLfloat) x;
	  data[index+1] = (GLfloat) y;
	  data[index+2] = (GLfloat) z;
	}
      }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glGenVertexArrays(1, &vao);
  }
  
  chunk->blocks = malloc(sizeof(Block *) * 4096);
  vec_copy3i(&chunk->pos, pos);
}

void chunk_delete(Chunk *chunk)
{
  model_delete((Model *) chunk);
  free(chunk->blocks);
}

inline Block * chunk_block_get(Chunk *chunk, Vector3i *pos)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  return chunk->blocks[index];
}

void chunk_block_set(Chunk *chunk, Vector3i *pos, Block *block)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  chunk->blocks[index] = block;
}

void chunk_block_delete(Chunk *chunk, Vector3i *pos)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  chunk->blocks[index] = NULL;
}

void chunk_draw(Chunk *chunk)
{
  GLuint blocks, texbuf;
  glGenBuffers(1, &blocks);
  glBindBuffer(GL_TEXTURE_BUFFER, blocks);
  glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint)*5832, NULL, GL_STREAM_DRAW);
  GLint *data = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
  for(int x=0;x<18;++x) {
    for(int y=0;y<18;++y) {
      for(int z=0;z<18;++z) {
	int index1 = x+(y*18)+(z*324);
	int index2 = (x-1)+((y-1)*16)+((z-1)*256);
	if(x == 0 || x == 17 || y == 0 || y == 17 || z == 0 || z == 17) {
	  data[index1] = -1;
	} else {
	  data[index1] = chunk->blocks[index2] == NULL ? -1 : chunk->blocks[index2]->texture;
	}
      }
    }
  }
  glUnmapBuffer(GL_TEXTURE_BUFFER);
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texbuf);
  glBindTexture(GL_TEXTURE_BUFFER, texbuf);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, blocks);

  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "blocks"), 0);
  glUniform1i(glGetUniformLocation(program, "textures"), 1);
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texture_array);
  matrix_update_mvp(glGetUniformLocation(program, "mvp"));
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBindVertexArray(vao);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) 0);
  glEnableVertexAttribArray(0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glDrawArrays(GL_POINTS, 0, 4096);

  glDeleteBuffers(1, &blocks);
  glDeleteTextures(1, &texbuf);
}
