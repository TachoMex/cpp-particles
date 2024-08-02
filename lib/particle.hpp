#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <string>
#include "paint/color.h"
#include "paint/canvas.h"
#include "vector.hpp"

#define DEFAULT_SPEED 5

class Particle {
public:
Vector speed, position;
Color color;

Particle(const Vector& s, const Vector& p, const Color& c) {
        speed = s;
        position = p;
        color = c;
}

void heartbeat() {
        position += speed;
}

std::string to_string() const {
        return "Particle{speed=" + speed.to_string() +
               ", position=" + position.to_string() +
               ", color=" + color.to_string() + "}";
}

void draw(Canvas* canvas) {
        // std::cout << "Particle at " << position.to_string() << std::endl;
        canvas->selected = color * 0.5;
        int x = round(position.x);
        int y = round(position.y);
        canvas->draw(x, y);
        canvas->draw(x + 1, y);
        canvas->draw(x - 1, y);
        canvas->draw(x, y + 1);
        canvas->draw(x, y - 1);
}

void bound(const Vector& limit) {
        if (position.x < 0) {
                position.x *= -1;
                speed.x *= -1;
        }
        if (position.y < 0) {
                position.y *= -1;
                speed.y *= -1;
        }
        if (position.x > limit.x) {
                position.x = limit.x - (position.x - limit.x);
                speed.x *= -1;
        }
        if (position.y > limit.y) {
                position.y = limit.y - (position.y - limit.y);
                speed.y *= -1;
        }
}
};

class ParticleSet {
public:
std::vector<Particle*> particles;

auto begin() const {
        return particles.begin();
}

auto end() const {
        return particles.end();
}

Vector limit;

ParticleSet(int x, int y) : limit(x, y){
}

void create_particle(const Vector& s, const Vector& p, const Color& c) {
        particles.push_back(new Particle(s, p, c));
}

void heartbeat() {
        for(auto p: particles) {
                p->heartbeat();
                p->bound(limit);
        }
}

void draw(Canvas* canvas) {
        for(auto p: particles) {
                p->draw(canvas);
        }
}

void split_particles() {
        auto copy = particles;
        for (auto p : copy) {
                create_particle(Vector::random_unit() * DEFAULT_SPEED, p->position, p->color);
        }
}

void create_random_particle_at(int x, int y, const Color& c){
        create_particle(Vector::random_unit() * DEFAULT_SPEED, Vector(x, y), c);
}
};

#endif
