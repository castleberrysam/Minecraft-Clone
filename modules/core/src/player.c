#include <stdlib.h>
#include <stdio.h>
#include "player.h"

static Model *player_model = NULL;

static void player_gen_model(void)
{
  player_model = malloc(sizeof(Model));
  model_init(player_model, 0, 0, 0, GL_POINTS, 0);
}

void player_init(Player *player)
{
  if(player_model == NULL) {player_gen_model();}
  
  entity_init((Entity *) player, PLAYER, player_model);
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

void player_draw(Player *player)
{
  model_draw(player_model);
}
