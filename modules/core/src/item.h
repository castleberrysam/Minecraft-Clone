#ifndef ITEM_H
#define ITEM_H

#include "block.h"
#include <stdint.h>

typedef struct Item {
  uint32_t quantity;
  Block *block;
} Item;

#endif /* ITEM_H */
