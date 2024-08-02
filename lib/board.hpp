#ifndef BOARD_HPP
#define BOARD_HPP
#include <vector>

#include "particle.hpp"
#include "grid.hpp"
#include "paint/canvas.h"

class Board {
 public:
  Canvas *img;
  Grid * grid;
  ParticleSet * particles;
  int width;
  int heigth;
  Color *palette = new Color[8];

  Board() {
    palette[0] = Color::black;
    palette[1] = Color::red;
    palette[2] = Color::blue;
    palette[3] = Color::green;
    palette[4] = Color::yellow;

    width = 800;
    heigth = 800;

    particles = new ParticleSet(width, heigth);
    particles -> create_random_particle_at(800 * RAND_DOUBLE, 800 * RAND_DOUBLE, palette[4]);
    particles -> create_random_particle_at(800 * RAND_DOUBLE, 800 * RAND_DOUBLE, palette[3]);
    particles -> create_random_particle_at(800 * RAND_DOUBLE, 800 * RAND_DOUBLE, palette[2]);
    particles -> create_random_particle_at(800 * RAND_DOUBLE, 800 * RAND_DOUBLE, palette[1]);
    grid = new Grid(100, 100, width, heigth);
    img = new Canvas(width+1, heigth+1, Color::white);
  }

  int color_index(const Color& c){
    for(int i = 0; i < 5; i++) {
      if(palette[i] == c) {
        return i;
      }
    }
    return 5;
  }

  void render() {
    img -> reset(Color::white);
    grid -> draw_grid(palette, img);
    particles -> draw(img);
    img -> render(0, 0);
  }

  void heartbeat() {
    particles -> heartbeat();
    for (auto& particle : particles -> particles) {
      if (grid -> set_color(particle -> position.x, particle -> position.y, color_index(particle -> color))) {
        auto prev = particle -> position - particle -> speed;
        if (grid -> grid_x_of(prev.x) != grid -> grid_x_of(particle -> position.x)) {
          particle -> speed.x *= -1;
        }
        if (grid -> grid_y_of(prev.y) != grid -> grid_y_of(particle -> position.y)) {
          particle -> speed.y *= -1;
        }
      }
    }

    render();
    glFlush();
    glutSwapBuffers();
  }

  void split() {
    particles -> split_particles();
  }
};

#endif
