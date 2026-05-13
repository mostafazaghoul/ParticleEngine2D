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

    sf::RenderWindow   mWindow;
    ParticleSystem     mParticles;
    std::mt19937       mRng;

    bool         mHolding    = false;
    sf::Vector2f mHeldPos;
    sf::Vector2f mHeldVel;
    float        mHeldRadius = ParticleSystem::RADIUS_LARGE;

    sf::RectangleShape mShakeBtn;
};
