#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <random>
#include <string>
#include <vector>

#include "enemy.hpp"
#include "player.hpp"
#include "projectile.hpp"

int main() {
  sf::SoundBuffer isaacDeathSound("isaacDEATH.wav");
  sf::SoundBuffer hitMarkerSound("hitmarker.wav");

  sf::Sound ids(isaacDeathSound);
  sf::Sound hms(hitMarkerSound);

  sf::Font font;
  font.openFromFile("game_over.ttf");

  sf::Text gameOverText(font, "GAME OVER - nacisnij R", 50);
  gameOverText.setFillColor(sf::Color::Red);
  gameOverText.setPosition({400.f, 450.f});

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

  sf::Clock clock;

  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<float> distX(0.f, 1400.f);
  std::uniform_real_distribution<float> distY(0.f, 1000.f);

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
          state = GameState::Playing;
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
      survivalText.setString(
          "Czas: " + std::to_string(survivalTime).substr(0, 4) + "s");
      spawnTimer += dt;
      difficultyTimer += dt;

      if (difficultyTimer >= 10.f) {
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

      if (difficultyMessageTimer > 0.f) {
        difficultyMessageTimer -= dt;
      }

      std::erase_if(projectiles,
                    [](const Projectile &p) { return p.isDead(); });

      for (auto &enemy : enemies) {
        if (enemy.isDead())
          continue;
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
      }
      window.draw(scoreText);
      window.draw(survivalText);
      if (difficultyMessageTimer > 0.f) {
        window.draw(difficultyText);
      }
    } else {
      window.draw(gameOverText);
    }

    window.display();
  }
  return 0;
}