#ifndef WORLD_H
#define WORLD_H

#include "vector.h"
#include "player.h"
#include "block.h"
#include <stdbool.h>

typedef struct Chunk {
  // in order of row majority: x, y, z
  Block **blocks;
  Vector3i pos;
  GLuint buffer;
  GLuint feedback;
  bool changed;
  bool empty;
} Chunk;

typedef struct World {
  Chunk **chunks;
  Entity **entities;
  Block **mappings;
} World;

void world_init(World *world);
void world_delete(World *world);
bool world_chunk_gen3i(World *world, Vector3i *pos, uint64_t seed);
bool world_chunk_gen3d(World *world, Vector3d *pos, uint64_t seed);
bool world_chunk_delete3i(World *world, Vector3i *pos);
bool world_chunk_delete3d(World *world, Vector3d *pos);
Chunk * world_chunk_get3i(World *world, Vector3i *pos);
void world_chunk_adjacent(World *world, Chunk *chunk, Chunk **adjacent);
void world_chunk_regen_buffer(World *world, Vector3i *pos);
bool world_alloc_entities(World *world, EntityType etype, Model *model, int num);
bool world_alloc_players(World *world, int num);
bool world_del_entity(World *world, Entity *entity);
bool world_del_player(World *world, Player *player);
bool world_add_mapping(World *world, Block *mapping);
bool world_delete_mapping(World *world, Block *mapping);
Block * world_mapping_get(World *world, char *str_id);
Block * world_block_get3i(World *world, Vector3i *pos);
Block * world_block_get3d(World *world, Vector3d *pos);
void world_block_set3i(World *world, Vector3i *pos, Block *block);
void world_block_set3d(World *world, Vector3d *pos, Block *block);
void world_draw(World *world);

void chunk_init(Chunk *chunk, Vector3i *pos);
void chunk_delete(Chunk *chunk);
Block * chunk_block_get(Chunk *chunk, Vector3i *pos); // relative to chunk
void chunk_block_set(Chunk *chunk, Vector3i *pos, Block *block);
void chunk_draw(Chunk *chunk, World *world);

#endif /* WORLD_H */
