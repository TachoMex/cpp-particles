#include <cstring>
#include <cstdlib>
#include <ctime>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "lib/stacktrace.hpp"
#include "lib/board.hpp"

using namespace std;

class Galaxy
{
public:
  Particle sun;
  vector<Particle *> asteroids;
  Canvas *img;

  Galaxy() : sun(0, Vector(400, 400), Color::yellow)
  {
    img = new Canvas(800 + 1, 800 + 1, Color::black);
    for (int i = 0; i < 100; i++)
    {
      asteroids.push_back(new Particle(Vector::random_unit() * DEFAULT_SPEED * RAND_DOUBLE, Vector(800 * RAND_DOUBLE, 800 * RAND_DOUBLE), hsl(RAND_DOUBLE * 360)));
      cout << "Generated new particle" << asteroids.back()->to_string() << endl;
    }
  }

  void draw()
  {
    img->fade(0.99);
    for (auto p : asteroids)
    {
      p->draw(img);
    }
    sun.draw(img);
    img->render(0, 0);
  }

  void delete_far_asteroids()
  {
    vector<Particle *> close;
    for (auto p : asteroids)
    {
      double distance = (p->position - sun.position).magnitude();
      cout << "Particle at distance " << distance << endl;
      if (distance > 300)
      {
        delete p;
      }
      else
      {
        close.push_back(p);
      }
    }
    asteroids = close;
  }

  void heartbeat()
  {
    delete_far_asteroids();
    for (auto p : asteroids)
    {
      p->heartbeat();
      double angle = (p->position - sun.position).angle() + M_PI;
      p->speed += Vector::polar(angle, 1);
    }
    draw();
  }
};

Galaxy *g;

void eventoClick(int b, int e, int x, int y)
{
}

void eventoArrastre(int x, int y)
{
}

void eventoTeclado(unsigned char k, int x, int y)
{
}

void renderFunction()
{
  g->draw();
  glFlush();
  glutSwapBuffers();
}

void heartbeat(int)
{
  g->heartbeat();
  renderFunction();
  glutTimerFunc(20, heartbeat, 0);
}

void opengl_init(int argc, char **argv, int width, int height)
{
  g = new Galaxy();
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

int main(int argc, char **argv)
{
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
