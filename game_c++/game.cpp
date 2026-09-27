#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 1. Configuração da Janela (SFML 3 usa sf::Vector2u)
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Jogo de Aviao - Top Down");
    window.setFramerateLimit(60);

    // 2. Avião do Jogador
    sf::ConvexShape player;
    player.setPointCount(3);
    player.setPoint(0, {0.f, -20.f});  // Bico
    player.setPoint(1, {-15.f, 15.f}); // Asa Esquerda
    player.setPoint(2, {15.f, 15.f});  // Asa Direita
    player.setFillColor(sf::Color::Cyan);
    player.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 50.f});
    float playerSpeed = 6.0f;

    // 3. Vetores e Variáveis de Controle
    std::vector<sf::RectangleShape> bullets;
    float bulletSpeed = 10.0f;
    sf::Clock shootClock;

    std::vector<sf::RectangleShape> enemies;
    float enemySpeed = 3.0f;
    sf::Clock enemyClock;

    int score = 0;
    bool gameOver = false;

    // Game Loop Principal
    while (window.isOpen()) {
        // SFML 3: pollEvent() retorna std::optional<sf::Event>
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (!gameOver) {
            // --- ENTRADA DO JOGADOR (SFML 3 usa sf::Keyboard::Key::*) ---
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                if (player.getPosition().x > 20)
                    player.move({-playerSpeed, 0.f});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                if (player.getPosition().x < WINDOW_WIDTH - 20)
                    player.move({playerSpeed, 0.f});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                if (player.getPosition().y > 20)
                    player.move({0.f, -playerSpeed});
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                if (player.getPosition().y < WINDOW_HEIGHT - 20)
                    player.move({0.f, playerSpeed});
            }

            // Disparo de Tiros (com cadência de 0.2s)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && shootClock.getElapsedTime().asSeconds() > 0.2f) {
                sf::RectangleShape bullet({4.f, 12.f});
                bullet.setFillColor(sf::Color::Yellow);
                bullet.setPosition({player.getPosition().x - 2.f, player.getPosition().y - 20.f});
                bullets.push_back(bullet);
                shootClock.restart();
            }

            // --- SPAWN DE INIMIGOS ---
            if (enemyClock.getElapsedTime().asSeconds() > 0.8f) {
                sf::RectangleShape enemy({30.f, 30.f});
                enemy.setFillColor(sf::Color::Red);
                enemy.setPosition({static_cast<float>(rand() % (WINDOW_WIDTH - 30)), -30.f});
                enemies.push_back(enemy);
                enemyClock.restart();
            }

            // --- ATUALIZAÇÃO DE POSIÇÕES ---
            // Mover Tiros
            for (size_t i = 0; i < bullets.size(); ) {
                bullets[i].move({0.f, -bulletSpeed});
                if (bullets[i].getPosition().y < -10) {
                    bullets.erase(bullets.begin() + i);
                } else {
                    ++i;
                }
            }

            // Mover Inimigos
            for (size_t i = 0; i < enemies.size(); ) {
                enemies[i].move({0.f, enemySpeed});
                if (enemies[i].getPosition().y > WINDOW_HEIGHT) {
                    enemies.erase(enemies.begin() + i);
                } else {
                    ++i;
                }
            }

            // --- DETECÇÃO DE COLISÕES (SFML 3 usa findIntersection) ---
            // Tiro vs Inimigo
            for (size_t i = 0; i < bullets.size(); ) {
                bool bulletDestroyed = false;
                for (size_t j = 0; j < enemies.size(); ) {
                    if (bullets[i].getGlobalBounds().findIntersection(enemies[j].getGlobalBounds())) {
                        enemies.erase(enemies.begin() + j);
                        bulletDestroyed = true;
                        score += 10;
                        std::cout << "Pontos: " << score << std::endl;
                        break;
                    } else {
                        ++j;
                    }
                }
                if (bulletDestroyed) {
                    bullets.erase(bullets.begin() + i);
                } else {
                    ++i;
                }
            }

            // Jogador vs Inimigo
            for (const auto& enemy : enemies) {
                if (player.getGlobalBounds().findIntersection(enemy.getGlobalBounds())) {
                    gameOver = true;
                    std::cout << "GAME OVER! Pontuacao Final: " << score << std::endl;
                }
            }
        }

        // --- RENDERIZAÇÃO ---
        window.clear(sf::Color(10, 10, 30));

        if (!gameOver) {
            window.draw(player);
            for (const auto& bullet : bullets) {
                window.draw(bullet);
            }
            for (const auto& enemy : enemies) {
                window.draw(enemy);
            }
        }

        window.display();
    }

    return 0;
}