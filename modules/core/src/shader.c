#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include "shader.h"

GLuint load_shader(char *filepath, GLenum type)
{
  FILE *file = fopen(filepath, "rb");
  if(!file) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[SHADR] path %s is corrupted or does not exist\n", filepath);
#endif
    return 0;
  }

  long len;
  if(fseek(file, 0, SEEK_END) != 0 ||
     (len = ftell(file)) == -1L) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[SHADR] failed to find size of %s\n", filepath);
#endif
    fclose(file);
    return 0;
  }
  rewind(file);

  char *buffer = malloc(len);
  if(fread(buffer, 1, len, file) != len) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[SHADR] failed to read contents of %s\n", filepath);
#endif
    fclose(file);
    free(buffer);
    return 0;
  }
  fclose(file);

  GLuint shader = glCreateShader(type);
  if(shader == 0) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[SHADR] shader generation of type %d failed\n", type);
#endif
    free(buffer);
    return 0;
  }

  glShaderSource(shader, 1, (const GLchar * const *) &buffer, (GLint *) &len);
  free(buffer);
  return shader;
}

GLuint compile_program(int num, GLuint *shaders)
{
  GLint success = GL_TRUE;
  for(int i=0;i<num;++i) {
    GLuint shader = shaders[i];

    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
#ifdef DEBUG_GRAPHICS
      fprintf(stderr, "[SHADR] shader failed to compile\n");
#endif
      success = GL_FALSE;
    }
    
#ifdef DEBUG_GRAPHICS
    GLint info_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    if(info_len < 2) {continue;}

    GLint type;
    glGetShaderiv(shader, GL_SHADER_TYPE, &type);
    char *typestr;
    switch(type) {
    case GL_VERTEX_SHADER:
      typestr = "vertex";
      break;
    case GL_GEOMETRY_SHADER:
      typestr = "geometry";
      break;
    case GL_FRAGMENT_SHADER:
      typestr = "fragment";
      break;
    }

    char *info = malloc(info_len);
    glGetShaderInfoLog(shader, info_len, NULL, info);
    fprintf(stderr, "[SHADR] begin %s shader %d info log\n%s[SHADR] end log\n", typestr, shader, info);
    free(info);
#endif
  }
  if(success != GL_TRUE) {return 0;}

  GLuint program = glCreateProgram();
  if(program == 0) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[SHADR] program generation failed\n");
#endif
    return 0;
  }

  for(int i=0;i<num;++i) {glAttachShader(program, shaders[i]);}
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  
#ifdef DEBUG_GRAPHICS
  if(success != GL_TRUE) {
    fprintf(stderr, "[SHADR] program failed to link\n");
  }
  
  GLint info_len;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);
  if(info_len < 2) {goto end;}

  char *info = malloc(info_len);
  glGetProgramInfoLog(program, info_len, NULL, info);
  fprintf(stderr, "[SHADR] begin program %d info log\n%s[SHADR] end log\n", program, info);
  free(info);
 end:
#endif
  
  if(success == GL_TRUE) {
    return program;
  } else {
    glDeleteProgram(program);
    return 0;
  }
}
