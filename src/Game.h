// Game.h
#ifndef GAME_H
#define GAME_H

#include "EntityManager.hpp"
#include "SFML/Graphics.hpp"
#include "imgui-SFML.h"
#include <SFML/Audio.hpp>
#include "EntityConfigs.h"

#define POINTS 5


class Game {
    sf::RenderWindow m_window;
    EntityManager m_entityManager;
    sf::Text m_scoreText;
    sf::Font m_font;
    //add music component
    sf::Music m_music;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    ProjectileConfig m_projectileConfig;
    sf::Clock m_deltaClock;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawn = 0;
    bool m_stopMovement = false;
    bool m_stopEnemySpawn = false;
    bool m_stopLifeSpan = false;
    bool m_stopCollision = false;
    bool m_paused = false;
    bool m_running = true;
    void init(const std::string& config);
    void loadMusic(std::string& musicFile);
    void setPaused(bool paused);
    void sMovement(); // Entity position/movement update
    void sRender(); // Render all entities
    void sUserInput(); // User input
    void sCollision(); // Collision detection
    void sEnemySpawnr(); // Spawn enemies
    void sAblityProjectileSpawn(std::shared_ptr<Entity> entity); // Spawn projectiles
    void sLifeSpan(); // Update life span
    void sGUI(); // Render GUI
    void sSpawnPlayer(); // Spawn player
    void sSpawnEnemy(); // Spawn enemy
    void sSpawnSmallEnemies(std::shared_ptr<Entity> entity); // Spawn small enemy
    void sSpawnProjectile(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos); // Spawn projectile
    std::shared_ptr<Entity> player();
public:
    Game(const std::string& configFilePath);
    void run();
};

#endif // GAME_H
