// Copyright Tacho 2021

#ifndef CANVAS_H_
#define CANVAS_H_

#include <cstdio>
#include <algorithm>
#include <cmath>
#include <utility>
#include <stack>

#include "image.h"
#include "utils.h"

using std::swap;

class Canvas {
public:
Image canvas;
Color selected;
Color background;
int eraser_size = 10;

public:
Canvas(int w, int h, const Color& c = Color::white) : canvas(w, h, c) {
        background = Color(255, 255, 255);
        selected = Color(0, 0, 255);
}

int width() {
        return canvas.width();
}
int height() {
        return canvas.height();
}

  #ifdef GL_POINTS
void render(int x, int y) {
        canvas.draw_at(0, 0);
}
  #endif

void draw(int y, int x) {
        try {
                canvas.at(x, y) = selected;
        } catch(int e) {}
}

void reset(const Color& c) {
        for (auto& p : canvas) {
                p = c;
        }
}

void erase(int y, int x) {
        for (int i = -eraser_size; i <= eraser_size; i++)
                for (int j = -eraser_size; j <= eraser_size; j++)
                        single_erase(y, x);
}

void fade(double f) {
        canvas.fade(f);
}

void single_erase(int y, int x) {
        try {
                canvas.at(x, y) = background;
        } catch(int e) {}
}

void bresenham_same_delta(int x1, int y1,
                          int x2, int y2, void (Canvas::*action)(int, int)) {
        if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
        int step = (y1 < y2 ? 1 : -1);
        for (; x1 <= x2; x1++, y1 += step) {
                (this->*action)(x1, y1);
        }
}

void bresenham_delta_0(int i, int j,
                       int x, bool invert, void (Canvas::*action)(int, int)) {
        if (i > j)
                swap(i, j);
        for (; i <= j; i++)
                invert ? (this->*action)(x, i) : (this->*action)(i, x);
}

void line(int x1, int y1,
          int x2, int y2, void (Canvas::*action)(int, int) = &Canvas::draw) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        if (dy == 0) {
                bresenham_delta_0(x1, x2, y1, false, action);
        } else if (dx == 0) {
                bresenham_delta_0(y1, y2, x1, true, action);
        } else if (dx == dy) {
                bresenham_same_delta(x1, y1, x2, y2, action);
        } else if (dy < dx) {
                if (x1 > x2) {
                        swap(x1, x2);
                        swap(y1, y2);
                }
                int x = x1, y = y1;
                int incremento = (y1 < y2 ? 1 : -1);
                int p = 0;
                p = 2 * dy - dx;
                while (x != x2 || y != y2) {
                        (this->*action)(x, y);
                        if (p >= 0) {
                                x++;
                                y += incremento;
                                p = p + 2 * dy - 2 * dx;
                        } else {
                                x++;
                                p = p + 2 * dy;
                        }
                }
        } else {
                if (y1 > y2) {
                        swap(x1, x2);
                        swap(y1, y2);
                }
                int y = y1, x = x1;
                int incremento = (x1 < x2 ? 1 : -1);
                int p = 0;
                p = 2 * dx - dy;
                while (y != y2 || x != x2) {
                        (this->*action)(x, y);
                        if (p > 0) {
                                y++;
                                x += incremento;
                                p = p + 2 * dx - 2 * dy;
                        } else {
                                y++;
                                p = p + 2 * dx;
                        }
                }
        }
        (this->*action)(x2, y2);
}

void rectangle(int x1, int y1, int x2, int y2) {
        line(x1, y1, x1, y2);
        line(x1, y1, x2, y1);
        line(x1, y2, x2, y2);
        line(x2, y1, x2, y2);
}

void filled_rectangle(int x1, int y1, int x2, int y2) {
        if(x1 > x2) {
                swap(x1, x2);
        }
        for (; x1 <= x2; x1++) {
                line(x1, y1, x1, y2);
        }
}

void fill(int x, int y) {
        if (canvas.at(y, x) == selected)
                return;

        Color target = canvas.at(y, x);

        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        draw(x, y);
        std::stack<int> stack;
        stack.push(x);
        stack.push(y);

        while (!stack.empty()) {
                y = stack.top(); stack.pop();
                x = stack.top(); stack.pop();
                for (int i = 0; i < 4; i++) {
                        int x2 = x + dx[i];
                        int y2 = y + dy[i];
                        try {
                                if (canvas.at(y2, x2) == target) {
                                        draw(x2, y2);
                                        stack.push(x2);
                                        stack.push(y2);
                                }
                        } catch (int e) {}
                }
        }
}

void ellipse(int h, int k, double a, double b) {
        int x = 0;
        int y = b;
        int p;
        double a2 = square(a), b2 = square(b);
        p = b2 - a2 * b + 0.25 * a2;
        draw(h + x, k + y);
        draw(h - x, k + y);
        draw(h + x, k - y);
        draw(h - x, k - y);

        while (x * b2  < y * a2) {
                if (p < 0) {
                        p = p + 2 * x * b2 + b2;
                } else {
                        y--;
                        p = p + 2 * x * b2 + b2 - 2 * y * a2;
                }
                x++;
                draw(h + x, k + y);
                draw(h - x, k + y);
                draw(h + x, k - y);
                draw(h - x, k - y);
        }

        p = b2 * square(x + 0.5) + a2 * square(y - 1) - a2 * b2;

        while (y > 0) {
                if (p > 0) {
                        y--;
                        p = p - 2 * a2 * y + a2;
                } else {
                        x++;
                        y--;
                        p = p + 2 * b2 * x - 2 * a2 * y + a2;
                }
                draw(h + x, k + y);
                draw(h - x, k + y);
                draw(h + x, k - y);
                draw(h - x, k - y);
        }
}

void polygon(int x, int y, int r, double ang, int l) {
        int h, k;
        int p, q;
        h = floor(r * cos(ang));
        k = floor(r * sin(ang));
        for (int i = 1; i <= l; i++) {
                p = floor(r * cos(i * (2.0 * M_PI / l) + ang));
                q = floor(r * sin(i * (2.0 * M_PI / l) + ang));
                line(h + x, k + y, p + x, q + y);
                h = p;
                k = q;
        }
}

void circle_points(int h, int k, int x, int y) {
        draw(h + x, k + y);
        draw(h - x, k + y);
        draw(h + x, k - y);
        draw(h - x, k - y);
        draw(h + y, k + x);
        draw(h - y, k + x);
        draw(h + y, k - x);
        draw(h - y, k - x);
}

void circle(int h, int k, int r) {
        int x, y;
        x = 0;
        y = r;
        double p;
        p = 1 - r;

        circle_points(h, k, x, y);

        while (x < y) {
                x++;
                if (p < 0) {
                        p += 2 * x + 1;
                } else {
                        y--;
                        p += 2 * (x - y) + 1;
                }
                circle_points(h, k, x, y);
        }
}

void star(int x, int y,
          int r, double ang, int l, double inner_polygon = 0.5) {
        int h, k;
        int p, q;
        int s, t;
        s = round(r * cos(M_PI / l + ang) * inner_polygon);
        t = round(r * sin(M_PI / l + ang) * inner_polygon);
        for (int i = 1; i <=l; i++) {
                p = round(r * cos(i * (2.0 * M_PI / l) + ang));
                q = round(r * sin(i * (2.0 * M_PI / l) + ang));
                line(s + x, t + y, p + x, q + y);
                s = round(r * cos((2.0 * i + 1.0) * M_PI / l + ang) * inner_polygon);
                t = round(r * sin((2.0 * i + 1.0) * M_PI / l + ang) * inner_polygon);
                line(s + x, t + y, p + x, q + y);
                h = p;
                k = q;
        }
}

void heart(int x, int y, int a, int b) {
        int h, k;
        int p, q;
        int IT = 100;
        double t = 0;
        h = round(a * cube(sin(t)));
        k = round(((13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t) + 2.5) / 29 + 0.5) * b);
        for (int i = 0; i <=IT; i++) {
                p = round(a * cube(sin(t)));
                q = round(((13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t) + 2.5) / 29 + 0.5) * b);
                line(x + h, y + k, x + p, y + q);
                h = p;
                k = q;
                t += 2 * M_PI / IT;
        }
}

void spline_curve(double x1, double y1,
                  double x2, double y2,
                  double x3, double y3,
                  double x4, double y4) {
        int p, q;
        p = round(x1);
        q = round(y1);
        for (double t = 0; t <= 1; t += 1.0 / 45) {
                double t2 = t * t, t3 = t2 * t;
                int x = round(cube(1 - t) * x1 + 3 * t * square(1 - t) * x3 + 3 * t2 * (1 - t) * x4 + t3 * x2);
                int y = round(cube(1 - t) * y1 + 3 * t * square(1 - t) * y3 + 3 * t2 * (1 - t) * y4 + t3 * y2);
                line(p, q, x, y);
                p = x;
                q = y;
        }
}

void load_bmp(const char *const name) {
        Image *i = new Image();
        i->load_bmp(name);
        i->draw_at(&canvas, 0, 0);
        delete i;
}

void save_bmp(const char *const name) {
        canvas.save_bmp(name);
}
};
#endif  // CANVAS_H_
