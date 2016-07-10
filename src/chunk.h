#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"

typedef struct {
  Model model;
  bool changed;
  // in order of row majority: x, y, z
  Block **blocks;
  Vector3i pos;
} Chunk;

void chunk_init(Chunk *chunk, Vector3i *pos);
void chunk_delete(Chunk *chunk);
Block * chunk_block_get(Chunk *chunk, Vector3i *pos); // relative to chunk
void chunk_block_delete(Chunk *chunk, Vector3i *pos);
void chunk_draw(Chunk *chunk);

#endif /* CHUNK_H */
