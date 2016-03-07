#ifndef MODEL_H
#define MODEL_H

#include "vector.h"

typedef enum {
  MODEL_OBJ
} model_type;

typedef struct {
  int num_vertices;
  vectord4 *vertices;
  vectord3 *tex_coords;
  vectord3 *normals;
  int num_faces;
  vectori3 *faces;
} model_obj;

typedef struct {
  model_type type;
  union {
    model_obj obj;
  } data;
} model;

#endif /* MODEL_H */
