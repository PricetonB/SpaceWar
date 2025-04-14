#pragma once
#include <cstdlib> 
#include <cmath>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"


struct MinMaxFStr {
	float Min;
	float Max;
};

struct MinMaxIStr {
	size_t Min;
	size_t Max;
};

struct PlayerSpecsStr {
	uint16_t ShapeRadius;
	uint16_t CollisionRadius;
	float Speed;
	sf::Color FillColor;
	sf::Color OutlineColor;
	uint16_t OutlineThickness;
	size_t ShapeVertices;

};

struct EnemySpecsStr {
	uint16_t ShapeRadius;
	uint16_t CollisionRadius;
	MinMaxFStr Speed;
	sf::Color OutlineColor;
	uint16_t OutlineThickness;
	MinMaxIStr ShapeVertices;
	uint16_t LifeSpan;
	uint16_t SpawnInterval;
};

struct BulletSpecsStr {
	uint16_t ShapeRadius;
	uint16_t CollisionRadius;
	float Speed;
	sf::Color FillColor;
	sf::Color OutlineColor;
	uint16_t OutlineThickness;
	size_t ShapeVertices;
	uint16_t LifeSpan;
};



class Game
{
private:
	sf::RenderWindow Window;
	EntityManagerCls EntityManager;

	PlayerSpecsStr PlayerSpecs;
	EnemySpecsStr EnemySpecs;
	BulletSpecsStr BulletSpecs;

	EntityPtr Player;

	size_t GameScore;

	size_t GameFrameCount;

	sf::Clock DeltaClock;

	uint16_t SpawnInterval;
	bool ManuelSpawnFlag;

	bool IsPaused;
	bool IsRunning;

	void Init(const std::string& configFile);

	void SpawnPlayer();
	void SpawnEnemy();
	void SpawnBullet(const float angle);
	void SpawnSmallEnemy(std::shared_ptr<EntityCls> deadEnemy);

	void sMovement();
	void sRender();
	void sSpawner();
	void sCollision();
	void sUserInput();
	void sLifeSpan();
	void sSpecialShoot();
	

public:
	Game(const std::string& configFile);

	void Run();
};


