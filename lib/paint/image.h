// Copyright Tacho 2021

#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstring>
#include <algorithm>
#include <iterator>

#include "color.h"

class Image {
private:
Color *pixels;
int width_, height_;

public:
typedef Color* iterator;
typedef const Color* const_iterator;
iterator begin() {
        return pixels;
}

iterator end() {
        return &pixels[width_ * height_ - 1];
}

Image(int w, int h, const Color& def = Color::black) {
        width_ = w;
        height_ = h;
        pixels = new Color[w * h];
        int j = 0;;
        for (auto &i : *this) {
                i = def;
        }
}

Image() {
        pixels = NULL;
        width_ = height_ = 0;
}

Image(const Image &i) {
        width_ = i.width_;
        height_ = i.height_;
        pixels = new Color[width_ * height_];
        memcpy(pixels, i.pixels, width_ * height_ * sizeof(Color));
}

int height() {
        return height_;
}

int rows() {
        return height_;
}

int cols() {
        return width_;
}

int width() {
        return width_;
}

Color& at(int i, int j) {
        if (i < 0 || i >= height_  || j < 0 || j >= width_)
                throw 0;
        return pixels[i * width_ + j];
}

Color at(int i, int j) const {
        if (i < 0 || i >= height_  || j < 0 || j >= width_)
                throw 0;
        return pixels[i * width_ + j];
}

Image gray_scale() {
        Image ret(width_, height_);
        auto b = ret.begin();
        for (auto &a : *this) {
                *b++ = a.to_gray();
        }
        return ret;
}

Image x_derivative() {
        Image ret(width_, height_);
        for (int i = 0; i < height_; i++) {
                ret.at(i, 0) = at(i, 0);
                for (int j = 1; j < width_; j++)
                        ret.at(i, j) = absdif(at(i, j), at(i, j - 1));
        }
        return ret;
}

Image y_derivative() {
        Image ret(width_, height_);
        for (int i = 0; i < width_; i++) {
                ret.at(0, i) = at(0, i);
                for (int j = 1; j < height_; j++)
                        ret.at(j, i) = absdif(at(j, i), at(j - 1, i));
        }
        return ret;
}

Image derivative() {
        Image X = x_derivative();
        Image Y = y_derivative();
        auto dx = X.begin();
        auto dy = Y.begin();
        Image ret(width_, height_);
        for (auto &b : ret) {
                b = max(*dx++, *dy++);
        }
        return ret;
}

Image threshold(unsigned char min = 128, Color down = Color()) {
        Image ret(width_, height_);
        auto b = ret.begin();
        for (auto a : *this) {
                *b++ = (a.light() > min ? a : down);
        }
        return ret;
}

Image threshold(unsigned char min, Color down, Color up) {
        Image ret(width_, height_);
        auto b = ret.begin();
        for (auto a : *this) {
                *b++ = (a.light() > min ? up : down);
        }
        return ret;
}

void copy(const Image &o) {
        memcpy(pixels, o.pixels, width_ * height_ * sizeof(Color));
}

void copy(const Image &o, int h, int k) {
        for (int i = 0; i < o.height_; i++)
                for (int j = 0; j < o.width_; j++) {
                        int p, q;
                        p = h + i;
                        q = k + j;
                        try {
                                at(p, q) = o.at(i, j);
                        }catch(int e) {}
                }
}

void fade(double f) {
        for (auto& a : *this) {
                a = a * f;
        }
}

    #ifdef GL_POINTS
void draw_at(int h, int k) {
        for (int i = 0; i < height_; i++)
                for (int j = 0; j < width_; j++) {
                        int p, q;
                        p = h + i;
                        q = k + j;
                        Color c = at(i, j);
                        glColor3ub(c.r, c.g, c.b);
                        glBegin(GL_POINTS);
                        glVertex2i(q, p);
                        glEnd();
                }
}
    #endif

~Image() {
        delete pixels;
}

Image operator=(Image i) {
        delete pixels;
        width_ = i.width_;
        height_ = i.height_;
        pixels = new Color[width_ * height_];
        memcpy(pixels, i.pixels, width_ * height_ * sizeof(Color));
        return i;
}

Image matrix_filter(double filter[3][3]) {
        Image ret(*this);
        for (int i = 0; i < width_; i++) {
                for (int j = 0; j < height_; j++) {
                        double sr = 0;
                        double sg = 0;
                        double sb = 0;
                        for (int k = -1; k < 2; k++) {
                                for (int l = -1; l < 2; l++) {
                                        int a = j + k, b = i + l;
                                        if (a >= 0 && b >= 0 && b < width_ && a < height_) {
                                                Color aux = at(a, b);
                                                sr += static_cast<double>(aux.r * filter[k + 1][l + 1]);
                                                sg += static_cast<double>(aux.g * filter[k + 1][l + 1]);
                                                sb += static_cast<double>(aux.b * filter[k + 1][l + 1]);
                                        }
                                }
                        }
                        ret.at(j, i) = Color(sr, sg, sb);
                }
        }
        return ret;
}

Image gaussian_filter() {
        double filter[3][3] = {
                {1.0 / 21, 1.0 / 7, 1.0 / 21},
                {1.0 / 7, 5.0 / 21, 1.0 / 7},
                {1.0 / 21, 1.0 / 7, 1.0 / 21}
        };
        return matrix_filter(filter);
}

Image sharpen_filter() {
        double filter[3][3] = {
                {  0, -1,  0},
                {-1,  5, -1},
                {  0, -1,  0 }
        };
        return matrix_filter(filter);
}

Image blur_filter() {
        double filter[3][3] = {
                {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
                {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
                {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
        };
        return matrix_filter(filter);
}

Image erode() {
        Image ret(width_, height_);
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        for (int i = 0; i < height_; i++)
                for (int j = 0; j < width_; j++) {
                        Color c = at(i, j);
                        for (int k = 0; k < 4; k++) {
                                try {
                                        Color z = at(i + dy[k], j + dx[k]);
                                        c = min(c, z);
                                }catch(int e) {}
                        }
                        ret.at(i, j) = c;
                }
        return ret;
}

Image dilate() {
        Image ret(width_, height_);
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        for (int i = 0; i < height_; i++)
                for (int j = 0; j < width_; j++) {
                        Color c = at(i, j);
                        for (int k = 0; k < 4; k++) {
                                try {
                                        Color z = at(i + dy[k], j + dx[k]);
                                        c = max(c, z);
                                }catch(int e) {}
                        }
                        ret.at(i, j) = c;
                }
        return ret;
}

Image laplace() {
        double filter[3][3] = {
                {  -1, -1,  -1},
                {-1,  8, -1},
                {  -1, -1,  -1 }
        };
        return matrix_filter(filter);
}

void save_bmp(const char *nombre) {
        std::ofstream f(nombre);
        f.put('B'); f.put('M'); // Tipo
        int tam = 54 + width_ * height_;
        f.write(reinterpret_cast<char*>(&tam), sizeof(int));
        int reservado = 0;
        f.write(reinterpret_cast<char*>(&reservado), sizeof(int));
        int offset = 0x36;
        f.write(reinterpret_cast<char*>(&offset), sizeof(int));
        int tamCabeceras = 40;
        f.write(reinterpret_cast<char*>(&tamCabeceras), sizeof(int));
        f.write(reinterpret_cast<char*>(&width_), sizeof(int)); // ancho
        f.write(reinterpret_cast<char*>(&height_), sizeof(int)); // alto
        int16_t planos = 1;
        f.write(reinterpret_cast<char*>(&planos), sizeof(int16_t));
        int16_t bits = 24;
        f.write(reinterpret_cast<char*>(&bits), sizeof(int16_t));
        int compresion = 0;
        f.write(reinterpret_cast<char*>(&compresion), sizeof(int));
        f.put(0); f.put(0); f.put(0); f.put(0); // Tamaño Paleta
        f.put(0); f.put(0); f.put(0); f.put(0); // BitsPorMetroX
        f.put(0); f.put(0); f.put(0); f.put(0); // BitsPorMetroY
        f.put(0); f.put(0); f.put(0); f.put(0); // Colores Usados
        f.put(0); f.put(0); f.put(0); f.put(0); // Colores Importantes
        int ajuste = (4 - (width_ * 3) % 4) % 4;
        // std::cout << f.tellp() << std::endl;
        for (int i = 0; i < height_; i++) {
                for (int j = 0; j < width_; j++)
                        f << at(i, j);
                for (int j = 0; j < ajuste; j++)
                        f << Color();
        }
        f.close();
}

void load_bmp(const char *nombre) {
        delete pixels;
        std::ifstream f(nombre);
        if (f.get() != 'B' || f.get() != 'M') {
                std::cout << "No es BMP" << std::endl;
        }
        int tam, head_size;
        f.read(reinterpret_cast<char*>(&tam), sizeof(int));
        f.get(); f.get(); f.get(); f.get(); // Reservado
        f.read(reinterpret_cast<char*>(&head_size), sizeof(int));
        f.get(); f.get(); f.get(); f.get(); // Offset
        f.read(reinterpret_cast<char*>(&width_), sizeof(int)); // x++;
        f.read(reinterpret_cast<char*>(&height_), sizeof(int)); // y++;
        f.get(); f.get(); // Planos
        int16_t bits;
        f.read(reinterpret_cast<char*>(&bits), sizeof(int16_t));
        f.get(); f.get(); f.get(); f.get(); // Compresion
        f.get(); f.get(); f.get(); f.get(); // Tamaño Paleta
        f.get(); f.get(); f.get(); f.get(); // BitsPorMetroX
        f.get(); f.get(); f.get(); f.get(); // BitsPorMetroY
        f.get(); f.get(); f.get(); f.get(); // Colores Usados
        f.get(); f.get(); f.get(); f.get(); // Colores Importantes
        pixels = new Color[width_ * height_];
        int ajuste = (4 - (width_ * 3) % 4) % 4;
        f.seekg(head_size, f.beg);
        for (int i = 0; i < height_; i++) {
                for (int j = 0; j < width_; j++) {
                        f >> at(i, j);
                }
                for (int j = 0; j < ajuste; j++)
                        f.get();
        }
        f.close();
}

Image region(int p, int q, int h, int k, Image& r) {
        delete r.pixels;
        r.height_ = k;
        r.pixels = new Color[h * k];

        for (int i = 0; i < k; i++)
                for (int j = 0; j <h; j++)
                        try {
                                r.at(i, j) = at(q + i, p + j);
                        }catch(int e) {}
        return r;
}


Image* region(int p, int q, int h, int k) {
        Image* r = new Image(h, k);

        for (int i = 0; i < k; i++)
                for (int j = 0; j <h; j++)
                        try {
                                r->at(i, j) = at(q + i, p + j);
                        } catch(int e) {}
        return r;
}


void draw_at(Image* o, int x, int y) {
        for (int i = 0; i < height_; i++) {
                for (int j = 0; j < width_; j++) {
                        try {
                                o->at(y + i, x + j) = at(i, j);
                        } catch (int e) {}
                }
        }
}

    #ifdef GL_H
void gl_read() {
        delete pixels;
        width_ = glutGet(GLUT_WINDOW_WIDTH);
        height_ = glutGet(GLUT_WINDOW_HEIGHT);
        pixels = new Color[width_ * height_];
        glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

void gl_read(int x2, int y2) {
        delete pixels;
        width_ = x2;
        height_ = y2;
        pixels = new Color[width_ * height_];
        glReadPixels(0, 0, x, y, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

void gl_draw() {
        glDrawPixels(x, y, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}
    #endif  // GL_H
};

#endif  // IMAGE_H_
