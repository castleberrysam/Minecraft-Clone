#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "vector.h"
#include "player.h"
#include "block.h"
#include <stdbool.h>

typedef struct {
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
bool world_alloc_entities(World *world, EntityType etype, Model *model, int num);
bool world_alloc_players(World *world, int num);
bool world_del_entity(World *world, Entity *entity);
bool world_del_player(World *world, Player *player);
bool world_add_mapping(World *world, Block *mapping);
bool world_delete_mapping(World *world, Block *mapping);
Block * world_block_get3i(World *world, Vector3i *pos);
Block * world_block_get3d(World *world, Vector3d *pos);
void world_block_delete3i(World *world, Vector3i *pos);
void world_block_delete3d(World *world, Vector3d *pos);
void world_draw(World *world);

#endif /* WORLD_H */
