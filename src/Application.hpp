#pragma once

#include <SFML/Graphics.hpp>

class Application {
public:
    Application();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow mWindow;
};
