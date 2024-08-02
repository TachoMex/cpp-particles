#ifndef GRID_HPP
#define GRID_HPP

#include "paint/canvas.h"

class Grid {
 public:
  int *board;
  int grid_x;
  int grid_y;

  int width;
  int heigth;

  Grid(int x, int y, int w, int h) {
    grid_x = x;
    grid_y = y;
    width = w;
    heigth = h;
    board = new int[x * y];
    memset(board, 0, sizeof(board));
  }

  int starting_x(int x) const {
    return x * width / grid_x + 2;
  }

  int starting_y(int y) const {
    return y * heigth / grid_y  + 2;
  }

  int ending_x(int x) const {
    return starting_x(x + 1) - 2;
  }

  int ending_y(int y) const {
    return starting_y(y + 1) - 2;
  }

  int grid_x_of(double x) {
    return int(floor(x * grid_x / width));
  }

  int grid_y_of(double y) {
    return int(floor(y * grid_y / heigth));
  }

  bool set_color(double x, double y, int color) {
    // std::cout << "Filling the grid " << x << ", " << y << " := " << color << std::endl;
    int i = grid_x_of(x);
    int j = grid_y_of(y);
    if (board[j + i * grid_x] != color) {
      board[j + i * grid_x] = color;
      return true;
    }
    return false;
  }

  void draw_particle(int i, int j, const Color * const palette, Canvas * canvas) const {
    // std::cout << "Drawing (" << i << ", " <<  j << ") at "
    //           << "(" << starting_x(i) << "," << starting_y(j) << ") >> "
    //           << "(" << ending_x(i) << "," << ending_y(j) << ")"
    //           << std::endl;
    int color = board[j + i * grid_x];
    canvas->selected = palette[color];
    canvas->filled_rectangle(starting_x(i), starting_y(j), ending_x(i), ending_y(j));
  }

  void draw_grid(const Color * const palette, Canvas* canvas) const {
    for(int i = 0; i < grid_x; i++) {
      for (int j = 0; j < grid_y; j++) {
        draw_particle(i, j, palette, canvas);
      }
    }
  }
};

#endif
