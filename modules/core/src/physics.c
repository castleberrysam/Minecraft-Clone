#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "physics.h"

static double phys_time_target(double pos, double vel)
{
  if(vel > 0.0) {
    return ((ceil(pos) + PHYS_TOL) - pos) / vel;
  } else if(vel < 0.0) {
    return ((floor(pos) - PHYS_TOL) - pos) / vel;
  } else {
    return DBL_MAX;
  }
}

static double phys_time_intersect(double pos, double vel)
{
  if(vel > 0.0) {
    return (ceil(pos) - pos) / vel;
  } else if(vel < 0.0) {
    return (floor(pos) - pos) / vel;
  } else {
    return DBL_MAX;
  }
}

static bool phys_intersect(World *world, Vector3d *pos, Vector3d *vel, double t, Vector3d *where)
{
  Vector3d tmp;
  if(where == NULL) {where = &tmp;}
  vec_copy3d(where, vel);
  vec_scale3d(where, where, t);
  vec_add3d(where, where, pos);
  Block *block = world_block_get3d(world, where);
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] checking intersect with block %p <%d, %d, %d>\n",
	  block, (int) floor(where->x), (int) floor(where->y), (int) floor(where->z));
#endif
  return block != NULL;
}

static bool phys_intersect_x(World *world, Entity *entity, double t)
{
  Vector3d pos, tmp;
  vec_copy3d(&pos, &entity->pos);
  vec_add3d(&pos, &pos, &entity->bb_off);
  pos.x += entity->vel.x > 0.0 ? entity->bb_dim.x : -entity->bb_dim.x;

  double ymax = floor(entity->bb_dim.y);
  double zmax = floor(entity->bb_dim.z);
  for(double y=-entity->bb_dim.y;floor(y)<=ymax;++y) {
    for(double z=-entity->bb_dim.z;floor(z)<=zmax;++z) {
      y = MIN(y, entity->bb_dim.y);
      z = MIN(z, entity->bb_dim.z);
      vec_copy3d(&tmp, &pos);
      tmp.y += y;
      tmp.z += z;
      if(phys_intersect(world, &tmp, &entity->vel, t, NULL)) {
#ifdef DEBUG
	fprintf(stderr, "[PHYS ] collision on x\n");
#endif
	return true;
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] no collision on x\n");
#endif
  return false;
}

static bool phys_intersect_y(World *world, Entity *entity, double t)
{
  Vector3d pos, tmp;
  vec_copy3d(&pos, &entity->pos);
  vec_add3d(&pos, &pos, &entity->bb_off);
  pos.y += entity->vel.y > 0.0 ? entity->bb_dim.y : -entity->bb_dim.y;

  double xmax = floor(entity->bb_dim.x);
  double zmax = floor(entity->bb_dim.z);
  for(double x=-entity->bb_dim.x;floor(x)<=xmax;++x) {
    for(double z=-entity->bb_dim.z;floor(z)<=zmax;++z) {
      x = MIN(x, entity->bb_dim.x);
      z = MIN(z, entity->bb_dim.z);
      vec_copy3d(&tmp, &pos);
      tmp.x += x;
      tmp.z += z;
      if(phys_intersect(world, &tmp, &entity->vel, t, NULL)) {
#ifdef DEBUG
	fprintf(stderr, "[PHYS ] collision on y\n");
#endif
	return true;
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] no collision on y\n");
#endif
  return false;
}

static bool phys_intersect_z(World *world, Entity *entity, double t)
{
  Vector3d pos, tmp;
  vec_copy3d(&pos, &entity->pos);
  vec_add3d(&pos, &pos, &entity->bb_off);
  pos.z += entity->vel.z > 0.0 ? entity->bb_dim.z : -entity->bb_dim.z;

  double xmax = floor(entity->bb_dim.x);
  double ymax = floor(entity->bb_dim.y);
  for(double x=-entity->bb_dim.x;floor(x)<=xmax;++x) {
    for(double y=-entity->bb_dim.y;floor(y)<=ymax;++y) {
      x = MIN(x, entity->bb_dim.x);
      y = MIN(y, entity->bb_dim.y);
      vec_copy3d(&tmp, &pos);
      tmp.x += x;
      tmp.y += y;
      if(phys_intersect(world, &tmp, &entity->vel, t, NULL)) {
#ifdef DEBUG
	fprintf(stderr, "[PHYS ] collision on z\n");
#endif
	return true;
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] no collision on z\n");
#endif
  return false;
}

double phys_update(World *world, double tmax)
{
  double t = tmax;
  int i = 0;
  Entity *entity = world->entities[i];
  while(entity != NULL) {
#ifdef DEBUG
    fprintf(stderr, "[PHYS ] entity %p has pos <%.2f, %.2f, %.2f> and vel <%.2f, %.2f, %.2f>\n",
	    entity, entity->pos.x, entity->pos.y, entity->pos.z,
	    entity->vel.x, entity->vel.y, entity->vel.z);
#endif
    double x = entity->pos.x + entity->bb_off.x + (entity->vel.x > 0 ? entity->bb_dim.x : -entity->bb_dim.x);
    double y = entity->pos.y + entity->bb_off.y + (entity->vel.y > 0 ? entity->bb_dim.y : -entity->bb_dim.y);
    double z = entity->pos.z + entity->bb_off.z + (entity->vel.z > 0 ? entity->bb_dim.z : -entity->bb_dim.z);
    double tx = phys_time_target(x, entity->vel.x);
    double ty = phys_time_target(y, entity->vel.y);
    double tz = phys_time_target(z, entity->vel.z);
    double tx2 = phys_time_intersect(x, entity->vel.x);
    double ty2 = phys_time_intersect(y, entity->vel.y);
    double tz2 = phys_time_intersect(z, entity->vel.z);
#ifdef DEBUG
    fprintf(stderr, "[PHYS ] has intersection times <%.4f, %.4f, %.4f>\n",
	    tx > t ? t : tx, ty > t ? t : ty, tz > t ? t : tz);
#endif
    if(tx < ty) {
      if(ty < tz) { // tx, ty, tz
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
      } else if(tx < tz) { // tx, tz, ty
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
      } else { // tz, tx, ty
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
      }
    } else {
      if(tx < tz) { // ty, tx, tz
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
      } else if(ty < tz) { // ty, tz, tx
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
      } else { // tz, ty, tx
	if(tz2 > t) {goto next;}
	if(phys_intersect_z(world, entity, tz)) {
	  t = tz2;
	  entity->vel.z = 0.0;
	  goto next;
	}
	t = tz;
	if(ty2 > t) {goto next;}
	if(phys_intersect_y(world, entity, ty)) {
	  t = ty2;
	  entity->vel.y = 0.0;
	  goto next;
	}
	t = ty;
	if(tx2 > t) {goto next;}
	if(phys_intersect_x(world, entity, tx)) {
	  t = tx2;
	  entity->vel.x = 0.0;
	  goto next;
	}
	t = tx;
      }
    }

#ifdef DEBUG
    fprintf(stderr, "[PHYS ] entity finished, time step is %.4f\n", t);
#endif
  next:
    entity = world->entities[++i];
  }

  // second round, this time we will be applying the pos changes
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] starting second iteration to apply changes\n");
#endif
  i = 0;
  entity = world->entities[i];
  while(entity != NULL) {
    Vector3d tmp;

    vec_scale3d(&tmp, &entity->vel, t);
    vec_add3d(&entity->pos, &entity->pos, &tmp);

    vec_scale3d(&tmp, &entity->avel, t);
    vec_add3d(&entity->apos, &entity->apos, &tmp);

#ifdef DEBUG
    fprintf(stderr, "[PHYS ] entity %p now has pos <%.2f, %.2f, %.2f> and vel <%.2f, %.2f, %.2f>\n",
	    entity, entity->pos.x, entity->pos.y, entity->pos.z,
	    entity->vel.x, entity->vel.y, entity->vel.z);
#endif
    entity = world->entities[++i];
  }
#ifdef DEBUG
  fprintf(stderr, "[PHYS ] processed %d entities, cycle time %d usecs\n", i, (int) (t * 1000000));
#endif
  
  return t;
}

bool phys_trace(World *world, Entity *player, double dmax, Vector3d *result)
{
  double rx = RADS(-player->apos.x);
  double ry = RADS(player->apos.y);
  double vx = sin(ry) * cos(rx);
  double vy = sin(rx);
  double vz = -cos(ry) * cos(rx);
  
  Vector3d head, vel;
  vec_copy3d(&head, &player->pos);
  head.y += 0.5;
  vec_set3d(&vel, vx, vy, vz);
  double dd = 0.0, d = 0.0, dx, dy, dz;
  while(dd < dmax) {
    dx = phys_time_target(player->pos.x+(dd*vx), vx);
    dy = phys_time_target(player->pos.y+0.5+(dd*vy), vy);
    dz = phys_time_target(player->pos.z+(dd*vz), vz);
    d = dmax - dd;
    
    if(dx < dy) {
      if(dy < dz) { // dx, dy, dz
	if(dx > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
	d = dx;
	if(dy > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
	d = dy;
	if(dz > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
      } else if(dx < dz) { // dx, dz, dy
	if(dx > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
	d = dx;
	if(dz > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
	d = dz;
	if(dy > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
      } else { // dz, dx, dy
	if(dz > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
	d = dz;
	if(dx > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
        d = dx;
	if(dy > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
      }
    } else {
      if(dx < dz) { // dy, dx, dz
	if(dy > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
	d = dy;
	if(dx > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
	d = dx;
	if(dz > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
      } else if(dy < dz) { // dy, dz, dx
	if(dy > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
	d = dy;
	if(dz > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
	d = dz;
	if(dx > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
      } else { // dz, dy, dx
	if(dz > dmax) {return false;}
	if(phys_intersect(world, &head, &vel, dd+dz, result)) {return true;}
	d = dz;
	if(dy > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dy, result)) {return true;}
	d = dy;
	if(dx > d) {goto next;}
	if(phys_intersect(world, &head, &vel, dd+dx, result)) {return true;}
      }
    }

  next:
    dd += d;
  }
  return false;
}

bool phys_is_grounded(World *world, Entity *entity)
{
  if(entity->vel.y != 0.0) {return false;}
  
  Vector3d pos, tmp;
  vec_copy3d(&pos, &entity->pos);
  vec_add3d(&pos, &pos, &entity->bb_off);
  pos.y -= entity->bb_dim.y + 0.5;

  double xmax = floor(entity->bb_dim.x);
  double zmax = floor(entity->bb_dim.z);
  for(double x=-entity->bb_dim.x;floor(x)<=xmax;++x) {
    for(double z=-entity->bb_dim.z;floor(z)<=zmax;++z) {
      x = MIN(x, entity->bb_dim.x);
      z = MIN(z, entity->bb_dim.z);
      vec_copy3d(&tmp, &pos);
      tmp.x += x;
      tmp.z += z;
      Block *block = world_block_get3d(world, &tmp);
      if(block != NULL) {return true;}
    }
  }
  return false;
}

bool phys_aabb_intersect(Vector3d *pos1, Vector3d *dim1, Vector3d *pos2, Vector3d *dim2)
{
  Vector3d tmp, max1, min1, max2, min2;
  vec_add3d(&max1, pos1, dim1);
  vec_scale3d(&tmp, dim1, -1.0);
  vec_add3d(&min1, pos1, &tmp);
  vec_add3d(&max2, pos2, dim2);
  vec_scale3d(&tmp, dim2, -1.0);
  vec_add3d(&min2, pos2, &tmp);

  return max1.x > min2.x && max1.y > min2.y && max1.z > min2.z &&
    min1.x < max2.x && min1.y < max2.y && min1.z < max2.z;
}
