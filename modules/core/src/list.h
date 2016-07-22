#ifndef LIST_H
#define LIST_H

typedef struct Node {
  void *object;
  int hash;
  struct Node *next;
} Node;

void list_init(Node *list);
void list_delete(Node *list);
void list_add(Node *list, void *object, int hash);
void list_remove(Node *list, int hash);
void * list_get(Node *list, int hash);

#endif /* LIST_H */
