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
  Block *blocks;
  Object *objects;
  Model *models;
  Entity *entities;
  Player *players;
  Action *actions;
} Module;

#endif /* MODULE_H */
