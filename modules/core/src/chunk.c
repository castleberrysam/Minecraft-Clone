#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "world.h"
#include "shader.h"
#include "matrix.h"
#include "texture.h"

static GLuint buffer_regen = 0;
static GLuint vao_regen = 0;
static GLuint program_regen = 0;
static GLuint vao_render = 0;
static GLuint program_render = 0;

void chunk_init(Chunk *chunk, Vector3i *pos)
{
  if(buffer_regen == 0) {
    GLuint shaders[2];
    shaders[0] = load_shader("res/shader/chunk_regen.vert", GL_VERTEX_SHADER);
    shaders[1] = load_shader("res/shader/chunk_regen.geom", GL_GEOMETRY_SHADER);
    program_regen = compile_program(2, shaders);
    char *varyings[3] = {"pos", "texcoords", "tex"};
    set_xfb_varyings(program_regen, 3, varyings, GL_INTERLEAVED_ATTRIBS);

    shaders[0] = load_shader("res/shader/chunk_render.vert", GL_VERTEX_SHADER);
    shaders[1] = load_shader("res/shader/chunk_render.frag", GL_FRAGMENT_SHADER);
    program_render = compile_program(2, shaders);
    
    glGenBuffers(1, &buffer_regen);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_regen);
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

    glGenVertexArrays(1, &vao_regen);
    glGenVertexArrays(1, &vao_render);
  }
  
  chunk->blocks = calloc(4096, sizeof(Block *));
  vec_copy3i(&chunk->pos, pos);
  chunk->changed = true;
  chunk->empty = true;
  glGenBuffers(1, &chunk->buffer);
  chunk->num_verts = 0;
}

void chunk_delete(Chunk *chunk)
{
  free(chunk->blocks);
  glDeleteBuffers(1, &chunk->buffer);
}

inline Block * chunk_block_get(Chunk *chunk, Vector3i *pos)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  return chunk->blocks[index];
}

void chunk_block_set(Chunk *chunk, Vector3i *pos, Block *block)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  chunk->changed = !block_equal(chunk->blocks[index], block);
  chunk->blocks[index] = block;

  if(block == NULL) {
    chunk->empty = true;
    for(int i=0;i<4096;++i) {
      if(chunk->blocks[i] != NULL) {
	chunk->empty = false;
	break;
      }
    }
  } else {
    chunk->empty = false;
  }
}

static void chunk_regen_buffer(Chunk **adjacent)
{
  Chunk *chunk = adjacent[13];
  
  GLuint blocks, texbuf;
  glGenBuffers(1, &blocks);
  glBindBuffer(GL_TEXTURE_BUFFER, blocks);
  glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint)*5832, NULL, GL_STREAM_DRAW);
  GLint *data = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);

  for(int x=0;x<18;++x) {
    for(int y=0;y<18;++y) {
      for(int z=0;z<18;++z) {
	int ix = x != 0 ? (x != 17 ? x-1 : 0) : 15;
	int iy = y != 0 ? (y != 17 ? y-1 : 0) : 15;
	int iz = z != 0 ? (z != 17 ? z-1 : 0) : 15;
	int index = ix + (iy * 16) + (iz * 256);

	int ic = 0;
	ic += x != 0 ? (x != 17 ? 1 : 2) : 0;
	ic += y != 0 ? (y != 17 ? 9 : 18) : 0;
	ic += z != 0 ? (z != 17 ? 3 : 6) : 0;
	
	data[x+(y*18)+(z*324)] = adjacent[ic] != NULL ?
	  (adjacent[ic]->blocks[index] != NULL ? adjacent[ic]->blocks[index]->texture : -1) : -1;
      }
    }
  }
  glUnmapBuffer(GL_TEXTURE_BUFFER);
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texbuf);
  glBindTexture(GL_TEXTURE_BUFFER, texbuf);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, blocks);
  
  glUseProgram(program_regen);
  glUniform1i(glGetUniformLocation(program_regen, "blocks"), 0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_regen);
  glBindVertexArray(vao_regen);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) 0);
  glEnableVertexAttribArray(0);
  
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, chunk->buffer);
  glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 2048*6*6*((sizeof(GLfloat)*5)+sizeof(GLint)),
	       NULL, GL_STATIC_COPY);
  
  GLuint query;
  glGenQueries(1, &query);
  glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(GL_TRIANGLES);
  glDrawArrays(GL_POINTS, 0, 4096);
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
  glGetQueryObjectuiv(query, GL_QUERY_RESULT, &chunk->num_verts);
  glDeleteQueries(1, &query);
  chunk->num_verts *= 3;
  
  glDeleteBuffers(1, &blocks);
  glDeleteTextures(1, &texbuf);

  chunk->changed = false;
}

void chunk_draw(Chunk *chunk, World *world)
{
  if(chunk->empty) {return;}
  if(chunk->changed) {
    Chunk *adjacent[27];
    world_chunk_adjacent(world, chunk, adjacent);
    chunk_regen_buffer(adjacent);
  }
  
  glUseProgram(program_render);
  glUniform1i(glGetUniformLocation(program_render, "textures"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_atlas);
  matrix_update_mvp(glGetUniformLocation(program_render, "mvp"));
  glBindBuffer(GL_ARRAY_BUFFER, chunk->buffer);
  glBindVertexArray(vao_render);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (sizeof(GLfloat)*5)+sizeof(GLint),
			(const GLvoid *) 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (sizeof(GLfloat)*5)+sizeof(GLint),
			(const GLvoid *) (sizeof(GLfloat)*3));
  glVertexAttribIPointer(2, 1, GL_INT, (sizeof(GLfloat)*5)+sizeof(GLint),
			 (const GLvoid *) (sizeof(GLfloat)*5));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDrawArrays(GL_TRIANGLES, 0, chunk->num_verts);
}
