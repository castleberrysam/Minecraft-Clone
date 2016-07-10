#ifndef ACTION_H
#define ACTION_H

#include <stdbool.h>

typedef enum {
  PRESS,
  RELEASE,
  BOTH
} ActionType;

typedef struct {
  unsigned char key;
  bool case_sens;
  ActionType type;
  void (*callback)(unsigned char key, ActionType type);
} Action;

bool action_reg(Action action);
void action_unreg(Action action);
void action_unreg_key(unsigned char key);
void action_unreg_all(void);

#endif /* ACTION_H */
