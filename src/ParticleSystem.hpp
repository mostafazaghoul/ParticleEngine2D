#pragma once
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class ParticleSystem {
public:
    static constexpr float HALF_SIZE = 3.f;

    void spawn(sf::Vector2f position, sf::Vector2f velocity);
    void pushFrom(sf::Vector2f center, float radius);
    void update(float dt, sf::Vector2u bounds);
    void draw(sf::RenderWindow& window) const;
    std::size_t count() const { return mParticles.size(); }

private:
    std::vector<Particle> mParticles;
};
