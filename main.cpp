#include <SFML/Graphics.hpp>
#include <algorithm>

int main() {
  sf::RenderWindow window(sf::VideoMode({1400, 865}), "The Binding of Isaac:SHIT");
  window.setFramerateLimit(144);
  window.setKeyRepeatEnabled(false);
  sf::Texture basementBGC;
  basementBGC.setRepeated(false);
  sf::Texture textureFRONT;
  sf::Texture textureBACK;
  sf::Texture textureLEFT;
  sf::Texture textureRIGHT;

  if (!textureFRONT.loadFromFile("isaacFRONT.png")) {
    return -1;
  }
  if (!textureBACK.loadFromFile("isaacBACK.png")) {
    return -1;
  }
  if (!textureLEFT.loadFromFile("isaacLEFT.png")) {
    return -1;
  }
  if (!textureRIGHT.loadFromFile("isaacRIGHT.png")) {
    return -1;
  }
  if(!basementBGC.loadFromFile("basement.png")){
    return -1;
  }
  sf::Sprite sprite(textureFRONT);
  sf::Sprite floor(basementBGC);
  floor.setScale({.73f,.8f});
  sprite.setScale({.5f, .5f});
  sf::Clock clock;
  const float speed = 350.f;

  while (window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scan::R) {
          sprite.setPosition({200.f, 100.f});
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
          window.close();
        }
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
      sprite.move({speed * deltaTime, 0.f});
      sprite.setTexture(textureRIGHT);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
      sprite.move({-speed * deltaTime, 0.f});
      sprite.setTexture(textureLEFT);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) {
      sprite.move({0.f, -speed * deltaTime});
      sprite.setTexture(textureBACK);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) {
      sprite.move({0.f, speed * deltaTime});
      sprite.setTexture(textureFRONT);
    }
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());

    pos.x = std::clamp(pos.x, 0.f, windowSize.x - bounds.size.x);
    pos.y = std::clamp(pos.y, 0.f, windowSize.y - bounds.size.y);
    sprite.setPosition(pos);
    window.clear();
    window.draw(floor);
    window.draw(sprite);
    window.display();
  }

  return 0;
}