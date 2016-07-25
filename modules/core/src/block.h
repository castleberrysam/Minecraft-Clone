#ifndef BLOCK_H
#define BLOCK_H

#include <GL/glew.h>
#include <stdbool.h>
#include <stdint.h>
#include "entity.h"
#include "vector.h"

typedef struct World World;
typedef struct Player Player;

typedef enum Side {
  NORTH,
  EAST,
  SOUTH,
  WEST,
  TOP,
  BOTTOM
} Side;

typedef void (*block_create)(World *world, Player *player, Vector3i *pos);
typedef void (*block_destroy)(World *world, Player *player, Vector3i *pos);
typedef void (*block_tick)(World *world, Vector3i *pos);
typedef void (*block_collide)(World *world, Entity *entity, Vector3d *pos);
typedef bool (*block_activate)(World *world, Player *player, Vector3d *pos);

typedef struct Block {
  GLuint texture;
  char *str_id;
  char *name;
  bool tickable;
  block_create create;
  block_destroy destroy;
  block_tick tick;
  block_collide collide;
  block_activate activate;
} Block;

void block_init(Block *block, GLint texture, char *str_id, char *name, bool tickable);
void block_delete(Block *block);
void block_copy(Block *new, Block *old);
bool block_equal(Block *left, Block *right);
Side block_side_get(Vector3d *pos);

#endif /* BLOCK_H */
