#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#ifdef __MINGW32__
#include <sys/time.h>
// clock_gettime is not implemented
int clock_gettime(int clk_id, struct timespec *t) {
  struct timeval now;
  int rv = gettimeofday(&now, NULL);
  if(rv != 0) {return rv;}
  t->tv_sec  = now.tv_sec;
  t->tv_nsec = now.tv_usec * 1000;
  return 0;
}
#define CLOCK_REALTIME 0
#else
#include <time.h>
#endif
#include "game.h"
#include "physics.h"
#include "texture.h"
#include "font.h"
#include "module.h"
#include "matrix.h"
#include "shader.h"
#include "sound.h"

static const double MOUSE_SENS = 0.1;
static double lastx = 0.0;
static double lasty = 0.0;

static const double MOVE_VEL = 2.0;
static const double FRICTION = 0.2;

static Font *font;

#define FPS_LIM 60
static int fps = FPS_LIM;

static bool selecting = false;
static Vector3d trace;
Side side;

static int num_mappings = 0;
static int current_block = 0;

static GLuint program_points;

static Sound sound_break;

#ifdef DEBUG_GRAPHICS
static void glerror(GLenum source, GLenum type, GLuint id, GLenum severity,
		    GLsizei length, const GLchar *message, void *userParam)
{
  if(severity == GL_DEBUG_SEVERITY_NOTIFICATION) {return;}
  fprintf(stderr, "[INIT ] begin OpenGL error report\n[INIT ] source: %d\n[INIT ] type: %d\n[INIT ] id: %d\n[INIT ] severity: %d\n[INIT ] %s\n[INIT ] end report\n", source, type, id, severity, message);
  asm("int $3");
}
#endif

static void error(int error, const char *description)
{
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[INIT ] glfw error: %s\n", description);
#endif
}

static void display(GLFWwindow *window)
{
  Game *game = glfwGetWindowUserPointer(window);
  World *world = game->worlds[0];
  Entity *player = world->entities[0];

  glClearColor(0x87/255.0, 0xce/255.0, 0xfa/255.0, 1.0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  matrix_identity(mview_matrix);
  matrix_rotate_x(mview_matrix, RADS(player->apos.x));
  matrix_rotate_y(mview_matrix, RADS(player->apos.y));
  
  matrix_translate(mview_matrix, -player->pos.x, -player->pos.y-0.5, -player->pos.z);
  world_draw(world);
  
  mstack_pick(mstack, proj_matrix, 0);
  
  matrix_identity(mview_matrix);
  matrix_translate(mview_matrix, 10.0, 10.0, 0.0);
  char tmp[64];
  snprintf(tmp, 64, "FPS: %d", fps);
  font_text_draw(font, tmp);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  matrix_identity(mview_matrix);
  char *name = world->mappings[current_block]->name;
  matrix_translate(mview_matrix, (width-font_text_width(font, name))/2.0, 50.0, 0.0);
  font_text_draw(font, name);

  char coords[64];
  snprintf(coords, 64, "(%.2f, %.2f, %.2f)", player->pos.x, player->pos.y, player->pos.z);
  matrix_identity(mview_matrix);
  matrix_translate(mview_matrix, width-10.0-font_text_width(font, coords), 10.0, 0.0);
  font_text_draw(font, coords);

  matrix_identity(mview_matrix);
  matrix_translate(mview_matrix, width/2.0, height/2.0, 0.0);
  glVertexAttrib3f(0, 0.0, 0.0, 0.0);
  glDisableVertexAttribArray(0);
  glUseProgram(program_points);
  glPointSize(10.0);
  glDrawArrays(GL_POINTS, 0, 1);

  mstack_pick(mstack, proj_matrix, 1);

  glfwSwapBuffers(window);
}

static void reshape(GLFWwindow *window, int width, int height)
{
  matrix_ortho(proj_matrix, width, height, -0.1, 0.1);
  mstack_replace(mstack, proj_matrix, 0);
  matrix_perspective(proj_matrix, RADS(85.0), ((double) width)/height, 0.001, 100.0);
  mstack_replace(mstack, proj_matrix, 1);
  glViewport(0, 0, width, height);
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[RESHP] width %d height %d\n", width, height);
#endif
}

static void mouse(GLFWwindow *window, double x, double y)
{
  Game *game = glfwGetWindowUserPointer(window);
  Vector3d *rot = &game->worlds[0]->entities[0]->apos;
  rot->x += (y - lasty) * MOUSE_SENS;
  rot->y += (x - lastx) * MOUSE_SENS;
  rot->x = rot->x > 90.0 ? 90.0 : (rot->x < -90.0 ? -90.0 : rot->x);
  lastx = x;
  lasty = y;
#ifdef DEBUG_INPUT
  fprintf(stderr, "[MOUSE] x %.2f, y %.2f, xrot %.2f, yrot %.2f\n", x, y, rot->x, rot->y);
#endif
}

static void click(GLFWwindow *window, int button, int action, int modifiers)
{
  Game *game = glfwGetWindowUserPointer(window);
  if(!selecting) {return;}
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    world_block_set3d(game->worlds[0], &trace, NULL);
    sound_play_static(&sound_break, &trace);
  } else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    Block *block = world_block_get3d(game->worlds[0], &trace);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
       block->activate(game->worlds[0], (Player *) game->worlds[0]->entities[0], &trace)) {return;}
    
    Vector3d tmp;
    switch(side) {
    case NORTH:
      vec_set3d(&tmp, floor(trace.x)+0.5, floor(trace.y)+0.5, floor(trace.z)-0.5);
      break;
    case EAST:
      vec_set3d(&tmp, floor(trace.x)+1.5, floor(trace.y)+0.5, floor(trace.z)+0.5);
      break;
    case SOUTH:
      vec_set3d(&tmp, floor(trace.x)+0.5, floor(trace.y)+0.5, floor(trace.z)+1.5);
      break;
    case WEST:
      vec_set3d(&tmp, floor(trace.x)-0.5, floor(trace.y)+0.5, floor(trace.z)+0.5);
      break;
    case TOP:
      vec_set3d(&tmp, floor(trace.x)+0.5, floor(trace.y)+1.5, floor(trace.z)+0.5);
      break;
    case BOTTOM:
      vec_set3d(&tmp, floor(trace.x)+0.5, floor(trace.y)-0.5, floor(trace.z)+0.5);
      break;
    }
    Entity *player = game->worlds[0]->entities[0];
    Vector3d pos1, dim2;
    vec_add3d(&pos1, &player->pos, &player->bb_off);
    vec_set3d(&dim2, 0.5, 0.5, 0.5);
    if(!phys_aabb_intersect(&pos1, &player->bb_dim, &tmp, &dim2)) {
      world_block_set3d(game->worlds[0], &tmp, game->worlds[0]->mappings[current_block]);
    }
  }
}

static void scroll(GLFWwindow *window, double x, double y)
{
  current_block += (int) y;
  current_block %= num_mappings;
  while(current_block < 0) {current_block += num_mappings;}
}

static void keyboard(GLFWwindow *window, int key, int scancode, int action, int modifiers)
{
  Game *game = glfwGetWindowUserPointer(window);
  Vector3d *vel = &game->worlds[0]->entities[0]->vel;
  switch(key) {
  case GLFW_KEY_SPACE:
    if(action == GLFW_PRESS && phys_is_grounded(game->worlds[0], game->worlds[0]->entities[0])) {
      vel->y = ((modifiers & GLFW_MOD_CONTROL) != 0) ? sqrt(2*9.81*100.0) : sqrt(2*9.81*1.4);
    }
    break;
  }
#ifdef DEBUG_INPUT
  fprintf(stderr, "[KEYBD] key %d, scan %d, action %d, mod %d\n", key, scancode, action, modifiers);
#endif
}

static long trender_max = 0, tphysics_max = 0, ttotal_max = 0;

int main(void)
{
  glfwSetErrorCallback(error);
  
  if(!glfwInit()) {
#ifdef DEBUG
    fprintf(stderr, "[INIT ] glfw failed to initialize\n");
#endif
    return 1;
  }

#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
  GLFWwindow *window = glfwCreateWindow(800, 800, "test", NULL, NULL);
  if(!window) {
#ifdef DEBUG
    fprintf(stderr, "[INIT ] glfw window creation failed\n");
#endif
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  if(glewInit() != GLEW_OK) {
#ifdef DEBUG
    fprintf(stderr, "[INIT ] glew failed to initialize\n");
#endif
    return 1;
  }
  if(!GLEW_VERSION_4_0) {
#ifdef DEBUG
    fprintf(stderr, "[INIT ] context does not support OpenGL 4.0\n");
#endif
    return 1;
  }

#ifdef DEBUG_GRAPHICS
  if(GLEW_VERSION_4_3) {
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback((GLDEBUGPROC) glerror, NULL);
  }
#endif

  glfwSetFramebufferSizeCallback(window, reshape);
  glfwSetWindowRefreshCallback(window, display);
  glfwSetCursorPosCallback(window, mouse);
  glfwSetMouseButtonCallback(window, click);
  glfwSetScrollCallback(window, scroll);
  glfwSetKeyCallback(window, keyboard);

  matrix_static_init();
  mstack_push(mstack, proj_matrix);
  mstack_push(mstack, proj_matrix);
  
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  reshape(window, width, height);
  
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  font = malloc(sizeof(Font));
#ifdef __MINGW32__
  font_init(font, "C:\\Windows\\Fonts\\arial.ttf", 12);
#elif defined __APPLE__
  font_init(font, "/Library/Fonts/Arial.ttf", 12);
#else
  font_init(font, "/usr/share/fonts/TTF/arial.ttf", 12);
#endif

  GLuint shaders[2];
  shaders[0] = load_shader("res/shader/point.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader("res/shader/point.frag", GL_FRAGMENT_SHADER);
  program_points = compile_program(2, shaders);

  Game *game = malloc(sizeof(Game));
  game_init(game);
  game_alloc_worlds(game, 1);
  World *world = game->worlds[0];
  world_alloc_players(world, 1);
  Entity *player = world->entities[0];
  glfwSetWindowUserPointer(window, game);
  
  Module terrain;
#ifdef __MINGW32__
  module_init(&terrain, "terrain.dll");
#else
  module_init(&terrain, "libterrain.so");
#endif
  module_load(&terrain);
  printf("[INIT ] loaded module %s version %s by %s\n",
    terrain.get_name(), terrain.get_ver(), terrain.get_author());
  Block **blocks = terrain.get_blocks();
  Block *block = blocks[num_mappings];
  while(block != NULL) {
    world_add_mapping(world, block);
    
    block = blocks[++num_mappings];
  }
  
  for(double x=-5.5;x<7.0;++x) {
    for(double y=-5.5;y<7.0;++y) {
      for(double z=-5.5;z<7.0;++z) {
	Vector3d tmp;
	vec_set3d(&tmp, x+floor(player->pos.x/16), y+floor(player->pos.y/16), z+floor(player->pos.z/16));
	world_chunk_gen3d(world, &tmp, num_mappings);
      }
    }
  }

  sound_init(&sound_break, "res/break.ogg", 1.0, 0.5);

  uint64_t frame = 0;
  while(!glfwWindowShouldClose(window)) {
    struct timespec start, finish, begin, end;
    clock_gettime(CLOCK_REALTIME, &start);

    clock_gettime(CLOCK_REALTIME, &begin);
    display(window);
    clock_gettime(CLOCK_REALTIME, &end);
    end.tv_nsec += (end.tv_sec - begin.tv_sec) * 1000000000;
    long trender = (end.tv_nsec - begin.tv_nsec) / 1000;
    
    glfwPollEvents();

    selecting = phys_trace(world, player, 5.0, &trace);
    side = block_side_get(&trace);

    if(!phys_is_grounded(world, player)) {
      player->vel.y -= 9.81/fps;
    }
    if(player->vel.x >= FRICTION) {
      player->vel.x -= FRICTION;
    } else if(player->vel.x > 0.0) {
      player->vel.x = 0.0;
    } else if(player->vel.x <= -FRICTION) {
      player->vel.x += FRICTION;
    } else {
      player->vel.x = 0.0;
    }
    if(player->vel.z >= FRICTION) {
      player->vel.z -= FRICTION;
    } else if(player->vel.z > 0.0) {
      player->vel.z = 0.0;
    } else if(player->vel.z <= -FRICTION) {
      player->vel.z += FRICTION;
    } else {
      player->vel.z = 0.0;
    }
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      player->vel.x = sin(RADS(player->apos.y)) * MOVE_VEL;
      player->vel.z = -cos(RADS(player->apos.y)) * MOVE_VEL;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      player->vel.x = sin(RADS(player->apos.y-90)) * MOVE_VEL;
      player->vel.z = -cos(RADS(player->apos.y-90)) * MOVE_VEL;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      player->vel.x = -sin(RADS(player->apos.y)) * MOVE_VEL;
      player->vel.z = cos(RADS(player->apos.y)) * MOVE_VEL;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      player->vel.x = sin(RADS(player->apos.y+90)) * MOVE_VEL;
      player->vel.z = -cos(RADS(player->apos.y+90)) * MOVE_VEL;
    }

    clock_gettime(CLOCK_REALTIME, &begin);
#ifdef DEBUG_PHYSICS
    int cycles = 0;
#endif
    double t = 0.0;
    while(t < (1.0/fps)) {
      t += phys_update(world, (1.0/fps) - t);
#ifdef DEBUG_PHYSICS
      ++cycles;
#endif
    }
#ifdef DEBUG_PHYSICS
    fprintf(stderr, "[INIT ] physics took %d cycles\n", cycles);
#endif
    clock_gettime(CLOCK_REALTIME, &end);
    end.tv_nsec += (end.tv_sec - begin.tv_sec) * 1000000000;
    long tphysics = (end.tv_nsec - begin.tv_nsec) / 1000;

    alListener3f(AL_POSITION, player->pos.x, player->pos.y, player->pos.z);
    alListener3f(AL_VELOCITY, player->vel.x, player->vel.y, player->vel.z);

    ALfloat listener_rot[6];
    double fx = RADS(-player->apos.x);
    double fy = RADS(player->apos.y);
    double ax = RADS(90-player->apos.x);
    double ay = fy;
    listener_rot[0] = sin(fy) * cos(fx);
    listener_rot[1] = sin(fx);
    listener_rot[2] = -cos(fy) * cos(fx);
    listener_rot[3] = sin(ay) * cos(ax);
    listener_rot[4] = sin(ax);
    listener_rot[5] = -cos(ay) * cos(ax);
    alListenerfv(AL_ORIENTATION, listener_rot);
    
#ifdef DEBUG_GRAPHICS
    GLenum error = glGetError();
    while(error != GL_NO_ERROR) {
      fprintf(stderr, "[INIT ] gl error: %s\n", gluErrorString(error));
      error = glGetError();
    }
#endif
#ifdef DEBUG
    switch(alGetError()) {
    case AL_NO_ERROR:
      break;
    case AL_INVALID_NAME:
      fprintf(stderr, "[INIT ] al error: invalid name\n");
      break;
    case AL_INVALID_ENUM:
      fprintf(stderr, "[INIT ] al error: invalid enum\n");
      break;
    case AL_INVALID_VALUE:
      fprintf(stderr, "[INIT ] al error: invalid value\n");
      break;
    case AL_INVALID_OPERATION:
      fprintf(stderr, "[INIT ] al error: invalid operation\n");
      break;
    case AL_OUT_OF_MEMORY:
      fprintf(stderr, "[INIT ] al error: out of memory\n");
      break;
    }
#endif
    
    clock_gettime(CLOCK_REALTIME, &finish);
    finish.tv_nsec += (finish.tv_sec - start.tv_sec) * 1000000000;
    long ttotal = (finish.tv_nsec - start.tv_nsec) / 1000;
    fps = MIN(1000000/ttotal, FPS_LIM);
    /*
#ifdef DEBUG
    fprintf(stderr, "[INIT ] graphics %d, physics %d, total %d (%d fps)\n",
	    (int) trender, (int) tphysics, (int) ttotal, fps);
#endif
    */

    if(frame > 120) {
      trender_max = MAX(trender_max, trender);
      tphysics_max = MAX(tphysics_max, tphysics);
      ttotal_max = MAX(ttotal_max, ttotal);
    }

    ++frame;
    usleep(ttotal > (1000000/FPS_LIM) ? 0 : (1000000/FPS_LIM) - ttotal);
  }

  if(frame > 120) {
    fprintf(stderr, "[INIT ] exiting main loop, render max %d, physics max %d, total max %d (%d fps)\n",
	    (int) trender_max, (int) tphysics_max, (int) ttotal_max, (int) (1000000/ttotal_max));
  } else {
    fprintf(stderr, "[INIT ] runtime was too short to record stats\n");
  }

  module_delete(&terrain);
  game_delete(game);
  free(game);
  sound_cleanup_lib();
  glfwTerminate();
  
  return 0;
}
