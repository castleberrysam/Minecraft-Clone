#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include "world.h"

static const double PI = 3.14159265;
static const double PHYS_TOL = 0.000000001;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) > 0.0 ? (a) : -(a))
#define RADS(angle) (((angle) * PI) / 180.0)

double phys_update(World *world, double tmax);
bool phys_trace(World *world, Entity *player, double dmax, Vector3d *result);
bool phys_is_grounded(World *world, Entity *entity);
bool phys_aabb_intersect(Vector3d *off1, Vector3d *dim1, Vector3d *off2, Vector3d *dim2);

#endif /* PHYSICS_H */
