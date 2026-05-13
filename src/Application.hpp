#pragma once

#include <SFML/Graphics.hpp>
#include <random>
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
    std::mt19937     mRng;
};
