#ifndef ACTION_H
#define ACTION_H

typedef enum {
  PRESS,
  RELEASE,
  BOTH
} action_type;

typedef struct {
  int num_keys;
  unsigned char *keys;
  action_type type;
  void (*callback)(unsigned char key, action_type type);
} action;

#endif /* ACTION_H */
