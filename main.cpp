#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>


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
    speed = 400.f;
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
  void reset() {
     sprite.setPosition({200.f, 100.f}); 
     hp = maxHp;
     invulnerabilityTimer = 0.f;
  }
  void takeDamage() {
    if(invulnerabilityTimer <= 0.f){
      hp--;
      invulnerabilityTimer = invulnerabilityDuration;
    }
  }
  bool isDead() const { return hp <= 0; }
  sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
  int getHp() const { return hp; }
  void updateTimers(float dt){
    if(invulnerabilityTimer > 0.f){
      invulnerabilityTimer -= dt;
    }
  }

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

class Projectile {
  public:
    Projectile(sf::Vector2f startPos, sf::Vector2f direction) : shape(8.f){
      if(!textureLoaded){
        texture.loadFromFile("tear.png");
        textureLoaded = true;
      }
      shape.setTexture(&texture);
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

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

    void kill() { state = State::Dead; };
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
  

sf::Texture Projectile::texture;
bool Projectile::textureLoaded = false;


class Enemy {
  public:
    Enemy(sf::Vector2f startPos) : shape({50.f,50.f}){
      if(!textureLoaded){
        angryFLY.loadFromFile("redFLY.png");
        textureLoaded = true;
      }
      shape.setTexture(&angryFLY);
      shape.setOrigin({20.f,20.f});
      shape.setPosition(startPos);
    }
    void update(float dt, sf::Vector2f playerCenter){
      sf::Vector2f toPlayer = playerCenter - shape.getPosition();
      float length = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
      if(length != .0f){
        toPlayer /= length;
        shape.move(toPlayer * speed * dt);
      }
    }

    void draw(sf::RenderWindow& window){
      window.draw(shape);
    }
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    bool isDead() const { return dead; }
    void kill() { dead = true; }
  private:
    static sf::Texture angryFLY;
    static bool textureLoaded;
    sf::RectangleShape shape;
    float speed = 100.f;
    bool dead = false;
};

sf::Texture Enemy::angryFLY;
bool Enemy::textureLoaded = false;

int main() {
  sf::SoundBuffer isaacDeathSound("isaacDEATH.wav");
  sf::SoundBuffer hitMarkerSound("hitmarker.wav");

  sf::Sound ids(isaacDeathSound);
  sf::Sound hms(hitMarkerSound);
  sf::Font font;
  font.openFromFile("game_over.ttf");
  sf::Text gameOverText(font, "GAME OVER - nacisnij R", 50);
  gameOverText.setFillColor(sf::Color::Red);
  gameOverText.setPosition({400.f,450.f});
  float spawnTimer = 0.f;
  float spawnInterval = 1.f;
  float difficultyTimer = 0.f;
  enum class GameState { Playing, GameOver };
  sf::RenderWindow window(sf::VideoMode({1400, 1000}),
                          "The Binding of Isaac:SHIT");
  sf::RectangleShape bgc({1400.f, 1000.f});
  sf::Texture floor;
  if (floor.loadFromFile("basement.png")) {
    bgc.setTexture(&floor);
  }
  window.setFramerateLimit(144);
  window.setKeyRepeatEnabled(false);
  GameState state = GameState::Playing;
  Player player;
  std::vector<Projectile> projectiles;
  std::vector<Enemy> enemies;
  enemies.emplace_back(sf::Vector2f{800.f, 400.f});
  sf::Vector2f lastDirection{0.f, -1.f};
  sf::Clock clock;

  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<float> distX(0.f, 1400.f);
  std::uniform_real_distribution<float> distY(0.f , 1000.f);

  int score = 0;
  sf::Text scoreText(font, "Wynik: 0", 50);
  scoreText.setFillColor(sf::Color::Black);
  scoreText.setPosition({150.f, 0.f});

  float survivalTime = 0.f;
  sf::Text survivalText(font, "Czas: 0.0s", 50);
  survivalText.setFillColor(sf::Color::Black);
  survivalText.setPosition({150.f, -20.f});


  sf::Text difficultyText(font, "TRUDNOSC ZWIEKSZONA", 40);
  difficultyText.setFillColor(sf::Color::Yellow);
  sf::FloatRect diffBounds = difficultyText.getLocalBounds();
  difficultyText.setOrigin({diffBounds.size.x / 2.f, 0.f});
  difficultyText.setPosition({700.f, 30.f});
  float difficultyMessageTimer = 0.f;

  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
          window.close();
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::R) {
            player.reset();
            projectiles.clear();
            score = 0;
            scoreText.setString("Wynik: 0");
            survivalTime = 0.f;
            survivalText.setString("Czas: 0.0s");
            enemies.clear();
            enemies.emplace_back(sf::Vector2f{800.f, 400.f});
        }

        if (state == GameState::Playing) {
          sf::Vector2f fireDir{0.f, 0.f};
          if (keyPressed->scancode == sf::Keyboard::Scan::Up)
            fireDir = {0.f, -1.f};
          if (keyPressed->scancode == sf::Keyboard::Scan::Down)
            fireDir = {0.f, 1.f};
          if (keyPressed->scancode == sf::Keyboard::Scan::Left)
            fireDir = {-1.f, 0.f};
          if (keyPressed->scancode == sf::Keyboard::Scan::Right)
            fireDir = {1.f, 0.f};

          if (fireDir != sf::Vector2f{0.f, 0.f}) {
            projectiles.emplace_back(player.getCenter(), fireDir);
          }
        }
      }
    }

    if (state == GameState::Playing) {
      player.handleInput(dt);
      player.updateTimers(dt);
      sf::Vector2f playerCenter = player.getBounds().position +
                                  sf::Vector2f{player.getBounds().size.x / 2.f,
                                               player.getBounds().size.y / 2.f};
      player.constraintToWindow(window.getSize());

      for (auto &enemy : enemies) {
        enemy.update(dt, playerCenter);
      }

      survivalTime += dt;
      survivalText.setString("Czas: " + std::to_string(survivalTime).substr(0, 4) + "s");
      spawnTimer += dt;
      difficultyTimer += dt;
      if(difficultyTimer >= 10.f){
        difficultyTimer = 0.f;
        spawnInterval = std::max(0.2f, spawnInterval - 0.1f);
        difficultyMessageTimer = 2.f;
      }
      if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.f;
        enemies.emplace_back(sf::Vector2f{distX(rng), distY(rng)});
      }

      for (auto &p : projectiles) {
        p.update(dt);
      }

      if(difficultyMessageTimer > 0.f){
        difficultyMessageTimer -= dt;
      }

      std::erase_if(projectiles,
                    [](const Projectile &p) { return p.isDead(); });

      for (auto &enemy : enemies) {
        if(enemy.isDead()) continue;
        for (auto &proj : projectiles) {
          if (enemy.getBounds().findIntersection(proj.getBounds())) {
            hms.play();
            enemy.kill();
            score += 10;
            scoreText.setString("Wynik: " + std::to_string(score));
            proj.kill();
          }
        }
      }

      for (auto &enemy : enemies) {
        if (player.getBounds().findIntersection(enemy.getBounds())) {
          player.takeDamage();
        }
      }
      std::erase_if(enemies, [](const Enemy &e) { return e.isDead(); });

      if (player.isDead()) {
        ids.play();
        state = GameState::GameOver;
      }
    }

    window.clear();
    window.draw(bgc);
    
    if (state == GameState::Playing) {
      player.draw(window);
      for (auto &enemy : enemies)
      enemy.draw(window);
      for (auto &p : projectiles)
      p.draw(window);
      for (int i = 0; i < player.getHp(); i++) {
        sf::RectangleShape heart({30.f, 30.f});
        heart.setFillColor(sf::Color::Red);
        heart.setPosition({20.f + i * 40.f, 20.f});
        window.draw(heart);
        window.draw(scoreText);
        window.draw(survivalText);
        if(difficultyMessageTimer > 0.f){
          window.draw(difficultyText);
        }
      }
    } else {
      window.draw(gameOverText);
    }

    window.display();
  }
  return 0;
}