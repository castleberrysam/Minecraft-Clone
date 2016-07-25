#ifndef MODULE_H
#define MODULE_H

#include <stdbool.h>
#include "block.h"
#include "model.h"
#include "entity.h"
#include "action.h"

typedef char * (*get_str_func)(void);
typedef Block ** (*get_b_arr_func)(void);
typedef Model ** (*get_m_arr_func)(void);
typedef Entity ** (*get_e_arr_func)(void);
typedef Action ** (*get_a_arr_func)(void);

typedef struct Module {
  void *library;
  bool loaded;
  char *filename;
  get_str_func get_name;
  get_str_func get_author;
  get_str_func get_ver;
  get_b_arr_func get_blocks;
  get_m_arr_func get_models;
  get_e_arr_func get_entities;
  get_a_arr_func get_actions;
} Module;

void module_init(Module *module, char *filepath);
void module_delete(Module *module);
bool module_load(Module *module);
bool module_unload(Module *module);

#endif /* MODULE_H */
