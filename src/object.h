#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"

typedef struct {
  Vector3d rot;
  Vector3d pos;
  Vector3d vel;
  Vector3d bb_dims;
  Vector3d bb_offset;
  double mass;
} Object;

#endif /* OBJECT_H */
