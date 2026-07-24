#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>

class Projectile {
public:
  Projectile(sf::Vector2f startPos, sf::Vector2f direction);

  void update(float dt);
  void draw(sf::RenderWindow &window);
  bool isDead() const;
  sf::FloatRect getBounds() const;
  void kill();

private:
  static sf::Texture texture;
  static bool textureLoaded;
  enum class State { Flying, Falling, Dead };
  State state = State::Flying;

  sf::CircleShape shape;
  sf::Vector2f velocity;
  float speed = 600.f;
  float flightDuration = .4f;
  float fallDuration = .2f;
  float elapsed = 0.f;
};