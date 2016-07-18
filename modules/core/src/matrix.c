#include <GL/glew.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "matrix.h"

MatrixStack *mstack = NULL;
GLfloat proj_matrix[16];
GLfloat mview_matrix[16];

static void matrix_clear(GLfloat *matrix)
{
  for(int i=0;i<16;++i) {
    matrix[i] = 0.0f;
  }
}

void matrix_identity(GLfloat *matrix)
{
  matrix_clear(matrix);
  matrix[0] = 1.0f;
  matrix[5] = 1.0f;
  matrix[10] = 1.0f;
  matrix[15] = 1.0f;
}

void matrix_ortho(GLfloat *matrix, double width, double height, double near, double far)
{
  matrix_clear(matrix);
  matrix[0] = (GLfloat) (2 / width);
  matrix[5] = (GLfloat) (2 / height);
  matrix[10] = (GLfloat) (2 / (near - far));
  matrix[12] = -1.0f;
  matrix[13] = -1.0f;
  matrix[14] = ((GLfloat) ((far + near) / (near - far)));
  matrix[15] = 1.0f;
}

void matrix_perspective(GLfloat *matrix, double fov, double ratio, double near, double far)
{
  double f = 1 / tan(fov / 2);
  matrix_clear(matrix);
  matrix[0] = (GLfloat) (f / ratio);
  matrix[5] = (GLfloat) f;
  matrix[10] = (GLfloat) ((far + near) / (near - far));
  matrix[11] = -1.0f;
  matrix[14] = (GLfloat) ((2 * far * near) / (near - far));
}

void matrix_multiply(GLfloat *out, GLfloat *left, GLfloat *right)
{
  GLfloat tmp[16];
  matrix_clear(tmp);
  for(int x=0;x<4;++x) {
    for(int y=0;y<4;++y) {
      for(int i=0;i<4;++i) {
	tmp[y+(x*4)] += left[y+(i*4)] * right[i+(x*4)];
      }
    }
  }
  matrix_copy(out, tmp);
}

void matrix_translate(GLfloat *matrix, double x, double y, double z)
{
  GLfloat trans[16];
  matrix_identity(trans);
  trans[12] = (GLfloat) x;
  trans[13] = (GLfloat) y;
  trans[14] = (GLfloat) z;
  matrix_multiply(matrix, matrix, trans);
}

void matrix_rotate_x(GLfloat *matrix, double angle)
{
  GLfloat s = (GLfloat) sin(angle);
  GLfloat c = (GLfloat) cos(angle);
  GLfloat rot[16];
  matrix_clear(rot);
  rot[0] = 1.0f;
  rot[5] = c;
  rot[6] = s;
  rot[9] = -s;
  rot[10] = c;
  rot[15] = 1.0f;
  matrix_multiply(matrix, matrix, rot);
}

void matrix_rotate_y(GLfloat *matrix, double angle)
{
  GLfloat s = (GLfloat) sin(angle);
  GLfloat c = (GLfloat) cos(angle);
  GLfloat rot[16];
  matrix_clear(rot);
  rot[0] = c;
  rot[2] = -s;
  rot[5] = 1.0f;
  rot[8] = s;
  rot[10] = c;
  rot[15] = 1.0f;
  matrix_multiply(matrix, matrix, rot);
}

void matrix_rotate_z(GLfloat *matrix, double angle)
{
  GLfloat s = (GLfloat) sin(angle);
  GLfloat c = (GLfloat) cos(angle);
  GLfloat rot[16];
  matrix_clear(rot);
  rot[0] = c;
  rot[1] = s;
  rot[4] = -s;
  rot[5] = c;
  rot[10] = 1.0f;
  rot[15] = 1.0f;
  matrix_multiply(matrix, matrix, rot);
}

void matrix_scale(GLfloat *matrix, double scale)
{
  matrix[0] *= (GLfloat) scale;
  matrix[5] *= (GLfloat) scale;
  matrix[10] *= (GLfloat) scale;
}

void matrix_copy(GLfloat *new, GLfloat *old)
{
  for(int i=0;i<16;++i) {
    new[i] = old[i];
  }
}

void mstack_init(MatrixStack *stack, int max)
{
  stack->matrices = malloc(sizeof(GLfloat *) * max);
  stack->max = max;
  stack->depth = 0;
}

void mstack_delete(MatrixStack *stack)
{
  while(stack->depth > 0) {
    --stack->depth;
    free(stack->matrices[stack->depth]);
  }
}

bool mstack_push(MatrixStack *stack, GLfloat *matrix)
{  
  if(stack->depth == stack->max) {return false;}
  stack->matrices[stack->depth] = malloc(sizeof(GLfloat) * 16);
  matrix_copy(stack->matrices[stack->depth], matrix);
  ++stack->depth;
  return true;
}

bool mstack_pop(MatrixStack *stack, GLfloat *matrix)
{
  if(stack->depth == 0) {return false;}
  --stack->depth;
  matrix_copy(matrix, stack->matrices[stack->depth]);
  free(stack->matrices[stack->depth]);
  return true;
}

bool mstack_pick(MatrixStack *stack, GLfloat *matrix, int depth)
{
  if(depth >= stack->depth) {return false;}
  matrix_copy(matrix, stack->matrices[stack->depth-depth-1]);
  return true;
}

bool mstack_replace(MatrixStack *stack, GLfloat *matrix, int depth)
{
  if(depth >= stack->depth) {return false;}
  matrix_copy(stack->matrices[stack->depth-depth-1], matrix);
  return true;
}

void matrix_static_init(void)
{
  mstack = malloc(sizeof(MatrixStack));
  mstack_init(mstack, 16);
}

void matrix_update_mvp(GLint location)
{
  GLfloat tmp[16];
  matrix_multiply(tmp, proj_matrix, mview_matrix);
  glUniformMatrix4fv(location, 1, GL_FALSE, tmp);
}
