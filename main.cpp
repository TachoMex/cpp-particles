#include<cstring>
#include<cstdlib>
#include<ctime>

#include<GL/freeglut.h>
#include<GL/gl.h>

#include "lib/stacktrace.hpp"
#include "lib/board.hpp"

Board * board;

void eventoClick(int b , int e, int x, int y) {
  if (b == GLUT_LEFT_BUTTON && e == GLUT_UP) {
    board -> split();
  }
}

void eventoArrastre(int x, int y) {
}

void eventoTeclado(unsigned char k, int x, int y) {
}

void renderFunction() {
  board -> render();
}

void heartbeat(int) {
  board -> heartbeat();
  glutTimerFunc(20, heartbeat, 0);
}

void opengl_init(int argc, char** argv, int width, int height) {
  board = new Board();
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Paint");
  glClearColor(1, 1, 1, 0);
  gluOrtho2D(0.0, width, 0.0, height);
  glMatrixMode(GL_PROJECTION);

  glClear(GL_COLOR_BUFFER_BIT);
  renderFunction();
  glClear(GL_COLOR_BUFFER_BIT);
  renderFunction();
}


int main(int argc, char** argv) {
  srand((unsigned)time(NULL));
  std::cin.tie(0);
  std::cin.sync_with_stdio(0);
  Teuchos::print_stack_on_segfault();
  int width = 800, height = 800;
  opengl_init(argc, argv, width, height);
  glutMouseFunc(eventoClick);
  glutMotionFunc(eventoArrastre);
  glutDisplayFunc(renderFunction);
  glutKeyboardFunc(eventoTeclado);
  glutTimerFunc(20, heartbeat, 0);

  glutMainLoop();
  return 0;
}
