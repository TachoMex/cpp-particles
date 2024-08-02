#ifndef VECTOR_HPP
#define VECTOR_HPP

class Vector {
public:
double x, y;

Vector(double x_ = 0, double y_ = 0) {
        x = x_;
        y = y_;
}

static Vector polar(double angle, double magnitude) {
        return Vector(cos(angle), sin(angle));
}

static Vector random_unit() {
        double angle = RAND_DOUBLE * 2 * M_PI;
        return polar(angle, 1);
}

Vector operator*(double k) const {
        return Vector(x * k, y * k);
}

Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y);
}

Vector operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y);
}

Vector operator+=(const Vector& other) {
        x += other.x;
        y += other.y;
        return *this;
}

Vector operator*=(double k) {
        x *= k;
        y *= k;
        return *this;
}

Vector abs() const {
        return Vector(std::abs(x), std::abs(y));
}

Vector bound(const Vector& limit) const {
        double x_ = x > limit.x ? x - limit.x : x;
        double y_ = y > limit.y ? y - limit.y : y;
        return Vector(x_, y_);
}

double angle() const {
        return atan2(y, x);
}

double magnitude2() const {
        return x * x + y * y;
}

double magnitude() const {
        return sqrt(magnitude2());
}

std::string to_string() const {
        return std::string("(") + std::to_string(x) + ", " + std::to_string(y) + ")";
}
};

#endif
