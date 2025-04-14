#include "Game.h"
#include <fstream>
#include <sstream>
#include <iostream>


Game::Game(const std::string& configFile)
{
	Init(configFile);
}

void Game::Run()
{

	while (Window.isOpen()) {


		ManuelSpawnFlag = true;

		sUserInput();

		if (IsRunning) {
			GameFrameCount++;
			EntityManager.Update();
			sLifeSpan();
			sSpawner();
			sMovement();
			sSpecialShoot();
			sCollision();
			sRender();
		}
	}

}

void Game::Init(const std::string& configFile)
{
	GameFrameCount = 0;
	IsRunning = true;
	
	//Read in the config file 
	std::fstream input{ configFile };

	if (!input.is_open())
	{
		#include <filesystem>
        std::cout << "Current path: " << std::filesystem::current_path() << '\n';
		std::cerr << "Failed to open: " << configFile << '\n';
		exit(-1);
	}

	//get data from txt file and assign to variables
	std::string identifier;

	while (input >> identifier) {
		if (identifier == "Window")
		{
			unsigned int width;
			input >> width;

			unsigned int height;
			input >> height;

			unsigned int fps;
			input >> fps;

			int fullscreen;
			input >> fullscreen;

			if (fullscreen == 1) {
				Window.create(sf::VideoMode({width, height}), "Geometry Wars", sf::State::Windowed);
			}
			else {
				auto fullScreenModes = sf::VideoMode::getFullscreenModes();
				Window.create(fullScreenModes[0], "Geometry Wars", sf::State::Fullscreen);
			}
			// bullshit fps = 1;
			Window.setFramerateLimit(fps);
		}

		else if (identifier == "Player") {
			input >> PlayerSpecs.ShapeRadius;
			input >> PlayerSpecs.CollisionRadius;
			input >> PlayerSpecs.Speed;

			// Read color values as int, then cast to uint8_t
			int r, g, b;
			input >> r;
			input >> g;
			input >> b;
			PlayerSpecs.FillColor.r = static_cast<uint8_t>(r);
			PlayerSpecs.FillColor.g = static_cast<uint8_t>(g);
			PlayerSpecs.FillColor.b = static_cast<uint8_t>(b);

			input >> r;
			input >> g;
			input >> b;
			PlayerSpecs.OutlineColor.r = static_cast<uint8_t>(r);
			PlayerSpecs.OutlineColor.g = static_cast<uint8_t>(g);
			PlayerSpecs.OutlineColor.b = static_cast<uint8_t>(b);


			input >> PlayerSpecs.OutlineThickness;
			input >> PlayerSpecs.ShapeVertices;
		}
		else if (identifier == "Enemy") {
			input >> EnemySpecs.ShapeRadius;
			std::cout << "Shape radius: " << EnemySpecs.ShapeRadius << "\n";
		
			input >> EnemySpecs.CollisionRadius;
			std::cout << "Collision radius: " << EnemySpecs.CollisionRadius << "\n";
		
			input >> EnemySpecs.Speed.Min;
			std::cout << "Minimum speed: " << EnemySpecs.Speed.Min << "\n";
		
			input >> EnemySpecs.Speed.Max;
			std::cout << "Maximum speed: " << EnemySpecs.Speed.Max << "\n";
		
			// Read color values as int, then cast to uint8_t
			int r, g, b;
			input >> r;
			std::cout << "Outline color red: " << r << "\n";
					
			input >> g;
			std::cout << "Outline color green: " << g << "\n";
					
			input >> b;
			std::cout << "Outline color blue: " << b << "\n";
					
			EnemySpecs.OutlineColor.r = static_cast<uint8_t>(r);
			EnemySpecs.OutlineColor.g = static_cast<uint8_t>(g);
			EnemySpecs.OutlineColor.b = static_cast<uint8_t>(b);
		
			input >> EnemySpecs.OutlineThickness;
			std::cout << "Outline thickness: " << EnemySpecs.OutlineThickness << "\n";
		
			input >> EnemySpecs.ShapeVertices.Min;
			std::cout << "Minimum shape vertices: " << EnemySpecs.ShapeVertices.Min << "\n";
		
			input >> EnemySpecs.ShapeVertices.Max;
			std::cout << "Maximum shape vertices: " << EnemySpecs.ShapeVertices.Max << "\n";
		
			input >> EnemySpecs.LifeSpan;
			std::cout << "Lifespan: " << EnemySpecs.LifeSpan << "\n";
		
			input >> EnemySpecs.SpawnInterval;
			std::cout << "Spawn interval: " << EnemySpecs.SpawnInterval << "\n";
		}
		else if (identifier == "Bullet") {
			input >> BulletSpecs.ShapeRadius;
			input >> BulletSpecs.CollisionRadius;
			input >> BulletSpecs.Speed;
			// Read color values as int, then cast to uint8_t
			int r, g, b;
			input >> r;
			input >> g;
			input >> b;
			BulletSpecs.FillColor.r = static_cast<uint8_t>(r);
			BulletSpecs.FillColor.g = static_cast<uint8_t>(g);
			BulletSpecs.FillColor.b = static_cast<uint8_t>(b);

			input >> r;
			input >> g;
			input >> b;
			BulletSpecs.OutlineColor.r = static_cast<uint8_t>(r);
			BulletSpecs.OutlineColor.g = static_cast<uint8_t>(g);
			BulletSpecs.OutlineColor.b = static_cast<uint8_t>(b);


			input >> BulletSpecs.OutlineThickness;
			input >> BulletSpecs.ShapeVertices;
			input >> BulletSpecs.LifeSpan;
		}


	}
	SpawnPlayer();
	std::cout << "spawn interval: " << EnemySpecs.SpawnInterval << "\n";
}

void Game::SpawnPlayer()
{
	//This goes slightly against the EntityManagers paradigm
	Player = EntityManager.AddEntity("Player");

	sf::Vector2f middleOfWindow{ Window.getSize().x / 2.0f, Window.getSize().y / 2.0f };

	Player->cTransform = std::make_shared<CTransform>(middleOfWindow, sf::Vector2f(0.0f, 0.0f), 0.0f);
	Player->cShape = std::make_shared<CShape>(PlayerSpecs.ShapeRadius, PlayerSpecs.ShapeVertices,
	PlayerSpecs.FillColor, PlayerSpecs.OutlineColor, PlayerSpecs.OutlineThickness);
	Player->cCollision = std::make_shared<CCollision>(PlayerSpecs.CollisionRadius);
	Player->cInput = std::make_shared<CInput>();
	Player->cSpecialShoot = std::make_shared<CSpecialShoot>(40, 60 * 10);
}

void Game::SpawnEnemy()
{
	std::srand((unsigned)std::time(NULL) * GameFrameCount);

	auto enemy = EntityManager.AddEntity("Enemy");

	auto radius = EnemySpecs.ShapeRadius;

	float minX = 0 + radius + 5;
	float maxX = Window.getSize().x - radius - 5;

	float minY = 0 + radius + 5;
	float maxY = Window.getSize().y - radius - 5;

	auto shapeX = minX + (std::rand() % (int)(1 + maxX - minX));
	auto shapeY = minY + (std::rand() % (int)(1 + maxY - minY));

	sf::Vector2f shapePos{ shapeX, shapeY };

	float angle = 0 + (std::rand() % (1 + (360 - 0)));
	angle = angle * 3.14f / 180.0f;

	auto minSpeed = EnemySpecs.Speed.Min;
	auto maxSpeed = EnemySpecs.Speed.Max;
	
	auto speed = minSpeed + (std::rand() % (1 + int(maxSpeed - minSpeed)));
	// To prevent low speed
	//auto signSpeed = 1;
	//if (speed != 0) {
	//	signSpeed = speed / std::fabs(speed);
	//}
	
	speed = speed + 0.5f; //* signSpeed;

	auto velX = std::cosf(angle) * speed;
	auto velY = std::sinf(angle) * speed;


	sf::Vector2f shapeVel{ velX,  velY };

	enemy->cTransform = std::make_shared<CTransform>(shapePos, shapeVel, angle);
	enemy->cCollision = std::make_shared<CCollision>(EnemySpecs.CollisionRadius);
	enemy->cLifeSpan = std::make_shared<CLifeSpan>(EnemySpecs.LifeSpan);

	auto minVertices = EnemySpecs.ShapeVertices.Min;
	auto maxVertices = EnemySpecs.ShapeVertices.Max;
	auto vertices = minVertices + (std::rand() % (1 + maxVertices - minVertices));

	uint8_t minColor = 0;
	uint8_t maxColor = 255;
	uint8_t r = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t g = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t b = minColor + (std::rand() % (1 + maxColor - minColor));

	auto fillColor = sf::Color(r, g, b);

	enemy->cShape = std::make_shared<CShape>(EnemySpecs.ShapeRadius, vertices, fillColor,
		EnemySpecs.OutlineColor, EnemySpecs.OutlineThickness);

	enemy->cScore = std::make_shared<CScore>(vertices * 100);

	std::cout << "x : " << shapeX << ", y : " << shapeY << ", angle : " << angle << std::endl;
}

void Game::SpawnBullet(const float angle)
{
	auto bullet = EntityManager.AddEntity("Bullet");
	bullet->cShape = std::make_shared<CShape>(BulletSpecs.ShapeRadius, BulletSpecs.ShapeVertices,
		BulletSpecs.FillColor, BulletSpecs.OutlineColor, BulletSpecs.OutlineThickness);
	bullet->cCollision = std::make_shared<CCollision>(BulletSpecs.CollisionRadius);
	bullet->cLifeSpan = std::make_shared<CLifeSpan>(BulletSpecs.LifeSpan);

	auto velX = std::cosf(angle) * BulletSpecs.Speed;
	auto velY = std::sinf(angle) * BulletSpecs.Speed;
	sf::Vector2f shapeVel{ velX,  velY };
	bullet->cTransform = std::make_shared<CTransform>(Player->cTransform->Position, shapeVel, angle);

}

void Game::SpawnSmallEnemy(std::shared_ptr<EntityCls> deadEnemy)
{
	if (deadEnemy->GetTag() == "Enemy") {
		auto count = deadEnemy->cShape->Shape.getPointCount();
		for (size_t i = 0; i < count; i++)
		{
			auto smallEnemy = EntityManager.AddEntity("SmallEnemy");

			auto angle = (360.0f / count) * i * 3.14f / 180.0f;
			auto speed = std::sqrtf(std::powf(deadEnemy->cTransform->Velocity.x, 2) + std::powf(deadEnemy->cTransform->Velocity.y, 2));
			auto velX = std::cosf(angle) * speed;
			auto velY = -1 * std::sinf(angle) * speed;

			smallEnemy->cTransform = std::make_shared<CTransform>(deadEnemy->cTransform->Position, sf::Vector2f(velX, velY), angle);
			smallEnemy->cCollision = std::make_shared<CCollision>(deadEnemy->cCollision->Radius / 3.0f);
			smallEnemy->cLifeSpan = std::make_shared<CLifeSpan>(deadEnemy->cLifeSpan->TotalLife / 2.0f);

			smallEnemy->cShape = std::make_shared<CShape>(deadEnemy->cShape->Shape.getRadius() / 3.0f,
				deadEnemy->cShape->Shape.getPointCount(), deadEnemy->cShape->Shape.getFillColor(),
				deadEnemy->cShape->Shape.getOutlineColor(), deadEnemy->cShape->Shape.getOutlineThickness());

			smallEnemy->cScore = std::make_shared<CScore>(deadEnemy->cShape->Shape.getPointCount() * 100 * 2);
		}
	}

}

void Game::sMovement()
{
	if (Player->cInput->Down) {
		Player->cTransform->Velocity.y = PlayerSpecs.Speed;
	}
	else if (Player->cInput->Up) {
		Player->cTransform->Velocity.y = -PlayerSpecs.Speed;
	}
	else {
		Player->cTransform->Velocity.y = 0;
	}

	if (Player->cInput->Right) {
		Player->cTransform->Velocity.x = PlayerSpecs.Speed;
	}
	else if (Player->cInput->Left) {
		Player->cTransform->Velocity.x = -PlayerSpecs.Speed;
	}
	else {
		Player->cTransform->Velocity.x = 0;
	}

	for (auto& e : EntityManager.GetEntities()) {
		if (e->cTransform != nullptr)
		{
			e->cTransform->Position.x += e->cTransform->Velocity.x;
			e->cTransform->Position.y += e->cTransform->Velocity.y;

		}
	}
}

void Game::sRender()
{
	Window.clear();

	for (auto& e : EntityManager.GetEntities()) {
		if (e->cShape != nullptr)
		{
			e->cTransform->Angle += 1;

			e->cShape->Shape.setPosition(e->cTransform->Position);
			e->cShape->Shape.setRotation(sf::degrees(e->cTransform->Angle));
			Window.draw(e->cShape->Shape);
		}
	}
	Window.display();
}

void Game::sSpawner()
{
	if ((GameFrameCount % EnemySpecs.SpawnInterval) == 0) {
		SpawnEnemy();
		std::cout << "spawn interval at creation: " << EnemySpecs.SpawnInterval << "\n";
		std::cout << "life left at creation: " << EnemySpecs.LifeSpan << "\n";
	}

	//if (ManuelSpawnFlag) {
	//	SpawnEnemy();
	//}

	if (Player->cInput->Shoot) {
		Player->cInput->Shoot = false;
		auto playerX = Player->cTransform->Position.x;
		auto playerY = Player->cTransform->Position.y;

		auto diffX = Player->cInput->MousePos.x - playerX;
		auto diffY = Player->cInput->MousePos.y - playerY;

		auto angle = std::atan2f(diffY, diffX);

		SpawnBullet(angle);

	}

}

void Game::sCollision()
{
	auto windowX = Window.getSize().x;
	auto windowY = Window.getSize().y;

	auto entities = EntityManager.GetEntities();
	auto enemies = EntityManager.GetEntities("Enemy");
	auto bullets = EntityManager.GetEntities("Bullet");
	auto smallEnemies = EntityManager.GetEntities("SmallEnemy");

	if ((Player->cTransform->Position.x - Player->cCollision->Radius) < 0) {
		Player->cTransform->Position.x = Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.x + Player->cCollision->Radius) > windowX) {
		Player->cTransform->Position.x = windowX - Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.y - Player->cCollision->Radius) < 0) {
		Player->cTransform->Position.y += Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.y + Player->cCollision->Radius) > windowY) {
		Player->cTransform->Position.y -= Player->cCollision->Radius;
	}

	// check player - enemy collision


	for (auto& enemy : enemies) {
		if (enemy->cCollision != nullptr) {
			auto diffX = Player->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = Player->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;
			auto radiusPlayer = Player->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusPlayer + radiusEnemy), 2)) {
				enemy->Destroy();
				Player->Destroy();
				SpawnPlayer();
				GameScore = 0;
			}
		}
	}

	for (auto& enemy : smallEnemies) {
		if (enemy->cCollision != nullptr) {
			auto diffX = Player->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = Player->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;
			auto radiusPlayer = Player->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusPlayer + radiusEnemy), 2)) {
				enemy->Destroy();
				Player->Destroy();
				SpawnPlayer();
				GameScore = 0;
			}
		}
	}

	// check window bound collision
	for (auto& e : entities) {
		if (e->cCollision != nullptr) {
			auto pos = e->cTransform->Position;
			auto radius = e->cCollision->Radius;

			if ((pos.x - radius) < 0 || (pos.x + radius) > windowX) {
				e->cTransform->Velocity.x *= -1;
			}

			if ((pos.y - radius) < 0 || (pos.y + radius) > windowY) {
				e->cTransform->Velocity.y *= -1;
			}
		}
	}

	// check bullet-enemy collision


	for (auto& bullet : bullets) {
		auto radiusBullet = bullet->cCollision->Radius;
		for (auto& enemy : enemies) {
			auto diffX = bullet->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = bullet->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusBullet + radiusEnemy), 2)) {
				enemy->Destroy();
				SpawnSmallEnemy(enemy);
				bullet->Destroy();
				GameScore += enemy->cScore->Score;
			}
		}
		for (auto& enemy : smallEnemies) {
			auto diffX = bullet->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = bullet->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusBullet + radiusEnemy), 2)) {
				enemy->Destroy();
				bullet->Destroy();
				GameScore += enemy->cScore->Score;
			}
		}
	}
}

void Game::sUserInput()
{
	while (auto event = Window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			Window.close();
			continue;
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			using Scan = sf::Keyboard::Scancode;

			switch (keyPressed->scancode) {
			case Scan::Escape:
				Window.close();
				break;
			case Scan::W:
				Player->cInput->Up = true;
				break;
			case Scan::A:
				Player->cInput->Left = true;
				break;
			case Scan::S:
				Player->cInput->Down = true;
				break;
			case Scan::D:
				Player->cInput->Right = true;
				break;
			case Scan::P:
				IsRunning = !IsRunning;
				break;
			default:
				break;
			}
		}

		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			using Scan = sf::Keyboard::Scancode;

			switch (keyReleased->scancode) {
			case Scan::W:
				Player->cInput->Up = false;
				break;
			case Scan::A:
				Player->cInput->Left = false;
				break;
			case Scan::S:
				Player->cInput->Down = false;
				break;
			case Scan::D:
				Player->cInput->Right = false;
				break;
			default:
				break;
			}
		}

		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (!IsRunning) continue;

			switch (mousePressed->button) {
			case sf::Mouse::Button::Left:
				Player->cInput->Shoot = true;
				Player->cInput->MousePos = sf::Vector2f(
					static_cast<float>(mousePressed->position.x),
					static_cast<float>(mousePressed->position.y)
				);
				break;
			case sf::Mouse::Button::Right:
				Player->cInput->SpecialShoot = true;
				break;
			default:
				break;
			}
		}
	}
}

void Game::sLifeSpan()
{
	auto entities = EntityManager.GetEntities();
	for (auto entity : entities) {
		if (entity->cLifeSpan != nullptr) {
			entity->cLifeSpan->RemainingLife -= 1;


			if (entity->cLifeSpan->RemainingLife <= 0) {
				entity->Destroy();
			}
		}
	}
}

void Game::sSpecialShoot()
{
	if (Player->cInput->SpecialShoot) {
		if (Player->cSpecialShoot->RemainingCooldown == 0) {
			// spawn many bullet
			for (size_t i = 0; i < Player->cSpecialShoot->BulletAmount; i++)
			{
				float angle = (360.0f / Player->cSpecialShoot->BulletAmount) * i;
				SpawnBullet(angle);
			}

			// set up cooldown
			Player->cSpecialShoot->RemainingCooldown = Player->cSpecialShoot->Cooldown;
		}
		Player->cInput->SpecialShoot = false;
	}

	if (Player->cSpecialShoot->RemainingCooldown > 0) {
		Player->cSpecialShoot->RemainingCooldown--;
	}


}
