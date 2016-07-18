#include <GL/glew.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "model.h"

void model_init(Model *model, GLuint buffer, GLuint vao, GLuint program, GLenum mode, GLsizei nverts)
{
  model->buffer = buffer;
  model->vao = vao;
  model->program = program;
  model->mode = mode;
  model->nverts = nverts;
}

void model_delete(Model *model)
{
  ;
}

void model_copy(Model *new, Model *old)
{
  new->buffer = old->buffer;
  new->vao = old->vao;
  new->program = old->program;
  new->mode = old->mode;
  new->nverts = old->nverts;
}

void model_draw(Model *model)
{
  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, model->buffer);
  glDrawArrays(model->mode, 0, model->nverts);
}
