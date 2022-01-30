
#pragma once
#include "Header.h"

class Game;
class Level;
class Delver;
class Enemy;

class Test
{
private:
	Game* testGame;
	Level* testLevel;
	Delver* testDelver;
	std::vector<Enemy*> testList;

	sf::Texture* t1;
	sf::Texture* t2;

	void resetList();

public:
	Test();
	~Test();

	//Test functions
	void testGenerateWalls();
	void testMoveLevelHit();
	void testSpawnSpecific();
	void testRandomlySpawn();
	void testDelverGetHit();
};