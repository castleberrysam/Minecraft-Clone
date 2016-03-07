#include <GL/glew.h>
#include <GL/glut.h>

int main_window;

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // do yer rendering here
  glutSwapBuffers();
}

void reshape(int width, int height)
{
  // reconfigure yer projection matrix here
}

void keyboard(unsigned char key, int x, int y)
{
  // handle yer keypresses here
}

void mouse(int button, int state, int x, int y)
{
  // handle yer mouse clicks here
}

void passive_motion(int x, int y)
{
  // handle yer mouse movements here
}

void idle(void)
{
  // do yer background computations here
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  // create window
  glutInitWindowSize(1600, 900);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
  main_window = glutCreateWindow("1337 Hax");

  // register event callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passive_motion);
  glutIdleFunc(idle);

  // configure GL
  glClearColor(0.5, 0.5, 0.5, 1.0);
  
  glutMainLoop();
  return 0;
}
