#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

class Player {
public:
  Player();

  sf::Vector2f getCenter() const;
  void handleInput(float deltaTime);
  void constraintToWindow(sf::Vector2u windowSize);
  void draw(sf::RenderWindow &window);
  void reset();
  void takeDamage();
  bool isDead() const;
  sf::FloatRect getBounds() const;
  int getHp() const;
  void updateTimers(float dt);

private:
  int hp = 3;
  const int maxHp = 3;
  float invulnerabilityTimer = 0.f;
  const float invulnerabilityDuration = 1.f;
  sf::Texture textureFRONT;
  sf::Texture textureBACK;
  sf::Texture textureLEFT;
  sf::Texture textureRIGHT;
  sf::Sprite sprite;
  float speed;
};