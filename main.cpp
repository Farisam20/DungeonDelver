
#pragma once
#include "Header.h"

class Game;
class Test;

int main(void)
{
	//Global Textures
	sf::Texture* tileset = new sf::Texture();
	sf::Texture* sprites = new sf::Texture();
	tileset->loadFromFile("assets/textures/tileset.png");
	sprites->loadFromFile("assets/textures/spriteSheet.png");

	//Global Vectors
	sf::Vector2u tileSize(16, 16);
	sf::Vector2f globalScale(4, 4);

	//Tests
	/*
	Test *test = new Test();
	test->testGenerateWalls();
	test->testMoveLevelHit();
	test->testSpawnSpecific();
	test->testRandomlySpawn();
	test->testDelverGetHit();
	delete test;
	*/

	//Main game loop
	Game app(tileset, sprites, tileSize, globalScale);
	while (app.running())
	{
		app.updateDt();
		app.update();
		app.render();
	}

	//Clean up
	delete tileset;
	delete sprites;
	return 0;
}