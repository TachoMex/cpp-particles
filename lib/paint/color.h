// Copyright Tacho 2021
#ifndef LIB_COLOR_H_
#define LIB_COLOR_H_
#include <fstream>
#include <iostream>
#include <cmath>

class Color {
public:
unsigned char r;
unsigned char g;
unsigned char b;

Color() {
        r = g = b = 0;
}

Color(int x, int y, int z) {
        r = std::min(255, std::max(0, x));
        g = std::min(255, std::max(0, y));
        b = std::min(255, std::max(0, z));
}

Color operator+(const Color& o) {
        int rn = r + o.r;
        int gn = g + o.g;
        int bn = b + o.b;
        return Color(rn, gn, bn);
}

Color operator+=(const Color& o) {
        r += o.r;
        g += o.g;
        b += o.b;
        return *this;
}

Color operator-(const Color& o) const {
        int rn = r - o.r;
        int gn = g - o.g;
        int bn = b - o.b;
        return Color(rn, gn, bn);
}

std::string to_string() const {
        return std::string("(") + std::to_string(static_cast<int>(r)) + ","
               + std::to_string(static_cast<int>(g)) + "," + std::to_string(static_cast<int>(b)) + ")";
}

inline bool operator==(Color o) const {
        return r == o.r && g == o.g && b == o.b;
}

inline bool operator!=(Color o) {
        return r != o.r && g != o.g && b != o.b;
}

Color operator*(double s) const {
        int rn = s * r;
        int gn = s * g;
        int bn = s * b;
        return Color(rn, gn, bn);
}

Color to_gray() {
        int c;
        c = 0.30 * r + 0.59 * g + 0.11 * b;
        return Color(c, c, c);
}


unsigned char light() {
        return (unsigned char)(0.30 * r + 0.59 * g + 0.11 * b);
}

static const Color white;
static const Color black;
static const Color green;
static const Color blue;
static const Color red;
static const Color yellow;
};

const Color Color::white = Color(255, 255, 255);
const Color Color::black = Color(0, 0, 0);
const Color Color::green = Color(0, 255, 0);
const Color Color::blue = Color(0, 0, 255);
const Color Color::red = Color(255, 0, 0);
const Color Color::yellow = Color(255, 255, 0);

std::istream& operator>>(std::istream& s, Color& c) {
        if (&s == &std::cin) {
                int r, g, b;
                std::cin >> r;
                std::cin >> g;
                std::cin >> b;
                c.r = r;
                c.g = g;
                c.b = b;
        } else {
                c.b = s.get();
                c.g = s.get();
                c.r = s.get();
        }
        return s;
}



std::ostream& operator<<(std::ostream& s, Color c) {
        if (&s == &std::cout) {
                std::cout << c.to_string();
        } else {
                s.put(c.b);
                s.put(c.g);
                s.put(c.r);
        }
        return s;
}

Color max(Color a, Color b) {
        Color c;
        c.r = (a.r > b.r ? a.r : b.r);
        c.g = (a.g > b.g ? a.r : b.g);
        c.b = (a.r > b.b ? a.b : b.b);
        return c;
}

Color min(Color a, Color b) {
        Color c;
        c.r = (a.r < b.r ? a.r : b.r);
        c.g = (a.g < b.g ? a.r : b.g);
        c.b = (a.r < b.b ? a.b : b.b);
        return c;
}

Color absdif(Color a, Color b) {
        return max(a, b) - min(a, b);
}


Color hsl(int angle) {
        angle = (angle % 360 + 360) % 360;
        double hue = floor(255 * static_cast<double>(angle % 60) / 60.0);
        double hue_compliment = 255.0 - hue;
        switch (angle / 60 % 6) {
        case 0:
                return Color(255, hue, 0);
        case 1:
                return Color(hue_compliment, 255, 0);
        case 2:
                return Color(0, 255, hue);
        case 3:
                return Color(0, hue_compliment, 255);
        case 4:
                return Color(hue, 0, 255);
        case 5:
                return Color(255, 0, hue_compliment);
        }
        return Color::black; // this block is unreacheable
}

#ifdef GL_UNSIGNED_BYTE
void glColor(const Color & c) {
        glColor3ub(c.r, c.g, c.b);
}
#endif

#endif  // LIB_COLOR_H_
