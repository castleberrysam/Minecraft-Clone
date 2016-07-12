#include <stdlib.h>
#include <string.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "module.h"

void module_init(Module *module, char *filename)
{
  module->loaded = false;
  module->filename = malloc(strlen(filename)+1);
  strcpy(module->filename, filename);
}

void module_delete(Module *module)
{
  if(module->loaded) {module_unload(module);}
  free(module->filename);
}

bool module_load(Module *module)
{
  if(module->loaded) {return true;}
#ifdef __MINGW32__
  module->library = LoadLibrary(TEXT(module->filename));
  if(module->library == NULL) {return false;}
  module->get_name = (get_str_func) GetProcAddress(module->library, TEXT("module_get_name"));
  if(module->get_name == NULL) {goto cleanup;}
  module->get_author = (get_str_func) GetProcAddress(module->library, TEXT("module_get_author"));
  if(module->get_author == NULL) {goto cleanup;}
  module->get_ver = (get_str_func) GetProcAddress(module->library, TEXT("module_get_ver"));
  if(module->get_ver == NULL) {goto cleanup;}
  module->get_blocks = (get_b_arr_func) GetProcAddress(module->library, TEXT("module_get_blocks"));
  module->get_models = (get_m_arr_func) GetProcAddress(module->library, TEXT("module_get_models"));
  module->get_entities = (get_e_arr_func) GetProcAddress(module->library, TEXT("module_get_entities"));
  module->get_actions = (get_a_arr_func) GetProcAddress(module->library, TEXT("module_get_actions"));
  module->loaded = true;
  return true;
 cleanup:
  FreeLibrary(module->library);
  return false;
#else
  module->library = dlopen(module->filename, RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);
  if(module->library == NULL) {return false;}
  module->get_name = dlsym(module->library, "module_get_name");
  if(module->get_name == NULL) {goto cleanup;}
  module->get_author = dlsym(module->library, "module_get_author");
  if(module->get_author == NULL) {goto cleanup;}
  module->get_ver = dlsym(module->library, "module_get_ver");
  if(module->get_ver == NULL) {goto cleanup;}
  module->get_blocks = dlsym(module->library, "module_get_blocks");
  module->get_models = dlsym(module->library, "module_get_models");
  module->get_entities = dlsym(module->library, "module_get_entities");
  module->get_actions = dlsym(module->library, "module_get_actions");
  module->loaded = true;
  return true;
 cleanup:
  module_unload(module);
  return false;
#endif
}

bool module_unload(Module *module)
{
  if(!module->loaded) {return true;}
#ifdef __MINGW32__
  return FreeLibrary(module->library);
#else
  return dlclose(module->library);
#endif
}
