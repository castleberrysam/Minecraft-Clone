#ifndef MATRIX_H
#define MATRIX_H

#include <GL/glew.h>
#include <stdbool.h>

typedef struct {
  GLfloat **matrices;
  int max;
  int depth;
} MatrixStack;

extern MatrixStack *mstack;
extern GLfloat proj_matrix[16];
extern GLfloat mview_matrix[16];

void matrix_identity(GLfloat *matrix);
void matrix_ortho(GLfloat *matrix, double width, double height, double near, double far);
void matrix_perspective(GLfloat *matrix, double fov, double ratio, double near, double far);
void matrix_multiply(GLfloat *out, GLfloat *left, GLfloat *right);
void matrix_translate(GLfloat *matrix, double x, double y, double z);
void matrix_rotate_x(GLfloat *matrix, double angle);
void matrix_rotate_y(GLfloat *matrix, double angle);
void matrix_rotate_z(GLfloat *matrix, double angle);
void matrix_scale(GLfloat *matrix, double scale);
void matrix_copy(GLfloat *new, GLfloat *old);

void mstack_init(MatrixStack *stack, int max);
void mstack_delete(MatrixStack *stack);
bool mstack_push(MatrixStack *stack, GLfloat *matrix);
bool mstack_pop(MatrixStack *stack, GLfloat *matrix);
bool mstack_pick(MatrixStack *stack, GLfloat *matrix, int depth);
bool mstack_replace(MatrixStack *stack, GLfloat *matrix, int depth);

void matrix_static_init(void);
void matrix_update_mvp(GLint location);

#endif /* MATRIX_H */
