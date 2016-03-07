#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "vector.h"
#include "player.h"
#include "block.h"

typedef struct {
  int num_chunks;
  chunk *chunks;
  vectori3 *coords;
  int num_players;
  player *players;
  int num_mappings;
  block *mappings;
} world;

#endif /* WORLD_H */
