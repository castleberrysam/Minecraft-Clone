#include <stdlib.h>
#include <string.h>
#include "block.h"

void block_init(Block *block, GLint texture, char *str_id, char *name)
{
  block->texture = texture;
  block->str_id = malloc(strlen(str_id)+1);
  strcpy(block->str_id, str_id);
  block->name = malloc(strlen(name)+1);
  strcpy(block->name, name);
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
}

bool block_equal(Block *left, Block *right)
{
  if(left == NULL) {return right == NULL;}
  if(right == NULL) {return false;}
  return strcmp(left->str_id, right->str_id) == 0;
}
