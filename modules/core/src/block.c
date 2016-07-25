#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "block.h"
#include "physics.h"

static void default_create(World *world, Player *player, Vector3i *pos)
{
  ;
}

static void default_destroy(World *world, Player *player, Vector3i *pos)
{
  ;
}

static void default_tick(World *world, Vector3i *pos)
{
  ;
}

static void default_collide(World *world, Entity *entity, Vector3d *pos)
{
  ;
}

static bool default_activate(World *world, Player *player, Vector3d *pos)
{
  return false;
}

void block_init(Block *block, GLint texture, char *str_id, char *name, bool tickable)
{
  block->texture = texture;
  block->str_id = malloc(strlen(str_id)+1);
  strcpy(block->str_id, str_id);
  block->name = malloc(strlen(name)+1);
  strcpy(block->name, name);
  block->tickable = tickable;
  block->create = default_create;
  block->destroy = default_destroy;
  block->tick = default_tick;
  block->collide = default_collide;
  block->activate = default_activate;
}

void block_delete(Block *block)
{
  free(block->str_id);
  free(block->name);
}

void block_copy(Block *new, Block *old)
{
  new->texture = old->texture;
  new->str_id = malloc(strlen(old->str_id)+1);
  strcpy(new->str_id, old->str_id);
  new->name = malloc(strlen(old->name)+1);
  strcpy(new->name, old->name);
  new->tickable = old->tickable;
  new->create = old->create;
  new->destroy = old->destroy;
  new->tick = old->tick;
  new->collide = old->collide;
  new->activate = old->activate;
}

bool block_equal(Block *left, Block *right)
{
  if(left == NULL) {return right == NULL;}
  if(right == NULL) {return false;}
  return strcmp(left->str_id, right->str_id) == 0;
}

Side block_side_get(Vector3d *pos)
{
  if(ABS(pos->x - ceil(pos->x)) <= PHYS_TOL*1.1) {return EAST;}
  if(ABS(pos->x - floor(pos->x)) <= PHYS_TOL*1.1) {return WEST;}
  if(ABS(pos->y - ceil(pos->y)) <= PHYS_TOL*1.1) {return TOP;}
  if(ABS(pos->y - floor(pos->y)) <= PHYS_TOL*1.1) {return BOTTOM;}
  if(ABS(pos->z - ceil(pos->z)) <= PHYS_TOL*1.1) {return SOUTH;}
  return NORTH;
}
