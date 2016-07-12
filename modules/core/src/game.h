#ifndef GAME_H
#define GAME_H

#include "world.h"
#include <stdbool.h>

typedef struct {
  World **worlds;
} Game;

void game_init(Game *game);
void game_delete(Game *game);
bool game_alloc_worlds(Game *game, int num);
bool game_del_world(Game *game, World *world);

#endif /* GAME_H */
