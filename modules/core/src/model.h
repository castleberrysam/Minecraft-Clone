#ifndef MODEL_H
#define MODEL_H

#include "vector.h"
#include <GL/glew.h>

typedef struct Model {
  GLuint buffer;
  GLuint vao;
  GLuint program;
  GLenum mode;
  GLsizei nverts;
} Model;

void model_init(Model *model, GLuint buffer, GLuint vao, GLuint program, GLenum mode, GLsizei nverts);
void model_delete(Model *model);
void model_copy(Model *new, Model *old);
void model_draw(Model *model);

#endif /* MODEL_H */
