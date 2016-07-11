#ifndef MODULE_H
#define MODULE_H

#include "block.h"
#include "entity.h"
#include "player.h"
#include <stdbool.h>

typedef struct {
  void *library;
  bool loaded;
  char *name;
  char *author;
  char *version;
  Block **blocks;
  Model **models;
  Entity **entities;
  Action **actions;
} Module;

#endif /* MODULE_H */
