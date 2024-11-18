#include "Game.h"
#include <imgui.h>
#include <fstream>
#include <iostream>
#include <string>
#include "random.cpp"

void Game::init(const std::string& config)
{
	//load screen width, height, framerate, and fullscreen from config file
	std::ifstream fin(config);
	std::string token;	
	int width, height, frameRate;
	bool isFullscreen;
	fin >> token >> width >> height >> frameRate >> isFullscreen;
	if (isFullscreen) {
		m_window.create(sf::VideoMode(width, height), "Shapes War", sf::Style::Fullscreen);
	}
	else{
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
	m_text = sf::Text("Score: 0", m_font, fontSize);

	// player config
	PlayerConfig& p = m_playerConfig;
	fin >> token >> p.SR >> p.CR >> p.S >>p.FR>> p.FG >> p.FB >> p.OR >> p.OG >> p.OB >> p.OT >> p.V;

	// enemy config
	EnemyConfig& e = m_enemyConfig;	 
	fin >> token >> e.SR >> e.CR >> e.SMIN >> e.SMAX >> e.OR >> e.OG >> e.OB >> e.OT >> e.VMIN >> e.VMAX >> e.L >> e.SI;

	// bullet config
	ProjectileConfig& b = m_projectileConfig;  
	fin >> token >> b.SR >> b.CR >> b.S >> b.FR >> b.FG >> b.FB >> b.OR >> b.OG >> b.OB >> b.OT >> b.V >> b.L;
	this->sSpawnPlayer();

}
std::shared_ptr<Entity>Game::player() {
	auto player = m_entityManager.getEntities(EntityType::Player);
	assert(player.size() == 1);
	//check if player is active
	assert(player.front()->IsActive());
	//check if player has lifespan component
	assert(!player.front()->has<CLifeSpan>());
	return player.front();
	

}

void Game::setPaused(bool paused)
{
	m_paused = paused;

}

void Game::sMovement()
{
	//rest player velocity to 0
	auto playerEntity = player();

	auto& playerTransform = playerEntity->get<CTransform>();
	auto& playerInput = playerEntity->get<CInput>();
	playerTransform.valocity = { 0,0 };
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
	for (auto& entity : entities)
	{
		if (entity->has<CTransform>())
		{
			auto& transform = entity->get<CTransform>();
			transform.position += transform.valocity;
			transform.rotation += 1;
		}
	}
}

void Game::sRender()
{
	m_window.clear(sf::Color(18, 33, 43));
	auto entities = m_entityManager.getEntities();
	for (auto& entity : entities)
	{
		if (entity->has<CShape>() && entity->has<CTransform>())
		{
			entity->get<CTransform>().rotation += 1.0f;
			auto& shape = entity->get<CShape>().circle;
			auto& transform = entity->get<CTransform>();
			shape.setPosition(transform.position);
			shape.setRotation(transform.rotation);
			m_window.draw(shape);
		}
	}
	ImGui::SFML::Render(m_window);
	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		//switch case for up down left right shoot using mouse
		if (event.type == sf::Event::KeyPressed) {
			auto playerEntity = player();
			auto& playerInput = playerEntity->get<CInput>();
			switch (event.key.code)
			{
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
			auto& playerInput = playerEntity->get<CInput>();
			switch (event.key.code)
			{
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
			auto& playerInput = playerEntity->get<CInput>();
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
			auto& playerInput = playerEntity->get<CInput>();
			if (event.mouseButton.button == sf::Mouse::Left) {
				playerInput.shoot = false;
			}
		}




	}
}

void Game::sCollision()
{
	//check collision between Projectile and enemy
	auto projectiles = m_entityManager.getEntities(EntityType::Projectile);
	auto enemies = m_entityManager.getEntities(EntityType::Enemy);

	for (auto& projectile : projectiles) {
		if (projectile->has<CCollison>() && projectile->has<CTransform>()) {
			auto& projectileCollision = projectile->get<CCollison>();
			auto& projectileTransform = projectile->get<CTransform>();
			for (auto& enemy : enemies) {
				if (enemy->has<CCollison>() && enemy->has<CTransform>()) {
					auto& enemyCollision = enemy->get<CCollison>();
					auto& enemyTransform = enemy->get<CTransform>();
					float distance = (pow(projectileTransform.position.x - enemyTransform.position.x, 2) + pow(projectileTransform.position.y - enemyTransform.position.y, 2));
					if (distance < (pow (projectileCollision.radius + enemyCollision.radius,2))) {
						projectile->Destroy();
						enemy->Destroy();
						m_score += 10;
					}
				}
			}
		}
	}


}

void Game::sEnemySpawnr()
{
	if (m_currentFrame - m_lastEnemySpawn > m_enemyConfig.SI) {
		sSpawnEnemy();
	}




}

void Game::sProjectileSpawn()
{
}

void Game::sScore()
{
}


void Game::sLifeSpan()
{
	//get all entities with lifespan component'
	for (const auto e : m_entityManager.getEntities()) {
		if (e->has<CLifeSpan>()) {
			auto& lifeSpan = e->get<CLifeSpan>();
			lifeSpan.lifeTime--;
			if (lifeSpan.lifeTime <= 0) {
				e->Destroy();
			}
		}
	}

}

void Game::sGUI()
{
	ImGui::Begin("Game Info");
	ImGui::Text("Score: %d", m_score);
	if (ImGui::Button("Exit")) {
		m_running = false;
	}
	ImGui::End();
}


void Game::sSpawnPlayer()
{
	auto player = m_entityManager.AddEntity(EntityType::Player);
	//load from PlayerConfig struct
	auto& transform = player->add<CTransform>();
	auto& shape = player->add<CShape>();
	auto& collision = player->add<CCollison>();
	auto& score = player->add<CScore>();
	auto& input = player->add<CInput>();
	transform.position = { m_window.getSize().x / 2.0f,m_window.getSize().y / 2.0f};
	transform.rotation = 0;
	transform.valocity = { 0,0 };
	shape.circle.setRadius(m_playerConfig.SR);
	shape.circle.setPointCount(m_playerConfig.V);
	shape.circle.setFillColor(sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB));
	shape.circle.setOutlineColor(sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB));
	shape.circle.setOutlineThickness(2);
	shape.circle.setOutlineThickness(m_playerConfig.OT);
	shape.circle.setOrigin(m_playerConfig.SR, m_playerConfig.SR);
	collision.radius = m_playerConfig.CR;



}

void Game::sSpawnEnemy()
{
	auto enemy = m_entityManager.AddEntity(EntityType::Enemy);
	auto& transform = enemy->add<CTransform>();
	auto& shape = enemy->add<CShape>();
	auto& collision = enemy->add<CCollison>();
	transform.position = {getRandomValue ((float) m_enemyConfig.SR, (float)m_window.getSize().x- m_enemyConfig.SR), 
		getRandomValue((float)m_enemyConfig.SR,(float) m_window.getSize().y- m_enemyConfig.SR) };
	//give random velocity to enemy in random direction between VMIN and VMAX
	float angle = getRandomValue(0, 360) * 3.14159f / 180.0f;
	transform.valocity = { cos(angle) * getRandomValue(m_enemyConfig.SMIN, m_enemyConfig.SMAX), sin(angle) * getRandomValue(m_enemyConfig.SMIN, m_enemyConfig.SMAX) };
	transform.rotation = 0;
	shape.circle.setRadius(m_enemyConfig.SR);
	shape.circle.setPointCount(getRandomValue(m_enemyConfig.VMIN, m_enemyConfig.VMAX));
	//fill wite random color
	shape.circle.setFillColor(sf::Color(getRandomValue(0, 255), getRandomValue(0, 255), getRandomValue(0, 255)));
	//outline with color from config
	shape.circle.setOutlineColor(sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB));
	shape.circle.setOutlineThickness(m_enemyConfig.OT);
	shape.circle.setOrigin(m_enemyConfig.SR, m_enemyConfig.SR);
	collision.radius = m_enemyConfig.CR;
	m_lastEnemySpawn = m_currentFrame;


	
}

void Game::sSpawnSmallEnemies(std::shared_ptr<Entity> entity)
{
}

void Game::sSpawnProjectile(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos)
{
	auto projectile = m_entityManager.AddEntity(EntityType::Projectile);
	auto& transform = projectile->add<CTransform>();
	auto& shape = projectile->add<CShape>();
	auto& collision = projectile->add<CCollison>();
	auto& lifeSpan = projectile->add<CLifeSpan>();
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
	shape.circle.setRadius(m_projectileConfig.SR);
	shape.circle.setPointCount(m_projectileConfig.V);
	shape.circle.setFillColor(sf::Color(m_projectileConfig.FR, m_projectileConfig.FG, m_projectileConfig.FB));
	shape.circle.setOutlineColor(sf::Color(m_projectileConfig.OR, m_projectileConfig.OG, m_projectileConfig.OB));
	shape.circle.setOutlineThickness(m_projectileConfig.OT);
	shape.circle.setOrigin(m_projectileConfig.SR, m_projectileConfig.SR);
	collision.radius = m_projectileConfig.CR;
}

Game::Game(const std::string& configFilePath)
{
	init(configFilePath);
}

void Game::run()
{
	//todo add pause and exit button


	while (m_running)
	{
		m_entityManager.Update();
		ImGui::SFML::Update(m_window, m_deltaClock.restart());
		sEnemySpawnr();
		sUserInput();
		sMovement();
		sCollision();

		sLifeSpan();
		sGUI();
		sRender();
		m_currentFrame++;	


	}
}
