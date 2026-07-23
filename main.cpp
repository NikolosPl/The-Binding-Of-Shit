#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

class Player {
public:
  Player() : sprite(textureFRONT) {
    textureFRONT.loadFromFile("isaacFRONT.png");
    textureBACK.loadFromFile("isaacBACK.png");
    textureLEFT.loadFromFile("isaacLEFT.png");
    textureRIGHT.loadFromFile("isaacRIGHT.png");

    sprite.setTexture(textureFRONT, true);
    sprite.setScale({.5f, .5f});
    sprite.setPosition({200.f, 100.f});
    speed = 300.f;
  }
  sf::Vector2f getCenter() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return { bounds.position.x + bounds.size.x / 2.f, 
             bounds.position.y + bounds.size.y / 2.f
    };
  }
  void handleInput(float deltaTime) {
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
  void constraintToWindow(sf::Vector2u windowSize) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f winSize = static_cast<sf::Vector2f>(windowSize);

    pos.x = std::clamp(pos.x, 0.f, winSize.x - bounds.size.x);
    pos.y = std::clamp(pos.y, 0.f, winSize.y - bounds.size.y);
    sprite.setPosition(pos);
  }
  void draw(sf::RenderWindow &window) { window.draw(sprite); }
  void reset() { sprite.setPosition({200.f, 100.f}); }

private:
  sf::Texture textureFRONT;
  sf::Texture textureBACK;
  sf::Texture textureLEFT;
  sf::Texture textureRIGHT;
  sf::Sprite sprite;
  float speed;
};

class Projectile {
  public:
    Projectile(sf::Vector2f startPos, sf::Vector2f direction) : shape(8.f){
      shape.setFillColor(sf::Color::Cyan);
      shape.setOrigin({8.f,8.f});
      shape.setPosition(startPos);
      velocity = direction * speed;
    };

    void update(float dt){
      elapsed += dt;

      if(state == State::Flying){
        shape.move(velocity * dt);
        if(elapsed >= flightDuration){
          state = State::Falling;
          elapsed = 0.f;
        }
      }
      else if(state == State::Falling){
        float t = elapsed / fallDuration;
        float scale = std::max(0.f, 1.f - t);
        shape.setScale({scale,scale});
        if(elapsed >= fallDuration){
          state = State::Dead;
        }
      }
    }

    void draw(sf::RenderWindow& window){
      if(state != State::Dead){
        window.draw(shape);
      }
    }

    bool isDead() const { return state == State::Dead; };
  private:
    enum class State { Flying, Falling, Dead };
    State state = State::Flying;

    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speed = 600.f;
    float flightDuration = .4f;
    float fallDuration = .2f;
    float elapsed = 0.f;
};

int main() {
  sf::RenderWindow window(sf::VideoMode({1400, 1000}),
                          "The Binding of Isaac:SHIT");
  sf::RectangleShape bgc({1400.f, 1000.f});
  sf::Texture floor;
  if (floor.loadFromFile("basement.png")) {
    bgc.setTexture(&floor);
  }
  window.setFramerateLimit(144);
  window.setKeyRepeatEnabled(false);

  Player player;
  std::vector<Projectile> projectiles;
  sf::Vector2f lastDirection{0.f, -1.f};
  sf::Clock clock;


  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scan::R) {
          player.reset();
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
          window.close();
        }
        sf::Vector2f fireDir{0.f, 0.f};
        if (keyPressed->scancode == sf::Keyboard::Scan::Up) fireDir = {0.f, -1.f};
        if (keyPressed->scancode == sf::Keyboard::Scan::Down) fireDir = {0.f, 1.f};
        if (keyPressed->scancode == sf::Keyboard::Scan::Left) fireDir = {-1.f, 0.f};
        if (keyPressed->scancode == sf::Keyboard::Scan::Right) fireDir = {1.f, 0.f};

        if(fireDir != sf::Vector2f{0.f, 0.f}){
          projectiles.emplace_back(player.getCenter(), fireDir);
        }
      }
    }
    player.handleInput(dt);
    player.constraintToWindow(window.getSize());

    for(auto& p : projectiles){
      p.update(dt);
    }
    std::erase_if(projectiles, [](const Projectile& p) {return p.isDead(); });

    window.clear();
    window.draw(bgc);
    player.draw(window);
    for(auto& p : projectiles){
      p.draw(window);
    }
    window.display();
  }
  return 0;
}