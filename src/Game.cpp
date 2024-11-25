#include "Game.h"
#include <imgui.h>
#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <random>


// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

// Function to generate a random integer within a specified range
int getRandomValue(int minValue, int maxValue) {
    std::uniform_int_distribution<> dis(minValue, maxValue);
    return dis(gen);
}

// Function to generate a random float within a specified range
float getRandomValue(float minValue, float maxValue) {
    std::uniform_real_distribution<> dis(minValue, maxValue);
    return dis(gen);
}
void Game::loadMusic(std::string& musicFile)
{
    if (!m_music.openFromFile(musicFile))
    {
        std::cerr << "Could not load music file!\n";
        exit(-1);
    }
    m_music.setLoop(true);
    m_music.play();
}



void Game::init(const std::string &config) {
    //load screen width, height, framerate, and fullscreen from config file
    std::ifstream fin(config);
    std::string token;
    int width, height, frameRate;
    bool isFullscreen;
    fin >> token >> width >> height >> frameRate >> isFullscreen;
    if (isFullscreen) {
        m_window.create(sf::VideoMode(width, height), "Shapes War", sf::Style::Fullscreen);
    } else {
        m_window.create(sf::VideoMode(width, height), "Shapes War");
    }
    m_window.setFramerateLimit(frameRate);
    ImGui::SFML::Init(m_window);


    //load font from config file
    std::string fontName;
    int fontSize, fontR, fontG, fontB;
    fin >> token >> fontName >> fontSize >> fontR >> fontG >> fontB;
    if (!m_font.loadFromFile(fontName)) {
        std::cerr << "Could not load font!\n";
        exit(-1);
    }
    m_scoreText.setFont(m_font);
    m_scoreText.setCharacterSize(fontSize);
    m_scoreText.setFillColor(sf::Color(fontR, fontG, fontB));
    m_scoreText.setPosition(10, 10);
    std::string musicFile;
    fin >> token >> musicFile;
    loadMusic(musicFile);




    // player config
    PlayerConfig &p = m_playerConfig;
    fin >> token >> p.SR >> p.CR >> p.S >> p.FR >> p.FG >> p.FB >> p.OR >> p.OG >> p.OB >> p.OT >> p.V;

    // enemy config
    EnemyConfig &e = m_enemyConfig;
    fin >> token >> e.SR >> e.CR >> e.SMIN >> e.SMAX >> e.OR >> e.OG >> e.OB >> e.OT >> e.VMIN >> e.VMAX >> e.L >> e.SI;

    // bullet config
    ProjectileConfig &b = m_projectileConfig;
    fin >> token >> b.SR >> b.CR >> b.S >> b.FR >> b.FG >> b.FB >> b.OR >> b.OG >> b.OB >> b.OT >> b.V >> b.L;
    this->sSpawnPlayer();

}

std::shared_ptr <Entity> Game::player() {
    auto player = m_entityManager.getEntities(EntityType::Player).front();
    if (player == nullptr)
      return nullptr;
    return player;


}

void Game::setPaused(bool paused) {
    m_paused = paused;

}

void Game::sMovement() {
    if (m_stopMovement) return;
    //rest player velocity to 0
    auto playerEntity = player();
    auto &playerTransform = playerEntity->get<CTransform>();
    auto &playerInput = playerEntity->get<CInput>();
    playerTransform.valocity = {0, 0};
    if (playerInput.up) {
        playerTransform.valocity.y = -m_playerConfig.S;
    }
    if (playerInput.down) {
        playerTransform.valocity.y = m_playerConfig.S;
    }
    if (playerInput.left) {
        playerTransform.valocity.x = -m_playerConfig.S;
    }
    if (playerInput.right) {
        playerTransform.valocity.x = m_playerConfig.S;
    }
    auto entities = m_entityManager.getEntities();
    for (auto &entity: entities) {
        if (entity->has<CTransform>()) {
            auto &transform = entity->get<CTransform>();
            transform.position += transform.valocity;
            transform.rotation += 1;
        }
    }
}

void Game::sRender() {
    m_window.clear(sf::Color(18, 33, 43));
    auto entities = m_entityManager.getEntities();
    for (auto &entity: entities) {
        if (entity->has<CShape>() && entity->has<CTransform>()) {
            entity->get<CTransform>().rotation += 1.0f;
            auto &shape = entity->get<CShape>().circle;
            auto &transform = entity->get<CTransform>();
            shape.setPosition(transform.position);
            shape.setRotation(transform.rotation);
            if (entity->has<CLifeSpan>()) {
                // set opacity proportional to it's remaining lifespan
                auto &lifeSpan = entity->get<CLifeSpan>();
                int alpha = 255 * lifeSpan.RemainingLifeTime / lifeSpan.lifeTime;
                shape.setFillColor(
                        sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                // set outline color with same alpha value
                shape.setOutlineColor(
                        sf::Color(shape.getOutlineColor().r, shape.getOutlineColor().g, shape.getOutlineColor().b,
                                  alpha));


            }

            m_window.draw(shape);
        }
    }
    //render text
    m_scoreText.setString("Score: " + std::to_string(m_score));
    m_window.draw(m_scoreText);

    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            m_running = false;
        }
        if (event.type == sf::Event::LostFocus) {
            setPaused(true);
        }
        if (event.type == sf::Event::GainedFocus) {
            setPaused(false);
        }
        // List of keys to process
        std::set <sf::Keyboard::Key> keysToProcess = {
                sf::Keyboard::W,
                sf::Keyboard::S,
                sf::Keyboard::A,
                sf::Keyboard::D,
                sf::Keyboard::Space,
                sf::Keyboard::P,
                sf::Keyboard::Escape
        };

        // Ignore all other keys
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            if (keysToProcess.find(event.key.code) == keysToProcess.end()) {
                continue;
            }
        }


        //switch case for up down left right shoot using mouse
        if (event.type == sf::Event::KeyPressed) {
            auto playerEntity = player();
            auto &playerInput = playerEntity->get<CInput>();
            switch (event.key.code) {
                case sf::Keyboard::W:
                    playerInput.up = true;
                    break;
                case sf::Keyboard::S:
                    playerInput.down = true;
                    break;
                case sf::Keyboard::A:
                    playerInput.left = true;
                    break;
                case sf::Keyboard::D:
                    playerInput.right = true;
                    break;
                case sf::Keyboard::Space:
                    playerInput.ability = true;
                    if (!m_paused) {
                        sAblityProjectileSpawn(playerEntity);
                    }
                    break;
                case sf::Keyboard::P:
                    setPaused(!m_paused);
                    break;
                case sf::Keyboard::Escape:
                    m_running = false;
                    break;
                default:
                    break;
            }


        }
        if (event.type == sf::Event::KeyReleased) {
            auto playerEntity = player();
            auto &playerInput = playerEntity->get<CInput>();
            switch (event.key.code) {
                case sf::Keyboard::W:
                    playerInput.up = false;
                    break;
                case sf::Keyboard::S:
                    playerInput.down = false;
                    break;
                case sf::Keyboard::A:
                    playerInput.left = false;
                    break;
                case sf::Keyboard::D:
                    playerInput.right = false;
                    break;
                case sf::Keyboard::Space:
                    playerInput.ability = false;
                    break;
                default:
                    break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            auto playerEntity = player();
            auto &playerInput = playerEntity->get<CInput>();
            if (event.mouseButton.button == sf::Mouse::Left) {
                playerInput.shoot = true;
                sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
                //check if game is nor paused and if so spawn projectile
                if (!m_paused) {
                    sSpawnProjectile(playerEntity, mousePos);
                }
            }

        }
        if (event.type == sf::Event::MouseButtonReleased) {
            auto playerEntity = player();
            auto &playerInput = playerEntity->get<CInput>();
            if (event.mouseButton.button == sf::Mouse::Left) {
                playerInput.shoot = false;
            }
        }


    }
}

void Game::sCollision() {
    if (m_stopCollision) return;
    //check collision between Projectile and enemy
    auto projectiles = m_entityManager.getEntities(EntityType::Projectile);
    auto abilityProjectiles = m_entityManager.getEntities(EntityType::AbilityProjectile);
    auto enemies = m_entityManager.getEntities(EntityType::Enemy);

    for (auto &projectile: projectiles) {
        if (projectile->has<CCollison>() && projectile->has<CTransform>()) {
            auto &projectileCollision = projectile->get<CCollison>();
            auto &projectileTransform = projectile->get<CTransform>();
            for (auto &enemy: enemies) {
                if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
                    auto &enemyCollision = enemy->get<CCollison>();
                    auto &enemyTransform = enemy->get<CTransform>();
                    float distance = (pow(projectileTransform.position.x - enemyTransform.position.x, 2) +
                                      pow(projectileTransform.position.y - enemyTransform.position.y, 2));
                    if (distance < (pow(projectileCollision.radius + enemyCollision.radius, 2))) {
                        projectile->Destroy();
                        enemy->Destroy();
                        sSpawnSmallEnemies(enemy);
                        m_score += enemy->get<CScore>().score;;
                    }
                }
            }
            //do the same for small enemy
            auto smallEnemies = m_entityManager.getEntities(EntityType::SmallEnemy);
            for (auto &enemy: smallEnemies) {
                if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
                    auto &enemyCollision = enemy->get<CCollison>();
                    auto &enemyTransform = enemy->get<CTransform>();
                    float distance = (pow(projectileTransform.position.x - enemyTransform.position.x, 2) +
                                      pow(projectileTransform.position.y - enemyTransform.position.y, 2));
                    if (distance < (pow(projectileCollision.radius + enemyCollision.radius, 2))) {
                        projectile->Destroy();
                        enemy->Destroy();
                        m_score += enemy->get<CScore>().score;;
                    }
                }
            }
        }
    }
    //do the same for ability projectile

    for (auto &proj: abilityProjectiles) {
        if (proj->has<CCollison>() && proj->has<CTransform>()) {
            auto &projCollision = proj->get<CCollison>();
            auto &projTransform = proj->get<CTransform>();
            for (auto &enemy: enemies) {
                if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
                    auto &enemyCollision = enemy->get<CCollison>();
                    auto &enemyTransform = enemy->get<CTransform>();
                    float distance = (pow(projTransform.position.x - enemyTransform.position.x, 2) +
                                      pow(projTransform.position.y - enemyTransform.position.y, 2));
                    if (distance < (pow(projCollision.radius + enemyCollision.radius, 2))) {
                        enemy->Destroy();
                        sSpawnSmallEnemies(enemy);
                        m_score += enemy->get<CScore>().score;
                    }
                }
            }
            //do the same for small enemy
            auto smallEnemies = m_entityManager.getEntities(EntityType::SmallEnemy);
            for (auto &enemy: smallEnemies) {
                if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
                    auto &enemyCollision = enemy->get<CCollison>();
                    auto &enemyTransform = enemy->get<CTransform>();
                    float distance = (pow(projTransform.position.x - enemyTransform.position.x, 2) +
                                      pow(projTransform.position.y - enemyTransform.position.y, 2));
                    if (distance < (pow(projCollision.radius + enemyCollision.radius, 2))) {
                        enemy->Destroy();
                        m_score += enemy->get<CScore>().score;
                    }
                }
            }
        }
    }
    //for ability projectile if its colide with screen edge reverse its velocity
    for (auto &proj: abilityProjectiles) {
        if (proj->has<CCollison>() && proj->has<CTransform>()) {
            auto &projCollision = proj->get<CCollison>();
            auto &projTransform = proj->get<CTransform>();
            if (projTransform.position.x - projCollision.radius < 0 ||
                projTransform.position.x + projCollision.radius > m_window.getSize().x) {
                projTransform.valocity.x *= -1;
            }
            if (projTransform.position.y - projCollision.radius < 0 ||
                projTransform.position.y + projCollision.radius > m_window.getSize().y) {
                projTransform.valocity.y *= -1;
            }
        }
    }

    //for enamy if its colide with screen edge reverse its velocity
    for (auto &enemy: enemies) {
        if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
            auto &enemyCollision = enemy->get<CCollison>();
            auto &enemyTransform = enemy->get<CTransform>();
            if (enemyTransform.position.x - enemyCollision.radius < 0 ||
                enemyTransform.position.x + enemyCollision.radius > m_window.getSize().x) {
                enemyTransform.valocity.x *= -1;
            }
            if (enemyTransform.position.y - enemyCollision.radius < 0 ||
                enemyTransform.position.y + enemyCollision.radius > m_window.getSize().y) {
                enemyTransform.valocity.y *= -1;
            }
        }
    }
    //for player if its colide with screen edge dont let it go out of screen
    auto playerEntity = player();
    auto &playerCollision = playerEntity->get<CCollison>();
    auto &playerTransform = playerEntity->get<CTransform>();
    if (playerTransform.position.x - playerCollision.radius < 0) {
        playerTransform.position.x = playerCollision.radius;
    }
    if (playerTransform.position.x + playerCollision.radius > m_window.getSize().x) {
        playerTransform.position.x = m_window.getSize().x - playerCollision.radius;
    }
    if (playerTransform.position.y - playerCollision.radius < 0) {
        playerTransform.position.y = playerCollision.radius;
    }
    if (playerTransform.position.y + playerCollision.radius > m_window.getSize().y) {
        playerTransform.position.y = m_window.getSize().y - playerCollision.radius;
    }
    //check collision between player and enemy
    for (auto &enemy: enemies) {
        if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
            auto &enemyCollision = enemy->get<CCollison>();
            auto &enemyTransform = enemy->get<CTransform>();
            float distance = (pow(playerTransform.position.x - enemyTransform.position.x, 2) +
                              pow(playerTransform.position.y - enemyTransform.position.y, 2));
            if (distance < (pow(playerCollision.radius + enemyCollision.radius, 2))) {

                enemy->Destroy();
                sSpawnSmallEnemies(enemy);
               	playerEntity->Destroy();
                // chck if the plyar havnt been spwend from ather collisn

                sSpawnPlayer();

                m_score = 0;


            }
        }
    }
    //check collision between player and small enemy
    auto smallEnemies = m_entityManager.getEntities(EntityType::SmallEnemy);
    for (auto &enemy: smallEnemies) {
        if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
            auto &enemyCollision = enemy->get<CCollison>();
            auto &enemyTransform = enemy->get<CTransform>();
            float distance = (pow(playerTransform.position.x - enemyTransform.position.x, 2) +
                              pow(playerTransform.position.y - enemyTransform.position.y, 2));
            if (distance < (pow(playerCollision.radius + enemyCollision.radius, 2))) {
              	enemy->Destroy();
                playerEntity->Destroy();
                sSpawnPlayer();
                m_score = 0;
            }
        }
    }


}

void Game::sEnemySpawnr() {
    if (m_stopEnemySpawn) return;
    if (m_currentFrame - m_lastEnemySpawn > m_enemyConfig.SI) {
        sSpawnEnemy();
    }


}

void Game::sAblityProjectileSpawn(std::shared_ptr <Entity> entity) {
    if (m_score - player()->get<CAbility>().lastUsed < 2500) {
        return;
    }


    int numVertices = entity->get<CShape>().circle.getPointCount();
    int step = 360 / numVertices;
    //spawn small enemies at each vertex
    for (int i = 0; i < numVertices; i++) {
        auto abporj = m_entityManager.AddEntity(EntityType::AbilityProjectile);
        auto &transform = abporj->add<CTransform>();
        auto &shape = abporj->add<CShape>();
        auto &collision = abporj->add<CCollison>();
        auto &lifeSpan = abporj->add<CLifeSpan>();
        transform.position = entity->get<CTransform>().position;
        // Each small enemy travel outwards at fixed intervals equal to
        // 360/vertices
        float angle = i * step * 3.14159f / 180.0f;
        transform.valocity = {static_cast<float> (cos(angle)) * m_projectileConfig.S * 2, static_cast<float> (sin(angle)) * m_projectileConfig.S * 2};
        transform.rotation = 0;
        lifeSpan.lifeTime = m_projectileConfig.L * 3;
        lifeSpan.RemainingLifeTime = m_projectileConfig.L * 3;
        shape.circle.setRadius(m_projectileConfig.SR);
        shape.circle.setPointCount(m_projectileConfig.V);
        shape.circle.setFillColor(sf::Color(m_projectileConfig.FR, m_projectileConfig.FG, m_projectileConfig.FB));
        shape.circle.setOutlineColor(sf::Color(m_projectileConfig.OR, m_projectileConfig.OG, m_projectileConfig.OB));
        shape.circle.setOutlineThickness(m_projectileConfig.OT);
        shape.circle.setOrigin(m_projectileConfig.SR, m_projectileConfig.SR);
        collision.radius = m_projectileConfig.CR;
        //set last used ability time
        player()->get<CAbility>().lastUsed = m_score;

    }
}




void Game::sLifeSpan() {
    if (m_stopLifeSpan) return;
    //get all entities with lifespan component'
    for (const auto e: m_entityManager.getEntities()) {
        if (e->has<CLifeSpan>()) {
            auto &lifeSpan = e->get<CLifeSpan>();
            lifeSpan.RemainingLifeTime--;
            if (lifeSpan.RemainingLifeTime <= 0) {
                e->Destroy();
            }
        }
    }

}

void Game::sGUI() {
    ImGui::Begin("Game Info");

    if (ImGui::BeginTabBar("Game Settings")) {
        // Tab for general game information
        if (ImGui::BeginTabItem("Info")) {
            ImGui::Text("Score: %d", m_score);
            if (ImGui::Button("Exit")) {
                m_running = false;
            }
            ImGui::EndTabItem();
        }

        // Tab for spawning entities manually
        if (ImGui::BeginTabItem("Spawn")) {
            if (ImGui::Button("Spawn Enemy")) {
                sSpawnEnemy();
            }
            ImGui::EndTabItem();
        }

        // Tab for changing spawn rate
        if (ImGui::BeginTabItem("Spawn Rate")) {
            ImGui::SliderInt("Enemy Spawn Interval", &m_enemyConfig.SI, 1, 1000);
            ImGui::EndTabItem();
        }

        // Tab for stopping systems
        if (ImGui::BeginTabItem("Systems")) {
            ImGui::Checkbox("Stop Movement", &m_stopMovement);
            ImGui::Checkbox("Stop Spawning", &m_stopEnemySpawn);
            ImGui::Checkbox("Stop Life Span", &m_stopLifeSpan);
            ImGui::Checkbox("Stop Collision", &m_stopCollision);
            ImGui::EndTabItem();
        }

        // Tab for controlling entities
        if (ImGui::BeginTabItem("Entities")) {
            auto entities = m_entityManager.getEntities();
            std::vector <EntityType> entityTypes = {EntityType::Player, EntityType::Enemy, EntityType::SmallEnemy,
                                                    EntityType::Projectile};
            static int entityTypeIndex = 0;
            ImGui::Combo("Entity Type", &entityTypeIndex, "Player\0Enemy\0Small Enemy\0Projectile\0");

            for (const auto &entity: entities) {
                if (entity->GetType() == entityTypes[entityTypeIndex]) {
                    ImGui::Text("Entity ID: %zu", entity->GetID());
                    if (entity->has<CTransform>()) {
                        auto &transform = entity->get<CTransform>();
                        ImGui::Text("Position: (%.2f, %.2f)", transform.position.x, transform.position.y);
                        ImGui::Text("Velocity: (%.2f, %.2f)", transform.valocity.x, transform.valocity.y);
                    }
                    if (entity->has<CLifeSpan>()) {
                        auto &lifeSpan = entity->get<CLifeSpan>();
                        ImGui::Text("Life Time: %d", lifeSpan.lifeTime);
                        ImGui::Text("Remaining Life Time: %d", lifeSpan.RemainingLifeTime);
                    }
                    ImGui::Separator();
                    if (ImGui::Button("Destroy Entity")) {
                        entity->Destroy();
                    }
                }
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}


void Game::sSpawnPlayer() {
    // Check if a player already exists
    auto player = m_entityManager.AddEntity(EntityType::Player);
    //load from PlayerConfig struct
    auto &transform = player->add<CTransform>();
    auto &shape = player->add<CShape>();
    auto &collision = player->add<CCollison>();
    auto &input = player->add<CInput>();
    transform.position = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f};
    transform.rotation = 0;
    transform.valocity = {0, 0};
    shape.circle.setRadius(m_playerConfig.SR);
    shape.circle.setPointCount(m_playerConfig.V);
    shape.circle.setFillColor(sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB));
    shape.circle.setOutlineColor(sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB));
    shape.circle.setOutlineThickness(2);
    shape.circle.setOutlineThickness(m_playerConfig.OT);
    shape.circle.setOrigin(m_playerConfig.SR, m_playerConfig.SR);
    collision.radius = m_playerConfig.CR;

}

void Game::sSpawnEnemy() {
    auto enemy = m_entityManager.AddEntity(EntityType::Enemy);
    auto &transform = enemy->add<CTransform>();
    auto &shape = enemy->add<CShape>();
    auto &collision = enemy->add<CCollison>(m_enemyConfig.CR);
    transform.position = {getRandomValue((float) m_enemyConfig.SR, (float) m_window.getSize().x - m_enemyConfig.SR),
                          getRandomValue((float) m_enemyConfig.SR, (float) m_window.getSize().y - m_enemyConfig.SR)};
    //give random velocity to enemy in random direction between VMIN and VMAX
    float angle = getRandomValue(0, 360) * 3.14159f / 180.0f;
    transform.valocity = {static_cast<float> (cos(angle))* getRandomValue(m_enemyConfig.SMIN, m_enemyConfig.SMAX),
                         static_cast<float> (sin(angle))* getRandomValue(m_enemyConfig.SMIN, m_enemyConfig.SMAX)};
    transform.rotation = 0;
    shape.circle.setRadius(m_enemyConfig.SR);
    shape.circle.setPointCount(getRandomValue(m_enemyConfig.VMIN, m_enemyConfig.VMAX));
    //fill wite random color
    shape.circle.setFillColor(sf::Color(getRandomValue(0, 255), getRandomValue(0, 255), getRandomValue(0, 255)));
    //outline with color from config
    shape.circle.setOutlineColor(sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB));
    shape.circle.setOutlineThickness(m_enemyConfig.OT);
    shape.circle.setOrigin(m_enemyConfig.SR, m_enemyConfig.SR);
    auto & score = enemy->add<CScore>(POINTS * (enemy->get<CShape>().circle.getPointCount()));//points * number of vertcs
    m_lastEnemySpawn = m_currentFrame;


}

void Game::sSpawnSmallEnemies(std::shared_ptr <Entity> entity) {
    //get the num of vertices of the entity
    int numVertices = entity->get<CShape>().circle.getPointCount();
    int step = 360 / numVertices;
    //spawn small enemies at each vertex
    for (int i = 0; i < numVertices; i++) {
        auto enemy = m_entityManager.AddEntity(EntityType::SmallEnemy);
        auto &transform = enemy->add<CTransform>();
        auto &shape = enemy->add<CShape>();
        auto &collision = enemy->add<CCollison>();
        auto &lifeSpan = enemy->add<CLifeSpan>(m_enemyConfig.L);
        auto & score = enemy->add<CScore>(entity->get<CScore>().score*2);
        transform.position = entity->get<CTransform>().position;
        // Each small enemy travel outwards at fixed intervals equal to
        // 360/vertices
        float angle = i * step * 3.14159f / 180.0f;
        transform.valocity = {static_cast<float> (cos(angle))* m_enemyConfig.SMIN, static_cast<float> (sin(angle)) * m_enemyConfig.SMIN};
        transform.rotation = 0;
        shape.circle.setRadius(m_enemyConfig.SR / 2);
        //set point
        shape.circle.setPointCount(numVertices);
        //coler same as entity
        shape.circle.setFillColor(entity->get<CShape>().circle.getFillColor());
        shape.circle.setOutlineColor(sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB));
        shape.circle.setOutlineThickness(m_enemyConfig.OT);
        shape.circle.setOrigin(m_enemyConfig.SR / 2, m_enemyConfig.SR / 2);
        collision.radius = m_enemyConfig.CR / 2;
    }
}

void Game::sSpawnProjectile(std::shared_ptr <Entity> entity, const sf::Vector2f &mousePos) {
    auto projectile = m_entityManager.AddEntity(EntityType::Projectile);
    auto &transform = projectile->add<CTransform>();
    auto &shape = projectile->add<CShape>();
    auto &collision = projectile->add<CCollison>();
    auto &lifeSpan = projectile->add<CLifeSpan>();
    transform.position = entity->get<CTransform>().position;
    transform.rotation = 0;
    //get direction from player to mouse
    sf::Vector2f direction = mousePos - transform.position;
    //normalize direction
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    direction.x /= length;
    direction.y /= length;
    //set velocity to direction * speed
    transform.valocity = direction * m_projectileConfig.S;
    lifeSpan.lifeTime = m_projectileConfig.L;
    lifeSpan.RemainingLifeTime = m_projectileConfig.L;
    shape.circle.setRadius(m_projectileConfig.SR);
    shape.circle.setPointCount(m_projectileConfig.V);
    shape.circle.setFillColor(sf::Color(m_projectileConfig.FR, m_projectileConfig.FG, m_projectileConfig.FB));
    shape.circle.setOutlineColor(sf::Color(m_projectileConfig.OR, m_projectileConfig.OG, m_projectileConfig.OB));
    shape.circle.setOutlineThickness(m_projectileConfig.OT);
    shape.circle.setOrigin(m_projectileConfig.SR, m_projectileConfig.SR);
    collision.radius = m_projectileConfig.CR;
}

Game::Game(const std::string &configFilePath) {
    init(configFilePath);
}

void Game::run() {



    while (m_running) {
      ImGui::SFML::Update(m_window, m_deltaClock.restart());
      if(!m_paused){
        m_entityManager.Update();
        sEnemySpawnr();
        sUserInput();
        sMovement();
        sCollision();
        sLifeSpan();
        }
        sUserInput();
        sGUI();
        sRender();
        m_currentFrame++;


    }
}
