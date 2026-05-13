#include "Application.hpp"
#include <cmath>

static constexpr unsigned int WINDOW_WIDTH      = 1280;
static constexpr unsigned int WINDOW_HEIGHT     = 720;
static constexpr float        FIXED_DT          = 1.f / 60.f;
static constexpr float        MAX_RELEASE_SPEED = 1500.f;

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

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            mHeldPos = {static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y)};
            mHeldVel = {0.f, 0.f};
            mHolding = true;
        }

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left && mHolding) {
            float speed = std::hypot(mHeldVel.x, mHeldVel.y);
            if (speed > MAX_RELEASE_SPEED)
                mHeldVel *= MAX_RELEASE_SPEED / speed;
            mParticles.spawn(mHeldPos, mHeldVel);
            mHolding = false;
        }
    }
}

void Application::update(float dt)
{
    if (mHolding) {
        sf::Vector2i mp       = sf::Mouse::getPosition(mWindow);
        sf::Vector2f mousePos = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
        mHeldVel              = (mousePos - mHeldPos) / dt;
        mHeldPos              = mousePos;
        mParticles.pushFrom(mHeldPos, 2.f * ParticleSystem::HALF_SIZE);
    }

    mParticles.update(dt, mWindow.getSize());
}

void Application::render()
{
    mWindow.clear(sf::Color(15, 15, 20));
    mParticles.draw(mWindow);

    if (mHolding) {
        static constexpr int   SEG = 10;
        static constexpr float PI  = 3.14159265f;
        const float             r   = ParticleSystem::HALF_SIZE;
        const sf::Color         col(255, 230, 150);
        sf::VertexArray         verts(sf::Triangles, SEG * 3);
        for (int s = 0; s < SEG; ++s) {
            float a0 = s       * 2.f * PI / SEG;
            float a1 = (s + 1) * 2.f * PI / SEG;
            verts[s*3 + 0].position = mHeldPos;
            verts[s*3 + 1].position = {mHeldPos.x + std::cos(a0) * r, mHeldPos.y + std::sin(a0) * r};
            verts[s*3 + 2].position = {mHeldPos.x + std::cos(a1) * r, mHeldPos.y + std::sin(a1) * r};
            verts[s*3 + 0].color = col;
            verts[s*3 + 1].color = col;
            verts[s*3 + 2].color = col;
        }
        mWindow.draw(verts);
    }

    mWindow.display();
}
