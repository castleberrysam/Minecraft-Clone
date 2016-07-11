#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "chunk.h"

void chunk_init(Chunk *chunk, Vector3i *pos)
{
  model_init((Model *) chunk, DISPLAY_LIST);
  chunk->changed = true;
  chunk->blocks = malloc(sizeof(Block *) * 4096);
  vec_copy3i(&chunk->pos, pos);
}

void chunk_delete(Chunk *chunk)
{
  model_delete((Model *) chunk);
  free(chunk->blocks);
}

inline Block * chunk_block_get(Chunk *chunk, Vector3i *pos)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  return chunk->blocks[index];
}

void chunk_block_set(Chunk *chunk, Vector3i *pos, Block *block)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  chunk->changed = !block_equal(chunk->blocks[index], block);
  chunk->blocks[index] = block;
}

void chunk_block_delete(Chunk *chunk, Vector3i *pos)
{
  int index = pos->x+(pos->y*16)+(pos->z*256);
  chunk->changed = chunk->blocks[index] != NULL;
  chunk->blocks[index] = NULL;
}

static void chunk_draw_internal(void *user)
{
  Chunk *chunk = user;
  for(unsigned x=0;x<16;++x) {
    for(unsigned y=0;y<16;++y) {
      for(unsigned z=0;z<16;++z) {
	Vector3i tmp;
	vec_set3i(&tmp, x, y, z);
	Model *block = (Model *) chunk_block_get(chunk, &tmp);
	if(block == NULL) {continue;}

	if(x > 0 && x < 15 &&
	   y > 0 && y < 15 &&
	   z > 0 && z < 15) {
	  vec_set3i(&tmp, x-1, y, z);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}
	  vec_set3i(&tmp, x+1, y, z);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}
	  vec_set3i(&tmp, x, y-1, z);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}
	  vec_set3i(&tmp, x, y+1, z);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}
	  vec_set3i(&tmp, x, y, z-1);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}
	  vec_set3i(&tmp, x, y, z+1);
	  if(chunk_block_get(chunk, &tmp) == NULL) {goto no_optimize;}

	  continue;
	}
      no_optimize:
	
	glPushMatrix();
	glTranslated((double) x, (double) y, (double) z);
	model_draw(block);
	glPopMatrix();
      }
    }
  }
}

void chunk_draw(Chunk *chunk)
{
  if(chunk->changed) {
    model_gen_list((Model *) chunk, chunk_draw_internal, (void *) chunk);
    chunk->changed = false;
  }
  
  model_draw((Model *) chunk);
}
