#pragma once
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

class ParticleSystem {
public:
    static constexpr float RADIUS_LARGE  = 32.f;
    static constexpr float RADIUS_MEDIUM = 16.f;
    static constexpr float RADIUS_SMALL  =  8.f;

    void spawn(sf::Vector2f position, sf::Vector2f velocity, float radius);
    void pushFrom(sf::Vector2f center, float heldRadius);
    void jolt();
    void update(float dt, sf::Vector2u bounds);
    void draw(sf::RenderWindow& window) const;
    std::size_t count() const { return mParticles.size(); }

private:
    void resolveCollisions();

    std::vector<Particle> mParticles;
    std::mt19937          mRng{std::random_device{}()};
};
