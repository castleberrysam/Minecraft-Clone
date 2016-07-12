#include <stdio.h>
#include <tinydir.h>
#include "texture.h"
#include "block.h"
#include "model.h"
#include "entity.h"
#include "action.h"

Block **blocks = NULL;

char * module_get_name(void)
{
  return "Basic Terrain Blocks";
}

char * module_get_author(void)
{
  return "Blackjak34";
}

char * module_get_ver(void)
{
  return "1.0.0";
}

Block ** module_get_blocks(void)
{
  return blocks;
}

__attribute__((constructor)) void load(void)
{
  int num_blocks = 1;
  blocks = malloc(num_blocks*sizeof(Block *));
  blocks[num_blocks-1] = NULL;
  
  tinydir_dir dir;
#ifdef __MINGW32__
  tinydir_open(&dir, "modules\\terrain\\res");
#else
  tinydir_open(&dir, "modules/terrain/res");
#endif
  while(dir.has_next) {
    tinydir_file file;
    tinydir_readfile(&dir, &file);
    
    int len = strlen(file.name);
    if(file.is_dir || len < 5 ||
       strcmp(&file.name[len-4], ".png") != 0) {goto next;}
    
    GLuint texture = load_texture(file.path);
    if(texture == 0) {goto next;}

    Model model;
    model_init(&model, DISPLAY_LIST);
    model_gen_list(&model, render_cube, &texture);
    char str_id[69];
    snprintf(str_id, 64, "block_%s", file.name);
    str_id[strlen(str_id)-5] = '\0';
    Block *block = malloc(sizeof(Block));
    block_init(block, &model, num_blocks, str_id, str_id);
    model_delete(&model);
    
    ++num_blocks;
    blocks = realloc(blocks, num_blocks*sizeof(Block *));
    blocks[num_blocks-1] = NULL;
    blocks[num_blocks-2] = block;

  next:
    tinydir_next(&dir);
  }

  tinydir_close(&dir);
}

__attribute__((destructor)) void unload(void)
{
  int i = 0;
  Block *block = blocks[i];
  while(block != NULL) {
    block_delete(block);
    free(block);
    
    block = blocks[++i];
  }
  free(blocks);
  blocks = NULL;
}
