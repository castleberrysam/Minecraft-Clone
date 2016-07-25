#include <stdio.h>
#include <tinydir.h>
#include "texture.h"
#include "block.h"
#include "sound.h"

static Block **blocks = NULL;
static Sound sound_bounce;

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

static void collide_bounce(World *world, Entity *entity, Vector3d *pos)
{
  if(block_side_get(pos) != TOP || entity->vel.y > -0.1) {return;}
  entity->vel.y *= -1.1;
  sound_play_static(&sound_bounce, pos);
}

__attribute__((constructor)) void load(void)
{
  sound_init(&sound_bounce, "modules/terrain/res/bounce.ogg", 1.0, 1.0);
  
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
    
    GLint texture = texture_load(file.path);
    if(texture == -1) {goto next;}
    
    char str_id[69];
    snprintf(str_id, 64, "block_%s", file.name);
    str_id[strlen(str_id)-4] = '\0';
    Block *block = malloc(sizeof(Block));
    block_init(block, texture, str_id, str_id, false);
    if(strcmp(str_id, "block_trampoline") == 0) {
      block->collide = collide_bounce;
    }
    
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
