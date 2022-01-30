
#pragma once
#include "Test.hpp"

Test::Test()
{
	t1 = new sf::Texture();
	t2 = new sf::Texture();
	t1->loadFromFile("assets/textures/tileset.png");
	t2->loadFromFile("assets/textures/spriteSheet.png");

	sf::Vector2u scale(0, 0);
	sf::Vector2f size(0, 0);

	//Initialize test objects
	testGame = new Game(t1, t2, scale, size);
	testLevel = new Level(t1, scale, size);
	testDelver = new Delver(10, 1, 1, 1, 1, 1, scale, nullptr, nullptr, testLevel, testGame);
}

Test::~Test()
{
	resetList();
	delete testDelver;
	delete testLevel;
	delete testGame;
	delete t1;
	delete t2;
}


//Private function to reset testList
void Test::resetList()
{
	while (!testList.empty())
		testList.erase(testList.begin());
}


//Test functions

//Tests that walls are added properly
void Test::testGenerateWalls()
{
	//Test generateWalls()
	int wallCount = 0;     //wallcount at end should be perimeter: 2*WIDTH + 2*HEIGHT - 4;
	testLevel->generateWalls();
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < HEIGHT; j++)
			if (testLevel->getIndex(sf::Vector2i(i, j)) == wall)
				wallCount++;

	//Report on generateWalls()
	if (wallCount == 2 * WIDTH + 2 * HEIGHT - 4)
		std::cout << "Walls generated correctly.\n";
	else
		std::cout << "Walls generated incorrectly.\n";
}

//Simulates a movement and checks if everything else is adjusted accordingly
void Test::testMoveLevelHit()
{
	//Test moveLevelHit()
	resetList();
	testGame->resetEnemyList();
	testGame->spawnSpecific(0);                                                         //spawn a slime to check movement of enemies
	testList = testGame->getEnemyList();											    //get initial enemy list
	sf::Vector2f initPos[2] = { testGame->getLevelPos(), testList[0]->getPosition() };  //load positions of level and slime
	testGame->moveLevelHit(sf::Vector2f(1, 1));                                         //simulate delver being hit in pos x,y direction
	testList = testGame->getEnemyList();                                                //reload list
	sf::Vector2f finalPos[2] = { testGame->getLevelPos(), testList[0]->getPosition() }; //get final positions of level and slime

	//Report on moveLevelHit()
	bool checkLevel = (initPos[0] == finalPos[0] - sf::Vector2f(1, 1)); //check level moved properly
	bool checkEnemy = (initPos[1] == finalPos[1] - sf::Vector2f(1, 1)); //check enemy moved properly
	if (checkLevel && checkEnemy)
		std::cout << "Sprites moved correctly.\n";
	else
		std::cout << "Sprites moved incorrectly.\n";
}

//Checks if one specific enemy was spawned
void Test::testSpawnSpecific()
{
	//Test spawnSpecific()
	resetList();
	testGame->resetEnemyList();
	testList = testGame->getEnemyList();
	int initSize = testList.size();         //check size (should be 0)
	testGame->spawnSpecific(0);             //spawn a slime
	testList = testGame->getEnemyList();    //reload the list
	int finalSize = testList.size();        //check size (should be 1)

	//Report on spawnSpecific()
	bool correctEnemy = (dynamic_cast<Slime*>(testList[0]) != nullptr); //check to make sure correct type of enemy was spawned
	if (finalSize - initSize == 1 && correctEnemy)
		std::cout << "Specific enemy spawned correctly.\n";
	else
		std::cout << "Specific enemy not spawned correctly.\n";
}

//Checks if multiple enemies are spawned
void Test::testRandomlySpawn()
{
	//Test randomlySpawn()
	resetList();
	testGame->resetEnemyList();
	testList = testGame->getEnemyList();    //get the initial enemy list
	int initSize = testList.size();         //check size (should be 0)
	testGame->randomlySpawn(3);             //spawn 3 random enemies
	testList = testGame->getEnemyList();    //reload the list
	int finalSize = testList.size();        //check size (should be 3)

	//Report on randomlySpawn()
	if (finalSize - initSize == 3)
		std::cout << "Randomly spawned 3 enemies.\n";
	else
		std::cout << "Could not randomly spawn 3 enemies.\n";
}

//Checks if delver takes damage correctly
void Test::testDelverGetHit()
{
	//Test getHit()
	testDelver->getHit(sf::Vector2f(0, 0), 1, 1, 5);

	//Report on getHit()
	bool hpCorrect = (testDelver->getHealth() == 5);
	bool iFramesCorrect = (testDelver->getInvuln() == 50);
	if (hpCorrect && iFramesCorrect)
		std::cout << "Delver hit update works.\n";
	else
		std::cout << "Delver hit update does not work.\n";
}