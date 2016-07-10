#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include "world.h"

double phys_update(World *world, double tmax);
bool phys_trace(World *world, Entity *player, double dmax, Vector3d *result);
bool phys_is_grounded(World *world, Entity *entity);

#endif /* PHYSICS_H */
