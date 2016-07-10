#include <stdlib.h>
#include "inventory.h"

void inv_init(Inventory *inv, uint64_t size)
{
  inv->items = malloc(sizeof(Item *) * size);
}

void inv_delete(Inventory *inv)
{
  free(inv->items);
}
