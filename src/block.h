#ifndef BLOCK_H
#define BLOCK_H

#include "entity.h"
#include <stdint.h>

typedef struct {
  Model model;
  uint16_t num_id;
  char *str_id;
  char *name;
} Block;

void block_init(Block *block, Model *model, uint16_t num_id, char *str_id, char *name);
void block_delete(Block *block);
void block_copy(Block *new, Block *old);
bool block_equal(Block *left, Block *right);

#endif /* BLOCK_H */
