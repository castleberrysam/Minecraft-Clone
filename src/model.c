#include <GL/glew.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "model.h"

void model_init(Model *model, ModelType type)
{
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[MODEL] new model %p\n", model);
#endif
  model->type = type;
  switch(type) {
  case MODEL_OBJ:
    model->data.obj.nverts = 0;
    model->data.obj.verts = NULL;
    model->data.obj.ntcoords = 0;
    model->data.obj.tcoords = NULL;
    model->data.obj.nnorms = 0;
    model->data.obj.norms = NULL;
    model->data.obj.nfaces = 0;
    model->data.obj.faces = NULL;
    break;
  case DISPLAY_LIST:
    model->data.list = 0;
    break;
  }
}

void model_delete(Model *model)
{
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[MODEL] delete model %p, list %d\n", model, model->data.list);
#endif
  switch(model->type) {
  case MODEL_OBJ:
    free(model->data.obj.verts);
    free(model->data.obj.tcoords);
    free(model->data.obj.norms);
    free(model->data.obj.faces);
    break;
  case DISPLAY_LIST:
    glDeleteLists(model->data.list, 1);
    break;
  }
}

void model_copy(Model *new, Model *old)
{
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[MODEL] copy model %p to model %p, list %d\n", old, new, old->data.list);
#endif
  new->type = old->type;
  switch(new->type) {
  case MODEL_OBJ:
    new->data.obj.nverts = old->data.obj.nverts;
    memcpy(new->data.obj.verts, old->data.obj.verts, new->data.obj.nverts*sizeof(Vector4d));
    new->data.obj.ntcoords = old->data.obj.ntcoords;
    memcpy(new->data.obj.tcoords, old->data.obj.tcoords, new->data.obj.ntcoords*sizeof(Vector3d));
    new->data.obj.nnorms = old->data.obj.nnorms;
    memcpy(new->data.obj.norms, old->data.obj.norms, new->data.obj.nnorms*sizeof(Vector3d));
    new->data.obj.nfaces = old->data.obj.nfaces;
    memcpy(new->data.obj.faces, old->data.obj.faces, new->data.obj.nfaces*sizeof(Vector3i));
    break;
  case DISPLAY_LIST:
    // XXX TODO display list will be deleted and invalidated for all copies if any copy is deleted
    new->data.list = old->data.list;
    break;
  }
}

bool model_gen_list(Model *model, void (*draw)(void *user), void *user)
{
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[MODEL] gen list for model %p, draw func %p\n", model, draw);
#endif
  if(model->type != DISPLAY_LIST) {return false;}
  glDeleteLists(model->data.list, 1);
  model->data.list = glGenLists(1);
  if(model->data.list == 0) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[MODEL] display list generation failed\n");
#endif
    return false;
  }

  glNewList(model->data.list, GL_COMPILE);
  draw(user);
  glEndList();

#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[MODEL] new list is %d\n", model->data.list);
#endif
  return true;
}

void model_draw(Model *model)
{
  switch(model->type) {
  case MODEL_OBJ:
    break;
  case DISPLAY_LIST:
    glCallList(model->data.list);
    break;
  }
}
