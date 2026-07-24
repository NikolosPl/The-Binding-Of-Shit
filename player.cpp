#include "player.hpp"

Player::Player() : sprite(textureFRONT) {
  textureFRONT.loadFromFile("isaacFRONT.png");
  textureBACK.loadFromFile("isaacBACK.png");
  textureLEFT.loadFromFile("isaacLEFT.png");
  textureRIGHT.loadFromFile("isaacRIGHT.png");

  sprite.setTexture(textureFRONT, true);
  sprite.setScale({.5f, .5f});
  sprite.setPosition({200.f, 100.f});
  speed = 400.f;
}

sf::Vector2f Player::getCenter() const {
  sf::FloatRect bounds = sprite.getGlobalBounds();
  return {bounds.position.x + bounds.size.x / 2.f,
          bounds.position.y + bounds.size.y / 2.f};
}

void Player::handleInput(float deltaTime) {
  sf::Vector2f direction{0.f, 0.f};

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) {
    direction.y -= 1.f;
    sprite.setTexture(textureBACK);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) {
    direction.y += 1.f;
    sprite.setTexture(textureFRONT);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
    direction.x -= 1.f;
    sprite.setTexture(textureLEFT);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
    direction.x += 1.f;
    sprite.setTexture(textureRIGHT);
  }

  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (length != 0.f) {
    direction /= length;
    sprite.move(direction * speed * deltaTime);
  }
}

void Player::constraintToWindow(sf::Vector2u windowSize) {
  sf::FloatRect bounds = sprite.getGlobalBounds();
  sf::Vector2f pos = sprite.getPosition();
  sf::Vector2f winSize = static_cast<sf::Vector2f>(windowSize);

  pos.x = std::clamp(pos.x, 0.f, winSize.x - bounds.size.x);
  pos.y = std::clamp(pos.y, 0.f, winSize.y - bounds.size.y);
  sprite.setPosition(pos);
}

void Player::draw(sf::RenderWindow &window) { window.draw(sprite); }

void Player::reset() {
  sprite.setPosition({200.f, 100.f});
  hp = maxHp;
  invulnerabilityTimer = 0.f;
}

void Player::takeDamage() {
  if (invulnerabilityTimer <= 0.f) {
    hp--;
    invulnerabilityTimer = invulnerabilityDuration;
  }
}

bool Player::isDead() const { return hp <= 0; }
sf::FloatRect Player::getBounds() const { return sprite.getGlobalBounds(); }
int Player::getHp() const { return hp; }

void Player::updateTimers(float dt) {
  if (invulnerabilityTimer > 0.f) {
    invulnerabilityTimer -= dt;
  }
}