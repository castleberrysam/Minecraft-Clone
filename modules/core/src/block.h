#ifndef BLOCK_H
#define BLOCK_H

#include <GL/glew.h>
#include <stdint.h>
#include "entity.h"

typedef struct {
  GLint texture;
  char *str_id;
  char *name;
} Block;

void block_init(Block *block, GLint texture, char *str_id, char *name);
void block_delete(Block *block);
void block_copy(Block *new, Block *old);
bool block_equal(Block *left, Block *right);

#endif /* BLOCK_H */
