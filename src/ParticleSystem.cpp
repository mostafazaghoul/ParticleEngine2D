#include "ParticleSystem.hpp"
#include <cmath>
#include <unordered_map>

static constexpr float       GRAVITY       = 600.f;
static constexpr float       DAMPING       = 0.72f;
static constexpr float       RESTITUTION   = 0.85f;
static constexpr std::size_t MAX_PARTICLES = 50'000;
static constexpr float       PI            = 3.14159265f;
static constexpr int         SEGMENTS      = 20;

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

void ParticleSystem::spawn(sf::Vector2f position, sf::Vector2f velocity, float radius)
{
    if (mParticles.size() >= MAX_PARTICLES)
        return;
    std::uniform_real_distribution<float> hueDist(0.f, 360.f);
    mParticles.push_back({position, velocity, hsvToRgb(hueDist(mRng), 1.f, 1.f), radius});
}

void ParticleSystem::pushFrom(sf::Vector2f center, float heldRadius)
{
    for (auto& p : mParticles) {
        float        minDist = heldRadius + p.radius;
        sf::Vector2f d       = p.position - center;
        float        distSq  = d.x * d.x + d.y * d.y;
        if (distSq > 0.f && distSq < minDist * minDist) {
            float        dist = std::sqrt(distSq);
            sf::Vector2f n    = d / dist;
            p.position       += n * (minDist - dist);
            float along = p.velocity.x * n.x + p.velocity.y * n.y;
            if (along < 0.f)
                p.velocity -= n * along * 2.f;
        }
    }
}

void ParticleSystem::jolt()
{
    std::uniform_real_distribution<float> horizDist(-200.f, 200.f);
    for (auto& p : mParticles) {
        p.velocity.x += horizDist(mRng);
        p.velocity.y -= 800.f;
    }
}

void ParticleSystem::resolveCollisions()
{
    // Grid cell = 2 * max radius so colliding particles are always in adjacent cells.
    const float CELL = RADIUS_LARGE * 2.f;

    auto cellKey = [](int cx, int cy) -> uint64_t {
        return (static_cast<uint64_t>(static_cast<uint32_t>(cx)) << 32) |
                static_cast<uint32_t>(cy);
    };

    std::unordered_map<uint64_t, std::vector<std::size_t>> grid;
    grid.reserve(mParticles.size() * 2);

    for (std::size_t i = 0; i < mParticles.size(); ++i) {
        int cx = static_cast<int>(std::floor(mParticles[i].position.x / CELL));
        int cy = static_cast<int>(std::floor(mParticles[i].position.y / CELL));
        grid[cellKey(cx, cy)].push_back(i);
    }

    for (std::size_t i = 0; i < mParticles.size(); ++i) {
        int cx = static_cast<int>(std::floor(mParticles[i].position.x / CELL));
        int cy = static_cast<int>(std::floor(mParticles[i].position.y / CELL));

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                auto it = grid.find(cellKey(cx + dx, cy + dy));
                if (it == grid.end()) continue;
                for (std::size_t j : it->second) {
                    if (j <= i) continue;

                    float        minDist = mParticles[i].radius + mParticles[j].radius;
                    sf::Vector2f d       = mParticles[j].position - mParticles[i].position;
                    float        distSq  = d.x * d.x + d.y * d.y;
                    if (distSq >= minDist * minDist || distSq == 0.f)
                        continue;

                    float        dist    = std::sqrt(distSq);
                    sf::Vector2f n       = d / dist;
                    float        overlap = minDist - dist;

                    float m1 = mParticles[i].radius * mParticles[i].radius;
                    float m2 = mParticles[j].radius * mParticles[j].radius;
                    float im1 = 1.f / m1, im2 = 1.f / m2;
                    float totalInvMass = im1 + im2;

                    mParticles[i].position -= n * (overlap * im1 / totalInvMass);
                    mParticles[j].position += n * (overlap * im2 / totalInvMass);

                    float relVel = (mParticles[i].velocity.x - mParticles[j].velocity.x) * n.x
                                 + (mParticles[i].velocity.y - mParticles[j].velocity.y) * n.y;
                    if (relVel > 0.f) {
                        float impulse = (1.f + RESTITUTION) * relVel / totalInvMass;
                        mParticles[i].velocity -= n * (impulse * im1);
                        mParticles[j].velocity += n * (impulse * im2);
                    }
                }
            }
        }
    }
}

void ParticleSystem::update(float dt, sf::Vector2u bounds)
{
    for (auto& p : mParticles) {
        p.velocity.y += GRAVITY * dt;
        p.position   += p.velocity * dt;
    }

    resolveCollisions();

    const float wx = static_cast<float>(bounds.x);
    const float wy = static_cast<float>(bounds.y);

    for (auto& p : mParticles) {
        const float r      = p.radius;
        const float right  = wx - r;
        const float bottom = wy - r;
        if (p.position.x < r)     { p.position.x = 2.f * r      - p.position.x; p.velocity.x =  std::abs(p.velocity.x) * DAMPING; }
        if (p.position.x > right) { p.position.x = 2.f * right  - p.position.x; p.velocity.x = -std::abs(p.velocity.x) * DAMPING; }
        if (p.position.y < r)     { p.position.y = 2.f * r      - p.position.y; p.velocity.y =  std::abs(p.velocity.y) * DAMPING; }
        if (p.position.y > bottom){ p.position.y = 2.f * bottom - p.position.y; p.velocity.y = -std::abs(p.velocity.y) * DAMPING; }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const
{
    sf::VertexArray verts(sf::Triangles, mParticles.size() * SEGMENTS * 3);

    for (std::size_t i = 0; i < mParticles.size(); ++i) {
        const sf::Vector2f& pos  = mParticles[i].position;
        const sf::Color&    col  = mParticles[i].color;
        const float         r    = mParticles[i].radius;
        const std::size_t   base = i * SEGMENTS * 3;

        for (int s = 0; s < SEGMENTS; ++s) {
            float a0 = s       * 2.f * PI / SEGMENTS;
            float a1 = (s + 1) * 2.f * PI / SEGMENTS;
            verts[base + s*3 + 0].position = pos;
            verts[base + s*3 + 1].position = {pos.x + std::cos(a0) * r, pos.y + std::sin(a0) * r};
            verts[base + s*3 + 2].position = {pos.x + std::cos(a1) * r, pos.y + std::sin(a1) * r};
            verts[base + s*3 + 0].color = col;
            verts[base + s*3 + 1].color = col;
            verts[base + s*3 + 2].color = col;
        }
    }

    window.draw(verts);
}
