#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "inventory.h"

typedef struct {
  entity *entity;
  inventory *inventory;
  int health;
  double speed;
  double jump_height;
} player;

#endif /* PLAYER_H */
