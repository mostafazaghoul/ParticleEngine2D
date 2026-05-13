#include "ParticleSystem.hpp"
#include <cmath>

static constexpr float       GRAVITY       = 600.f;
static constexpr float       DAMPING       = 0.72f;
static constexpr float       HALF_SIZE     = 3.f;
static constexpr std::size_t MAX_PARTICLES = 50'000;

void ParticleSystem::spawn(sf::Vector2f position, sf::Vector2f velocity)
{
    if (mParticles.size() < MAX_PARTICLES)
        mParticles.push_back({position, velocity});
}

void ParticleSystem::update(float dt, sf::Vector2u bounds)
{
    const float right  = static_cast<float>(bounds.x) - HALF_SIZE;
    const float bottom = static_cast<float>(bounds.y) - HALF_SIZE;

    for (auto& p : mParticles) {
        p.velocity.y += GRAVITY * dt;
        p.position   += p.velocity * dt;

        if (p.position.x < HALF_SIZE) { p.position.x = HALF_SIZE; p.velocity.x =  std::abs(p.velocity.x) * DAMPING; }
        if (p.position.x > right)     { p.position.x = right;     p.velocity.x = -std::abs(p.velocity.x) * DAMPING; }
        if (p.position.y < HALF_SIZE) { p.position.y = HALF_SIZE; p.velocity.y =  std::abs(p.velocity.y) * DAMPING; }
        if (p.position.y > bottom)    { p.position.y = bottom;    p.velocity.y = -std::abs(p.velocity.y) * DAMPING; }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const
{
    sf::VertexArray verts(sf::Quads, mParticles.size() * 4);

    for (std::size_t i = 0; i < mParticles.size(); ++i) {
        const sf::Vector2f& pos = mParticles[i].position;
        const std::size_t   b   = i * 4;
        const sf::Color     col(180, 210, 255);

        verts[b + 0].position = {pos.x - HALF_SIZE, pos.y - HALF_SIZE};
        verts[b + 1].position = {pos.x + HALF_SIZE, pos.y - HALF_SIZE};
        verts[b + 2].position = {pos.x + HALF_SIZE, pos.y + HALF_SIZE};
        verts[b + 3].position = {pos.x - HALF_SIZE, pos.y + HALF_SIZE};
        verts[b + 0].color = col;
        verts[b + 1].color = col;
        verts[b + 2].color = col;
        verts[b + 3].color = col;
    }

    window.draw(verts);
}
