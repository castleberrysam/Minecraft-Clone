#ifndef BLOCK_H
#define BLOCK_H

#include "entity.h"

typedef struct {
  short num_id;
  char *str_id;
  char *name;
  entity *entity;
} block;

#endif /* BLOCK_H */
