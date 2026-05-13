#include "Application.hpp"
#include <cmath>

static constexpr unsigned int WINDOW_WIDTH      = 1280;
static constexpr unsigned int WINDOW_HEIGHT     = 720;
static constexpr float        FIXED_DT          = 1.f / 60.f;
static constexpr float        MAX_RELEASE_SPEED = 1500.f;
static constexpr float        PI                = 3.14159265f;
static constexpr int          SEGMENTS          = 20;

static const float RADII[] = {
    ParticleSystem::RADIUS_LARGE,
    ParticleSystem::RADIUS_MEDIUM,
    ParticleSystem::RADIUS_SMALL
};

Application::Application()
    : mWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
              "Particle Physics",
              sf::Style::Close | sf::Style::Titlebar)
    , mRng(std::random_device{}())
{
    mWindow.setVerticalSyncEnabled(true);

    mShakeBtn.setSize({50.f, 50.f});
    mShakeBtn.setPosition(10.f, 10.f);
    mShakeBtn.setOutlineThickness(1.f);
    mShakeBtn.setOutlineColor(sf::Color(100, 100, 120));
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
            sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                            static_cast<float>(event.mouseButton.y));
            if (mShakeBtn.getGlobalBounds().contains(mp)) {
                mParticles.jolt();
            } else {
                std::uniform_int_distribution<int> pick(0, 2);
                mHeldRadius = RADII[pick(mRng)];
                mHeldPos    = mp;
                mHeldVel    = {0.f, 0.f};
                mHolding    = true;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left && mHolding) {
            float speed = std::hypot(mHeldVel.x, mHeldVel.y);
            if (speed > MAX_RELEASE_SPEED)
                mHeldVel *= MAX_RELEASE_SPEED / speed;
            mParticles.spawn(mHeldPos, mHeldVel, mHeldRadius);
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
        mParticles.pushFrom(mHeldPos, mHeldRadius);
    }

    mParticles.update(dt, mWindow.getSize());
}

void Application::render()
{
    mWindow.clear(sf::Color(15, 15, 20));
    mParticles.draw(mWindow);

    if (mHolding) {
        const float     r   = mHeldRadius;
        const sf::Color col(255, 230, 150);
        sf::VertexArray verts(sf::Triangles, SEGMENTS * 3);
        for (int s = 0; s < SEGMENTS; ++s) {
            float a0 = s       * 2.f * PI / SEGMENTS;
            float a1 = (s + 1) * 2.f * PI / SEGMENTS;
            verts[s*3 + 0].position = mHeldPos;
            verts[s*3 + 1].position = {mHeldPos.x + std::cos(a0) * r, mHeldPos.y + std::sin(a0) * r};
            verts[s*3 + 2].position = {mHeldPos.x + std::cos(a1) * r, mHeldPos.y + std::sin(a1) * r};
            verts[s*3 + 0].color = col;
            verts[s*3 + 1].color = col;
            verts[s*3 + 2].color = col;
        }
        mWindow.draw(verts);
    }

    // Button — update fill on hover
    sf::Vector2f mouse(sf::Mouse::getPosition(mWindow));
    bool hovered = mShakeBtn.getGlobalBounds().contains(mouse);
    mShakeBtn.setFillColor(hovered ? sf::Color(70, 70, 90, 230) : sf::Color(40, 40, 55, 200));
    mWindow.draw(mShakeBtn);

    // Upward arrow inside button
    const sf::FloatRect b = mShakeBtn.getGlobalBounds();
    sf::ConvexShape arrow(3);
    arrow.setPoint(0, {b.left + b.width * 0.50f, b.top + b.height * 0.15f});
    arrow.setPoint(1, {b.left + b.width * 0.85f, b.top + b.height * 0.75f});
    arrow.setPoint(2, {b.left + b.width * 0.15f, b.top + b.height * 0.75f});
    arrow.setFillColor(sf::Color(210, 220, 255));
    mWindow.draw(arrow);

    mWindow.display();
}
