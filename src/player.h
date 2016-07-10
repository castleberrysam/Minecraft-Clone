#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "inventory.h"
#include <stdint.h>

typedef struct {
  Entity entity;
  Inventory *inv;
  uint32_t hp;
  double speed;
  double jump_height;
} Player;

void player_init(Player *player);
void player_delete(Player *player);

#endif /* PLAYER_H */
