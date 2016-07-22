#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int64_t x;
  int64_t y;
} Vector2i;

typedef struct {
  double x;
  double y;
} Vector2d;

typedef struct {
  int64_t x;
  int64_t y;
  int64_t z;
} Vector3i;

typedef struct {
  double x;
  double y;
  double z;
} Vector3d;

typedef struct {
  int64_t x;
  int64_t y;
  int64_t z;
  int64_t w;
} Vector4i;

typedef struct {
  double x;
  double y;
  double z;
  double w;
} Vector4d;

static inline void vec_set3i(Vector3i *out, int64_t x, int64_t y, int64_t z)
{
  out->x = x;
  out->y = y;
  out->z = z;
}

static inline void vec_set3d(Vector3d *out, double x, double y, double z)
{
  out->x = x;
  out->y = y;
  out->z = z;
}

static inline void vec_add3i(Vector3i *out, Vector3i *left, Vector3i *right)
{
  out->x = left->x + right->x;
  out->y = left->y + right->y;
  out->z = left->z + right->z;
}

static inline void vec_add3d(Vector3d *out, Vector3d *left, Vector3d *right)
{
  out->x = left->x + right->x;
  out->y = left->y + right->y;
  out->z = left->z + right->z;
}

static inline void vec_sub3i(Vector3i *out, Vector3i *left, Vector3i *right)
{
  out->x = left->x - right->x;
  out->y = left->y - right->y;
  out->z = left->z - right->z;
}

static inline void vec_sub3d(Vector3i *out, Vector3i *left, Vector3i *right)
{
  out->x = left->x - right->x;
  out->y = left->y - right->y;
  out->z = left->z - right->z;
}

static inline void vec_scale3i(Vector3i *out, Vector3i *vec, int64_t scale)
{
  out->x = vec->x * scale;
  out->y = vec->y * scale;
  out->z = vec->z * scale;
}

static inline void vec_scale3d(Vector3d *out, Vector3d *vec, double scale)
{
  out->x = vec->x * scale;
  out->y = vec->y * scale;
  out->z = vec->z * scale;
}

static inline void vec_div3i(Vector3i *out, Vector3i *vec, int64_t divisor)
{
  out->x = vec->x / divisor;
  out->y = vec->y / divisor;
  out->z = vec->z / divisor;
}

static inline void vec_mod3i(Vector3i *out, Vector3i *vec, int64_t divisor)
{
  out->x = vec->x % divisor;
  out->y = vec->y % divisor;
  out->z = vec->z % divisor;
  if(out->x < 0) {out->x += divisor;}
  if(out->y < 0) {out->y += divisor;}
  if(out->z < 0) {out->z += divisor;}
}

static inline void vec_copy3i(Vector3i *new, Vector3i *old)
{
  new->x = old->x;
  new->y = old->y;
  new->z = old->z;
}

static inline void vec_copy3d(Vector3d *new, Vector3d *old)
{
  new->x = old->x;
  new->y = old->y;
  new->z = old->z;
}

static inline bool vec_equal3i(Vector3i *left, Vector3i *right)
{
  if(left->x != right->x) {return false;}
  if(left->y != right->y) {return false;}
  if(left->z != right->z) {return false;}
  return true;
}

static inline bool vec_equal3d(Vector3d *left, Vector3d *right)
{
  if(left->x != right->x) {return false;}
  if(left->y != right->y) {return false;}
  if(left->z != right->z) {return false;}
  return true;
}

#endif /* VECTOR_H */
