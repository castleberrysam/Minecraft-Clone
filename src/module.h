#ifndef MODULE_H
#define MODULE_H

#include "block.h"
#include "entity.h"
#include "player.h"

typedef struct {
  char *name;
  char *author;
  char *version;
  int num_blocks;
  block *blocks;
  int num_entities;
  entity *entities;
  int num_players;
  player *players;
} module;

#endif /* MODULE_H */
