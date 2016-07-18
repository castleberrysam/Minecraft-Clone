#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"

typedef struct {
  // in order of row majority: x, y, z
  Block **blocks;
  Vector3i pos;
} Chunk;

void chunk_init(Chunk *chunk, Vector3i *pos);
void chunk_delete(Chunk *chunk);
Block * chunk_block_get(Chunk *chunk, Vector3i *pos); // relative to chunk
void chunk_block_set(Chunk *chunk, Vector3i *pos, Block *block);
void chunk_block_delete(Chunk *chunk, Vector3i *pos);
void chunk_regen_buffer(Chunk *chunk);
void chunk_draw(Chunk *chunk);

#endif /* CHUNK_H */
