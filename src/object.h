#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"

typedef struct {
  vectord3 pos;
  vectord3 vel;
  vectord3 bb_dims;
  vectord3 bb_offset;
  double mass;
} object;

#endif /* OBJECT_H */
