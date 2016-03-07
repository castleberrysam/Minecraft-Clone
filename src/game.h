#ifndef GAME_H
#define GAME_H

#include "module.h"
#include "world.h"
#include "action.h"

typedef struct {
  int num_loaded;
  module *loaded_mods;
  int num_unloaded;
  module *unloaded_mods;
  int num_worlds;
  world *worlds;
  int num_actions;
  action *actions;
} game;

#endif /* GAME_H */
