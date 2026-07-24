#include "projectile.hpp"

sf::Texture Projectile::texture;
bool Projectile::textureLoaded = false;

Projectile::Projectile(sf::Vector2f startPos, sf::Vector2f direction)
    : shape(8.f) {
  if (!textureLoaded) {
    texture.loadFromFile("tear.png");
    textureLoaded = true;
  }
  shape.setTexture(&texture);
  shape.setOrigin({8.f, 8.f});
  shape.setPosition(startPos);
  velocity = direction * speed;
}

void Projectile::update(float dt) {
  elapsed += dt;

  if (state == State::Flying) {
    shape.move(velocity * dt);
    if (elapsed >= flightDuration) {
      state = State::Falling;
      elapsed = 0.f;
    }
  } else if (state == State::Falling) {
    float t = elapsed / fallDuration;
    float scale = std::max(0.f, 1.f - t);
    shape.setScale({scale, scale});
    if (elapsed >= fallDuration) {
      state = State::Dead;
    }
  }
}

void Projectile::draw(sf::RenderWindow &window) {
  if (state != State::Dead) {
    window.draw(shape);
  }
}

bool Projectile::isDead() const { return state == State::Dead; };

sf::FloatRect Projectile::getBounds() const { return shape.getGlobalBounds(); }

void Projectile::kill() { state = State::Dead; };