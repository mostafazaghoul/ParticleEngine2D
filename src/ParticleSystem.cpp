#include "ParticleSystem.hpp"
#include <cmath>

static constexpr float       GRAVITY       = 600.f;
static constexpr float       DAMPING       = 0.72f;
static constexpr std::size_t MAX_PARTICLES = 50'000;
static constexpr float       PI            = 3.14159265f;
static constexpr int         SEGMENTS      = 10;

static sf::Color hsvToRgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1.f - std::abs(std::fmod(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    float r, g, b;
    if      (h < 60.f)  { r = c; g = x; b = 0; }
    else if (h < 120.f) { r = x; g = c; b = 0; }
    else if (h < 180.f) { r = 0; g = c; b = x; }
    else if (h < 240.f) { r = 0; g = x; b = c; }
    else if (h < 300.f) { r = x; g = 0; b = c; }
    else                { r = c; g = 0; b = x; }
    return {static_cast<uint8_t>((r + m) * 255.f),
            static_cast<uint8_t>((g + m) * 255.f),
            static_cast<uint8_t>((b + m) * 255.f)};
}

void ParticleSystem::spawn(sf::Vector2f position, sf::Vector2f velocity)
{
    if (mParticles.size() >= MAX_PARTICLES)
        return;
    std::uniform_real_distribution<float> hueDist(0.f, 360.f);
    mParticles.push_back({position, velocity, hsvToRgb(hueDist(mRng), 1.f, 1.f)});
}

void ParticleSystem::pushFrom(sf::Vector2f center, float radius)
{
    const float radiusSq = radius * radius;
    for (auto& p : mParticles) {
        sf::Vector2f d      = p.position - center;
        float        distSq = d.x * d.x + d.y * d.y;
        if (distSq > 0.f && distSq < radiusSq) {
            float        dist = std::sqrt(distSq);
            sf::Vector2f n    = d / dist;
            p.position       += n * (radius - dist);
            float along = p.velocity.x * n.x + p.velocity.y * n.y;
            if (along < 0.f)
                p.velocity -= n * along * 2.f;
        }
    }
}

void ParticleSystem::update(float dt, sf::Vector2u bounds)
{
    const float r      = HALF_SIZE;
    const float right  = static_cast<float>(bounds.x) - r;
    const float bottom = static_cast<float>(bounds.y) - r;

    for (auto& p : mParticles) {
        p.velocity.y += GRAVITY * dt;
        p.position   += p.velocity * dt;

        if (p.position.x < r)     { p.position.x = 2.f * r     - p.position.x; p.velocity.x =  std::abs(p.velocity.x) * DAMPING; }
        if (p.position.x > right) { p.position.x = 2.f * right - p.position.x; p.velocity.x = -std::abs(p.velocity.x) * DAMPING; }
        if (p.position.y < r)     { p.position.y = 2.f * r     - p.position.y; p.velocity.y =  std::abs(p.velocity.y) * DAMPING; }
        if (p.position.y > bottom){ p.position.y = 2.f * bottom- p.position.y; p.velocity.y = -std::abs(p.velocity.y) * DAMPING; }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const
{
    sf::VertexArray verts(sf::Triangles, mParticles.size() * SEGMENTS * 3);

    for (std::size_t i = 0; i < mParticles.size(); ++i) {
        const sf::Vector2f& pos  = mParticles[i].position;
        const sf::Color&    col  = mParticles[i].color;
        const std::size_t   base = i * SEGMENTS * 3;

        for (int s = 0; s < SEGMENTS; ++s) {
            float a0 = s       * 2.f * PI / SEGMENTS;
            float a1 = (s + 1) * 2.f * PI / SEGMENTS;

            verts[base + s*3 + 0].position = pos;
            verts[base + s*3 + 1].position = {pos.x + std::cos(a0) * HALF_SIZE, pos.y + std::sin(a0) * HALF_SIZE};
            verts[base + s*3 + 2].position = {pos.x + std::cos(a1) * HALF_SIZE, pos.y + std::sin(a1) * HALF_SIZE};
            verts[base + s*3 + 0].color = col;
            verts[base + s*3 + 1].color = col;
            verts[base + s*3 + 2].color = col;
        }
    }

    window.draw(verts);
}
