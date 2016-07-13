#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "model.h"

typedef enum {
  PLAYER,
  OTHER
} EntityType;

typedef struct {
  Model model;
  EntityType type;
  Vector3d pos;
  Vector3d vel;
  // used during physics
  Vector3d old_vel;
  // angular equivalents
  Vector3d apos;
  Vector3d avel;
  // used during physics
  Vector3d old_avel;
  // bounding box (rectangular)
  Vector3d bb_dim;
  Vector3d bb_off;
  double mass;
} Entity;

void entity_init(Entity *entity, EntityType etype, Model *model);
void entity_delete(Entity *entity);

#endif /* ENTITY_H */
