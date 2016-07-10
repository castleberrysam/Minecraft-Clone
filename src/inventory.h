#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

typedef struct {
  Item **items;
} Inventory;

void inv_init(Inventory *inv, uint64_t size);
void inv_delete(Inventory *inv);

#endif /* INVENTORY_H */
