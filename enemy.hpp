#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class Enemy {
public:
  Enemy(sf::Vector2f startPos);
  void update(float dt, sf::Vector2f playerCenter);
  void draw(sf::RenderWindow &window);
  sf::FloatRect getBounds() const;
  bool isDead() const;
  void kill();

private:
  static sf::Texture angryFLY;
  static bool textureLoaded;
  
  sf::RectangleShape shape;
  float speed = 100.f;
  bool dead = false;
};