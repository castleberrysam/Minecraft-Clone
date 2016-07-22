#include <stdlib.h>
#include "list.h"

void list_init(Node *list)
{
  list->object = NULL;
  list->hash = 0;
  list->next = NULL;
}

void list_delete(Node *list)
{
  Node *node = list->next;
  while(node != NULL) {
    Node *tmp = node->next;
    free(node);
    node = tmp;
  }
  free(list);
}

void list_add(Node *list, void *object, int hash)
{
  Node *node = list;
  while(node->next != NULL && node->next->hash < hash) {node = node->next;}
  Node *new = malloc(sizeof(Node));
  new->object = object;
  new->hash = hash;
  new->next = node->next;
  node->next = new;
}

void list_remove(Node *list, int hash)
{
  Node *node = list;
  while(node->next != NULL && node->next->hash < hash) {node = node->next;}
  if(node->next == NULL || node->next->hash != hash) {return;}
  Node *tmp = node->next->next;
  free(node->next);
  node->next = tmp;
}

void * list_get(Node *list, int hash)
{
  Node *node = list->next;
  while(node != NULL && node->hash < hash) {node = node->next;}
  return node->hash == hash ? node->object : NULL;
}
