// Game.h
#ifndef GAME_H
#define GAME_H

#include "EntityManager.hpp"
#include "SFML/Graphics.hpp"
#include "imgui-SFML.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct ProjectileConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
class Game
{
    sf::RenderWindow m_window;
    EntityManager m_entityManager;
    sf::Font m_font;
    sf::Text m_text;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    ProjectileConfig m_projectileConfig;
    sf::Clock m_deltaClock;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawn = 0;
    bool m_paused = false;
    bool m_running = true;
    void init(const std::string& config);
    void setPaused(bool paused);
    void sMovement(); // Entity position/movement update
    void sRender(); // Render all entities
    void sUserInput(); // User input
    void sCollision(); // Collision detection
    void sEnemySpawnr(); // Spawn enemies
    void sProjectileSpawn(); // Spawn projectiles
    void sScore(); // Update score
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
