#ifndef MODEL_H
#define MODEL_H

#include "vector.h"
#include <GL/glew.h>

typedef enum {
  MODEL_OBJ,
  DISPLAY_LIST
} ModelType;

typedef struct {
  uint64_t nverts;
  Vector4d *verts;
  uint64_t ntcoords;
  Vector3d *tcoords;
  uint64_t nnorms;
  Vector3d *norms;
  uint64_t nfaces;
  Vector3i *faces;
} ModelObj;

typedef union {
  ModelObj obj;
  GLuint list;
} ModelData;

typedef struct {
  ModelType type;
  ModelData data;
} Model;

void model_init(Model *model, ModelType type);
void model_delete(Model *model);
void model_copy(Model *new, Model *old);
bool model_gen_list(Model *model, void (*draw)(void *user), void *user);
void model_draw(Model *model);

#endif /* MODEL_H */
