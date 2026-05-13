#include "Application.hpp"

static constexpr unsigned int WINDOW_WIDTH  = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;
static constexpr float        FIXED_DT      = 1.f / 60.f;

Application::Application()
    : mWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
              "Particle Physics",
              sf::Style::Close | sf::Style::Titlebar)
{
    mWindow.setVerticalSyncEnabled(true);
}

void Application::run()
{
    sf::Clock clock;
    float accumulator = 0.f;

    while (mWindow.isOpen()) {
        float frameTime = clock.restart().asSeconds();

        // Guard against spiral of death after a long pause (e.g. breakpoint hit)
        if (frameTime > 0.25f)
            frameTime = 0.25f;

        accumulator += frameTime;

        processEvents();

        while (accumulator >= FIXED_DT) {
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

void Application::update(float /*dt*/)
{
    // Physics and simulation logic will go here
}

void Application::render()
{
    mWindow.clear(sf::Color(15, 15, 20)); // near-black background

    // Draw calls will go here

    mWindow.display();
}
