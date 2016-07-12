#include <stdlib.h>
#include "game.h"

void game_init(Game *game)
{
  game->worlds = malloc(sizeof(World *) * 512);
  game->worlds[0] = NULL;
}

void game_delete(Game *game)
{
  int i = 0;
  World *world = game->worlds[i];
  while(world != NULL) {
    world_delete(world);
    free(world);
    
    world = game->worlds[++i];
  }
  free(game->worlds);
}

bool game_alloc_worlds(Game *game, int num)
{
  int i = 0;
  World *tmp = game->worlds[i];
  while(tmp != NULL) {tmp = game->worlds[++i];}

  int j = i + num;
  while(i < j) {
    World *world = malloc(sizeof(World));
    world_init(world);
    game->worlds[i++] = world;
  }
  game->worlds[j] = NULL;
  return true;
}

bool game_del_world(Game *game, World *world)
{
  int i = 0, j = -1;
  World *tmp = game->worlds[i];
  while(tmp != NULL) {
    if(j < 0 && tmp == world) {
      world_delete(tmp);
      free(tmp);
      j = i;
    }
    tmp = game->worlds[++i];
  }
  if(j < 0) {return false;}
  
  game->worlds[j] = game->worlds[i-1];
  game->worlds[i-1] = NULL;
  return true;
}
