#include "enemy.hpp"

sf::Texture Enemy::angryFLY;
bool Enemy::textureLoaded = false;

Enemy::Enemy(sf::Vector2f startPos) : shape({50.f, 50.f}) {
  if (!textureLoaded) {
    angryFLY.loadFromFile("redFLY.png");
    textureLoaded = true;
  }
  shape.setTexture(&angryFLY);
  shape.setOrigin({20.f, 20.f});
  shape.setPosition(startPos);
}

void Enemy::update(float dt, sf::Vector2f playerCenter) {
  sf::Vector2f toPlayer = playerCenter - shape.getPosition();
  float length = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
  if (length != .0f) {
    toPlayer /= length;
    shape.move(toPlayer * speed * dt);
  }
}

void Enemy::draw(sf::RenderWindow &window) { window.draw(shape); }

sf::FloatRect Enemy::getBounds() const { return shape.getGlobalBounds(); }

bool Enemy::isDead() const { return dead; }

void Enemy::kill() { dead = true; }