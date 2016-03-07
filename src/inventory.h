#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

typedef struct {
  int num_items;
  item *items;
} inventory;

#endif /* INVENTORY_H */
