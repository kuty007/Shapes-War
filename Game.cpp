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
	return player.front();
	

}

void Game::setPaused(bool paused)
{
	m_paused = paused;

}

void Game::sMovement()
{
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

void Game::sCollision()
{
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
}

void Game::sGUI()
{
}

void Game::sSpawnPlayer()
{
	auto player = m_entityManager.AddEntity(EntityType::Player);
	//load from PlayerConfig struct
	auto& transform = player->add<CTransform>();
	auto& shape = player->add<CShape>();
	auto& collision = player->add<CCollison>();
	auto& score = player->add<CScore>();
	auto& lifeSpan = player->add<CLifeSpan>();
	auto& input = player->add<CInput>();
	//set up player
	//position of middle of the screen



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
	transform.position = {getRandomValue ((float) m_enemyConfig.SR, (float)m_window.getSize().x- m_enemyConfig.SR), getRandomValue((float)m_enemyConfig.SR,(float) m_window.getSize().y- m_enemyConfig.SR) };
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
	m_lastEnemySpawn = m_currentFrame;


	
}

void Game::sSpawnSmallEnemies(std::shared_ptr<Entity> entity)
{
}

void Game::sSpawnProjectile(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos)
{
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
		sMovement();
		sCollision();
		sUserInput();
		sGUI();
		sRender();
		m_currentFrame++;	


	}
}
