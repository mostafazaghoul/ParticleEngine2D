#pragma once

#include <SFML/Graphics.hpp>
#include "ParticleSystem.hpp"

class Application {
public:
    Application();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow mWindow;
    ParticleSystem   mParticles;

    bool         mHolding = false;
    sf::Vector2f mHeldPos;
    sf::Vector2f mHeldVel;
};
