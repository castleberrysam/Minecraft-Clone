#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <string.h>
#include "world.h"
#include "matrix.h"

void world_init(World *world)
{
  world->chunks = malloc(sizeof(Chunk *) * 8192);
  world->chunks[0] = NULL;
  world->entities = malloc(sizeof(Entity *) * 512);
  world->entities[0] = NULL;
  world->mappings = malloc(sizeof(Block *) * 512);
  world->mappings[0] = NULL;
}

void world_delete(World *world)
{
  int i = 0;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    chunk_delete(chunk);
    free(chunk);
    chunk = world->chunks[++i];
  }
  free(world->chunks);

  i = 0;
  Entity *entity = world->entities[i];
  while(entity != NULL) {
    if(entity->type == PLAYER) {
      player_delete((Player *) entity);
    } else {
      entity_delete(entity);
    }
    free(entity);
    entity = world->entities[++i];
  }
  free(world->entities);

  i = 0;
  Block *block = world->mappings[i];
  while(block != NULL) {
    block_delete(block);
    free(block);
    block = world->mappings[++i];
  }
  free(world->mappings);
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

static double world_noise(int x, int y)
{
  double result = 0.0;
  result += sin(x / 300.0) * 12.7;
  result += cos(y / 67.3) * 6.3;
  result += cos(x / 14.9) * 1.4;
  return result;
}

bool world_chunk_gen3i(World *world, Vector3i *pos, uint64_t seed)
{
#ifdef DEBUG
  fprintf(stderr, "[WORLD] new chunk <%d, %d, %d> seed %d\n",
	  (int) pos->x, (int) pos->y, (int) pos->z, (int) seed);
#endif
  int i = 0;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    if(vec_equal3i(&chunk->pos, pos)) {
#ifdef DEBUG
      fprintf(stderr, "[WORLD] that chunk already exists\n");
#endif
      return false;
    }
    chunk = world->chunks[++i];
  }

  chunk = malloc(sizeof(Chunk));
  chunk_init(chunk, pos);
  
  for(int x=0;x<16;++x) {
    for(int y=0;y<16;++y) {
      for(int z=0;z<16;++z) {
	int absx = x + (pos->x * 16);
	int absy = y + (pos->y * 16);
	int absz = z + (pos->z * 16);
	int index = x + (y * 16) + (z * 256);

	double noise = world_noise(absx, absz);
	if(absy > noise) {continue;}
	char *block = absy == floor(noise) ? "block_tiled" : "block_rock";
	chunk->blocks[index] = world_mapping_get(world, block);
	chunk->changed = true;
	chunk->empty = false;
      }
    }
  }

#ifdef DEBUG
  fprintf(stderr, "[WORLD] new chunk is %p\n", chunk);
#endif
  world->chunks[i] = chunk;
  world->chunks[i+1] = NULL;
  return true;
}

bool world_chunk_gen3d(World *world, Vector3d *pos, uint64_t seed)
{
  Vector3i tmp;
  vec_set3i(&tmp, (int64_t) floor(pos->x), (int64_t) floor(pos->y), (int64_t) floor(pos->z));
  return world_chunk_gen3i(world, &tmp, seed);
}

bool world_chunk_delete3i(World *world, Vector3i *pos)
{
#ifdef DEBUG
  fprintf(stderr, "[WORLD] delete chunk <%d, %d, %d>\n", (int) pos->x, (int) pos->y, (int) pos->z);
#endif
  int i = 0, j = -1;
  Chunk *tmp = world->chunks[i];
  while(tmp != NULL) {
    if(j < 0 && vec_equal3i(&tmp->pos, pos)) {
#ifdef DEBUG
      fprintf(stderr, "[WORLD] deleting chunk %p\n", tmp);
#endif
      chunk_delete(tmp);
      free(tmp);
      j = i;
    }
    tmp = world->chunks[++i];
  }
  if(j < 0) {
#ifdef DEBUG
    fprintf(stderr, "[WORLD] that chunk doesn't exist\n");
#endif
    return false;
  }
  
  world->chunks[j] = world->chunks[i-1];
  world->chunks[i-1] = NULL;
  return true;
}

bool world_chunk_delete3d(World *world, Vector3d *pos)
{
  Vector3i tmp;
  vec_set3i(&tmp, (int64_t) floor(pos->x), (int64_t) floor(pos->y), (int64_t) floor(pos->z));
  return world_chunk_delete3i(world, &tmp);
}

Chunk *world_chunk_get3i(World *world, Vector3i *pos)
{
  int i = 0;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    if(vec_equal3i(pos, &chunk->pos)) {return chunk;}

    chunk = world->chunks[++i];
  }

  return NULL;
}

void world_chunk_adjacent(World *world, Chunk *chunk, Chunk **adjacent)
{
  Vector3i pos;
  for(int x=0;x<3;++x) {
    for(int y=0;y<3;++y) {
      for(int z=0;z<3;++z) {
	vec_set3i(&pos, x-1, y-1, z-1);
	vec_add3i(&pos, &pos, &chunk->pos);
	adjacent[x+(y*9)+(z*3)] = world_chunk_get3i(world, &pos);
      }
    }
  }
}

bool world_alloc_entities(World *world, EntityType etype, Model *model, int num)
{
  int i = 0;
  Entity *tmp = world->entities[i];
  while(tmp != NULL) {tmp = world->entities[++i];}

  int j = i + num;
  while(i < j) {
    Entity *entity = malloc(sizeof(Entity));
    entity_init(entity, etype, model);
    world->entities[i++] = entity;
  }
  world->entities[j] = NULL;
  return true;
}

bool world_alloc_players(World *world, int num)
{
  int i = 0;
  Entity *tmp = world->entities[i];
  while(tmp != NULL) {tmp = world->entities[++i];}

  int j = i + num;
  while(i < j) {
    Entity *player = malloc(sizeof(Player));
    player_init((Player *) player);
    world->entities[i++] = player;
  }
  world->entities[j] = NULL;
  return true;
}

bool world_del_entity(World *world, Entity *entity)
{
  int i = 0, j = -1;
  Entity *tmp = world->entities[i];
  while(tmp != NULL) {
    if(j < 0 && tmp == entity) {
      entity_delete(tmp);
      free(tmp);
      j = i;
    }
    tmp = world->entities[++i];
  }
  if(j < 0) {return false;}
  
  world->entities[j] = world->entities[i-1];
  world->entities[i-1] = NULL;
  return true;
}

bool world_del_player(World *world, Player *player)
{
  int i = 0, j = -1;
  Entity *tmp = world->entities[i];
  while(tmp != NULL) {
    if(j < 0 && tmp == (Entity *) player) {
      player_delete(player);
      free(player);
      j = i;
    }
    tmp = world->entities[++i];
  }
  if(j < 0) {return false;}
  
  world->entities[j] = world->entities[i-1];
  world->entities[i-1] = NULL;
  return true;
}

bool world_add_mapping(World *world, Block *mapping)
{
#ifdef DEBUG
  fprintf(stderr, "[WORLD] adding mapping for %s (%s)\n", mapping->name, mapping->str_id);
#endif
  int i = 0;
  Block *tmp = world->mappings[i];
  while(tmp != NULL) {
    if(block_equal(tmp, mapping)) {
#ifdef DEBUG
      fprintf(stderr, "[WORLD] that mapping already exists\n");
#endif
      return false;
    }
    tmp = world->mappings[++i];
  }
  
  world->mappings[i] = malloc(sizeof(Block));
  block_copy(world->mappings[i], mapping);
  world->mappings[i+1] = NULL;
  return true;
}

bool world_delete_mapping(World *world, Block *mapping)
{
#ifdef DEBUG
  fprintf(stderr, "[WORLD] deleting mapping for %s (%s)\n", mapping->name, mapping->str_id);
#endif
  int i = 0, j = -1;
  Block *tmp = world->mappings[i];
  while(tmp != NULL) {
    if(j < 0 && block_equal(tmp, mapping)) {
      block_delete(tmp);
      free(tmp);
      j = i;
    }
    tmp = world->mappings[++i];
  }
  if(j < 0) {
#ifdef DEBUG
    fprintf(stderr, "[WORLD] that mapping doesn't exist\n");
#endif
    return false;
  }
  
  world->mappings[j] = world->mappings[i-1];
  world->mappings[i-1] = NULL;
  return true;
}

Block * world_mapping_get(World *world, char *str_id)
{
  int i = 0;
  Block *mapping = world->mappings[i];
  while(mapping != NULL) {
    if(strcmp(mapping->str_id, str_id) == 0) {return mapping;}

    mapping = world->mappings[++i];
  }
  return NULL;
}

Block * world_block_get3i(World *world, Vector3i *pos)
{
  Vector3i tmp;
  vec_copy3i(&tmp, pos);
  if(pos->x < 0) {++tmp.x;}
  if(pos->y < 0) {++tmp.y;}
  if(pos->z < 0) {++tmp.z;}
  vec_div3i(&tmp, &tmp, 16);
  if(pos->x < 0) {--tmp.x;}
  if(pos->y < 0) {--tmp.y;}
  if(pos->z < 0) {--tmp.z;}
#ifdef DEBUG
  fprintf(stderr, "[WORLD] block requested from chunk <%d, %d, %d>\n",
	  (int) tmp.x, (int) tmp.y, (int) tmp.z);
#endif
  
  int i = 0;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    if(vec_equal3i(&chunk->pos, &tmp)) {
      vec_mod3i(&tmp, pos, 16);
      return chunk_block_get(chunk, &tmp);
    }
    chunk = world->chunks[++i];
  }
#ifdef DEBUG
  fprintf(stderr, "[WORLD] block requested from non-existent chunk <%d, %d, %d>\n",
	  (int) tmp.x, (int) tmp.y, (int) tmp.z);
#endif
  return NULL;
}

Block * world_block_get3d(World *world, Vector3d *pos)
{	  
  Vector3i tmp;
  vec_set3i(&tmp, (int64_t) floor(pos->x), (int64_t) floor(pos->y), (int64_t) floor(pos->z));
  return world_block_get3i(world, &tmp);
}

void world_block_set3i(World *world, Vector3i *pos, Block *block)
{
  Vector3i tmp;
  vec_copy3i(&tmp, pos);
  if(pos->x < 0) {++tmp.x;}
  if(pos->y < 0) {++tmp.y;}
  if(pos->z < 0) {++tmp.z;}
  vec_div3i(&tmp, &tmp, 16);
  if(pos->x < 0) {--tmp.x;}
  if(pos->y < 0) {--tmp.y;}
  if(pos->z < 0) {--tmp.z;}
  
  int i = 0;
 postgen: ;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    if(vec_equal3i(&chunk->pos, &tmp)) {
      vec_mod3i(&tmp, pos, 16);
      chunk_block_set(chunk, &tmp, block);
      return;
    }
    chunk = world->chunks[++i];
  }
  
  world_chunk_gen3i(world, &tmp, 0);
  goto postgen;
}

void world_block_set3d(World *world, Vector3d *pos, Block *block)
{
  Vector3i tmp;
  vec_set3i(&tmp, (int64_t) floor(pos->x), (int64_t) floor(pos->y), (int64_t) floor(pos->z));
  return world_block_set3i(world, &tmp, block);
}

void world_draw(World *world)
{
  mstack_push(mstack, mview_matrix);
  int i = 0;
  Chunk *chunk = world->chunks[i];
  while(chunk != NULL) {
    Vector3d tmp;
    vec_set3d(&tmp, chunk->pos.x*-16.0, chunk->pos.y*-16.0, chunk->pos.z*-16.0);
    vec_add3d(&tmp, &tmp, &world->entities[0]->pos);
    if(sqrt((tmp.x*tmp.x)+(tmp.y*tmp.y)+(tmp.z*tmp.z)) > 100.0) {goto next;}
    
    matrix_translate(mview_matrix, chunk->pos.x*16.0, chunk->pos.y*16.0, chunk->pos.z*16.0);
    chunk_draw(chunk, world);
    mstack_pick(mstack, mview_matrix, 0);

  next:
    chunk = world->chunks[++i];
  }
  mstack_pop(mstack, mview_matrix);
}
