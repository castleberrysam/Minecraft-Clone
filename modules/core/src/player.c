#include <stdlib.h>
#include <stdio.h>
#include "player.h"

static Model *player_model = NULL;

static void player_draw(void *user)
{
  ;
}

static void player_gen_model(void)
{
#ifdef DEBUG
  fprintf(stderr, "[PLAYR] generating model\n");
#endif
  player_model = malloc(sizeof(Model));
  model_init(player_model, DISPLAY_LIST);
  model_gen_list(player_model, player_draw, NULL);
}

void player_init(Player *player)
{
  if(player_model == NULL) {player_gen_model();}
  
  entity_init(&player->entity, PLAYER, player_model);
  player->inv = malloc(sizeof(Inventory));
  inv_init(player->inv, 54);
  player->hp = 100;
  player->speed = 3.0;
  player->jump_height = 1.1;
}

void player_delete(Player *player)
{
  entity_delete(&player->entity);
  inv_delete(player->inv);
  free(player->inv);
}
