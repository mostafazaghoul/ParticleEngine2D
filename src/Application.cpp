#include "Application.hpp"
#include <cmath>

static constexpr unsigned int WINDOW_WIDTH  = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;
static constexpr float        FIXED_DT      = 1.f / 60.f;
static constexpr int          SPAWN_RATE    = 4;
static constexpr float        SPAWN_SPEED   = 300.f;
static constexpr float        PI            = 3.14159265f;

Application::Application()
    : mWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
              "Particle Physics",
              sf::Style::Close | sf::Style::Titlebar)
    , mRng(std::random_device{}())
{
    mWindow.setVerticalSyncEnabled(true);
}

void Application::run()
{
    sf::Clock clock;
    float accumulator = 0.f;

    while (mWindow.isOpen()) {
        float frameTime = clock.restart().asSeconds();
        if (frameTime > 0.25f)
            frameTime = 0.25f;

        accumulator += frameTime;

        processEvents();

        while (accumulator >= FIXED_DT) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mp = sf::Mouse::getPosition(mWindow);
                sf::Vector2f pos(static_cast<float>(mp.x), static_cast<float>(mp.y));

                std::uniform_real_distribution<float> angleDist(0.f, 2.f * PI);
                std::uniform_real_distribution<float> speedDist(0.5f * SPAWN_SPEED, SPAWN_SPEED);

                for (int i = 0; i < SPAWN_RATE; ++i) {
                    float a = angleDist(mRng);
                    float s = speedDist(mRng);
                    mParticles.spawn(pos, {std::cos(a) * s, std::sin(a) * s});
                }
            }

            update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        render();
    }
}

void Application::processEvents()
{
    sf::Event event{};
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            mWindow.close();
    }
}

void Application::update(float dt)
{
    mParticles.update(dt, mWindow.getSize());
}

void Application::render()
{
    mWindow.clear(sf::Color(15, 15, 20));
    mParticles.draw(mWindow);
    mWindow.display();
}
