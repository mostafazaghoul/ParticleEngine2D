#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color    color;
    float        radius;
};
