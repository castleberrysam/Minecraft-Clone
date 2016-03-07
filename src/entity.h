#ifndef ENTITY_H
#define ENTITY_H

#include "object.h"
#include "model.h"

typedef struct {
  object *object;
  model *model;
} entity;

#endif /* ENTITY_H */
