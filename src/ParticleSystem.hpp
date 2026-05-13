#pragma once
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class ParticleSystem {
public:
    void spawn(sf::Vector2f position, sf::Vector2f velocity);
    void update(float dt, sf::Vector2u bounds);
    void draw(sf::RenderWindow& window) const;
    std::size_t count() const { return mParticles.size(); }

private:
    std::vector<Particle> mParticles;
};
