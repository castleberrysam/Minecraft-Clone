#include "entity.h"

void entity_init(Entity *entity, EntityType etype, Model *model)
{
  model_copy(&entity->model, model);
  entity->type = etype;
  vec_set3d(&entity->pos, 7.0, 64.0, 7.0);
  vec_set3d(&entity->vel, 0.0, 0.0, 0.0);
  vec_set3d(&entity->apos, 0.0, 0.0, 0.0);
  vec_set3d(&entity->avel, 0.0, 0.0, 0.0);
  if(etype == PLAYER) {
    vec_set3d(&entity->bb_dim, 0.4, 0.9, 0.4);
  } else {
    vec_set3d(&entity->bb_dim, 0.5, 0.5, 0.5);
  }
  vec_set3d(&entity->bb_off, 0.0, 0.0, 0.0);
  entity->mass = 1.0;
}

void entity_delete(Entity *entity)
{
  model_delete(&entity->model);
}
