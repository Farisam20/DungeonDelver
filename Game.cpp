
#pragma once
#include "Game.h"

//Initializes window to be size of user's desktop
void Game::initWindow()
{
	//Window setup
	int width = sf::VideoMode::getDesktopMode().width;
	int height = sf::VideoMode::getDesktopMode().height;
	window = new sf::RenderWindow(sf::VideoMode(width, height), "Dungeon Delver", sf::Style::Titlebar | sf::Style::Close);
	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(1);
	window->setPosition(sf::Vector2i(-8, 0)); //weird offset to center screen, might just be local to my system
}

//Initializes fonts and sounds
void Game::initSettingVars()
{
	//Window base
	int x = sf::VideoMode::getDesktopMode().width / 2;
	int y = sf::VideoMode::getDesktopMode().height / 2;
	center = sf::Vector2f(x, y);

	//Font
	gameFont.loadFromFile("assets/fonts/arcade.ttf");

	//Music
	menuMusic.openFromFile("assets/sounds/music/menuMusic.wav");
	menuMusic.setVolume(25.f);
	menuMusic.setLoop(1);
	bgMusic.openFromFile("assets/sounds/music/dungeonMusic.wav");
	bgMusic.setVolume(5.f);
	bgMusic.setLoop(1);

	//SFX
	buffers.resize(9);
	sounds.resize(9);
	buffers[0].loadFromFile("assets/sounds/sfx/chest.ogg");
	buffers[1].loadFromFile("assets/sounds/sfx/stairs.wav");
	buffers[2].loadFromFile("assets/sounds/sfx/select1.wav");   //default select
	buffers[3].loadFromFile("assets/sounds/sfx/select2.wav");   //chest select
	buffers[4].loadFromFile("assets/sounds/sfx/levelOpen.ogg"); //all enemies dead
	buffers[5].loadFromFile("assets/sounds/sfx/delverHit.ogg");
	buffers[6].loadFromFile("assets/sounds/sfx/fullHeal.ogg");  //player healed
	buffers[7].loadFromFile("assets/sounds/sfx/delverDead.ogg");
	buffers[8].loadFromFile("assets/sounds/sfx/gameOver.ogg");

	//Assigns the sounds to their respective buffers
	for (int i = 0; i < buffers.size(); i++)
	{
		sounds[i].setBuffer(buffers[i]);
		if (i < 4 || i == 6)
			sounds[i].setVolume(10.f);
		else
			sounds[i].setVolume(25.f);
	}

	//volume normalization
	sounds[STAIRS].setVolume(40.f);
}

//Initializes all the game objects (player, enemies, level, etc)
void Game::initGameObjects()
{
	//Allocate
	curLevel = new Level(tileset, tileSize, globalScale);
	delver = new Delver(75, 7, 1, 55, 0, 6, (sf::Vector2u)globalScale, window, &enemyList, curLevel, this);
	gui = new GUI(center, tileset, &gameFont);

	//Load level's textures
	curLevel->placeSprites();
	curLevel->loadTiles();
	curLevel->loadEdges();
	curLevel->setScale(globalScale);

	//Center level about the starting position
	setLevel(curLevel->getStart());

	//Player
	delver->setOrigin(tileSize.x / 2, tileSize.y / 2);
	delver->setPosition(sf::Vector2f(center.x - (tileSize.x * globalScale.x) / 2, center.y - (tileSize.y * globalScale.y) / 2));
	delver->setTexture(*sprites);
	delver->setTextureRect(sf::IntRect(0, 0, 16, 16));
	delver->setScale(globalScale);
	prevPosition.x = -1;
	prevPosition.y = -1;

	//Spawn a slime
	spawnSpecific(0);

	//GUI
	gui->update(delver->getHealth(), curLevel->getFloor());
}

//Initializes custom cursor and other window cursor settings
void Game::initCursor()
{
	//Defining pixel array that represents the cursor
	sf::Image cursorImg;
	cursorImg.loadFromFile("assets/textures/cursor.png");
	const UINT8* reference = cursorImg.getPixelsPtr();

	//Setting system cursor to the pixel array
	window->setMouseCursorGrabbed(1);
	if (cursor.loadFromPixels(reference, sf::Vector2u(30, 30), sf::Vector2u(0,0)))
		window->setMouseCursor(cursor);
}

//Re-initializes game if player plays again after losing
void Game::reset()
{
	//Delete everything but window
	delete delver;
	delete curLevel;
	delete gui;

	while (!enemyList.empty())
		enemyList.erase(enemyList.begin());

	while (!states.empty())
	{
		delete states.top();
		states.pop();
	}

	//Re-initialize
	initSettingVars();
	initGameObjects();

	//Push the main menu state
	states.push(new menuState(window, center, &gameFont));
	menuMusic.play();
}


Game::Game(sf::Texture*& ntileset, sf::Texture*& nsprites, sf::Vector2u nTileSize, sf::Vector2f nScale)
{
	tileset = ntileset;
	sprites = nsprites;
	tileSize = nTileSize;
	globalScale = nScale;

	initWindow();
	initSettingVars();
	initGameObjects();
	initCursor();

	//Push the main menu state
	states.push(new menuState(window, center, &gameFont));
	menuMusic.play();
}

Game::~Game()
{
	bgMusic.stop();
	menuMusic.stop();

	delete window;
	delete delver;
	delete curLevel;
	delete gui;

	//Delete all the enemies
	while (!enemyList.empty())
		enemyList.erase(enemyList.begin());

	//Delete all the states
	while (!states.empty())
	{
		delete states.top();
		states.pop();
	}
}

bool Game::running()
{
	return window->isOpen();
}

sf::Vector2f Game::getLevelPos()
{
	return curLevel->getPosition();
}

void Game::resetEnemyList()
{
	while (!enemyList.empty())
		enemyList.erase(enemyList.begin());
}




//ENEMIES
Enemy* Game::spawnSpecific(int index)
{
	Enemy* s;
	int randX = -1;
	int randY = -1;

	//Checks until it finds a valid position to spawn an enemy
	while (!checkValidSpawn(sf::Vector2i(randX, randY)))
	{
		randX = (rand() % WIDTH - 2) + 1; // Random x [1, Width-1]
		randY = (rand() % HEIGHT - 2) + 1; // Random y [0, Height-1]
	}

	//Declare the enemy
	switch (index)
	{
	default:
	case 0:
		s = new Slime(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
		break;
	case 1:
		s = new Goblin(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
		break;
	case 2:
		s = new Eye(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
		break;
	}

	//Add to list
	enemyList.push_back(s);
	return s;
}
void Game::randomlySpawn(int amount)
{
	for (int i = 0; i < amount; ++i)
	{
		int randX = -1;
		int randY = -1;

		//Checks until it finds a valid position to spawn an enemy
		while (!checkValidSpawn(sf::Vector2i(randX, randY)))
		{
			randX = (rand() % WIDTH-2) + 1; // Random x [1, Width-1]
			randY = (rand() % HEIGHT-2) + 1; // Random y [0, Height-1]
		}

		//Declare the enemy
		Enemy* s;
		int randEnemy = rand() % 3;
		switch (randEnemy)
		{
		case 0:
			s = new Slime(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
			break;
		case 1:
			s = new Goblin(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
			break;
		case 2:
			s = new Eye(sf::Vector2f(curLevel->getPosition().x + (randX * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (randY * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
			break;
		}
		enemyList.push_back(s);
	}
}
void Game::SpawnBoss()
{
	//Place nyar in the middle of the arena
	Enemy* s = new Nyarlathotep(sf::Vector2f(curLevel->getPosition().x + (5 * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x), curLevel->getPosition().y + (WIDTH/2 * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y)), delver, this, curLevel, window);
	enemyList.push_back(s);
}
int Game::getMoveIndex(sf::Vector2i pos)
{
	//Get index of position in move array
	if (pos.x < WIDTH || pos.x > -1 || pos.y > -1 || pos.y < HEIGHT) 
		return moveArr[pos.y][pos.x];
	return 0;
}

//Find the biggest/smallest value in MoveArr in a 3x3 area around the given vector (used for enemy movement choices)
sf::Vector2i Game::biggestIn3by3(sf::Vector2i currentPos)
{
	int max = -1;
	sf::Vector2i retVector;
	for (int i = currentPos.y - 1; i <= currentPos.y + 1; ++i) // Iterate through all points that are within 1 space of current point
	{
		for (int a = currentPos.x - 1; a <= currentPos.x + 1; ++a)
		{
			if (i < HEIGHT && i > -1 && a > -1 && a < WIDTH)
			{
				if (a == currentPos.x || i == currentPos.y) // Only add a point if it is not diagonal
				{
					if (moveArr[i][a] > max && moveArr[i][a] != HEIGHT * WIDTH) // New maximum value
					{
						retVector.x = a;
						retVector.y = i;
						max = moveArr[i][a];
					}
				}
			}
		}
	}
	return retVector; // Return biggest array index
}
sf::Vector2i Game::smallestIn3By3(sf::Vector2i currentPos)
{
	int min = WIDTH * HEIGHT;
	sf::Vector2i retVector;
	for (int i = currentPos.y - 1; i <= currentPos.y + 1; ++i) // Iterate through all points that are within 1 space of current point
	{
		for (int a = currentPos.x - 1; a <= currentPos.x + 1; ++a)
		{
			if (i < HEIGHT && i > -1 && a > -1 && a < WIDTH)
			{
				if (currentPos.x == a || currentPos.y == i && ((currentPos.x != a || currentPos.y != i) || moveArr[i][a] == -1)) // Make sure point is not the current pointtt, and that it is not diagonal
				{
					if (moveArr[i][a] <= min)
					{
						retVector.x = a;
						retVector.y = i;
						min = moveArr[i][a];
					}
				}
			}
		}
	}
	return retVector; // Return min
}

//Add enemy to list, mainly used for projectitles
void Game::addEnemy(Enemy* enemy)
{
	enemyList.push_back(enemy);
}




//HELPERS
void Game::pausing()
{
	//Push a pause state if player presses esc from the game
	if (states.empty() && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		states.push(new pauseState(window, center, &gameFont));
}
void Game::upgradeStats(int choice)
{
	switch (choice)
	{
		//Health
	case 0:
		delver->healthUp();
		break;

		//Movespeed
	case 1:
		delver->speedUp();
		break;

		//Damage
	case 2:
		delver->dmgUp();
		break;
	}
}

//Returns position of sprite in terms of i and j for layout[][] checking
sf::Vector2i Game::calcSpritePos(sf::Vector2f pos, sf::Vector2f levelPos)
{
	//Calculate distance between bottom left of player and top left of level
	float xOff = (pos.x + tileSize.x / 2 * globalScale.x) - levelPos.x;
	float yOff = (pos.y + tileSize.y / 2 * globalScale.y) - levelPos.y;

	//Divide values by relevant scalars and round to get closest i and j
	xOff /= globalScale.x * tileSize.x;
	yOff /= globalScale.y * tileSize.y;
	xOff -= 1;
	yOff -= 1;

	//Return i and j as a vector
	return sf::Vector2i(round(xOff), round(yOff));
}

//Checks if new position calculated from calcSpritePos() is valid or not
bool Game::checkValidPosition(sf::Vector2i pos)
{
	return curLevel->getIndex(pos) == empty || curLevel->getIndex(pos) == stairs;
}

//Checks if random enemy spawn is valid
bool Game::checkValidSpawn(sf::Vector2i pos)
{
	//Check for valid tile
	bool checkTile = curLevel->getIndex(pos) == empty || curLevel->getIndex(pos) == stairs;

	//Check sufficiently far from player
	double distance = distanceBetween((sf::Vector2f)pos, (sf::Vector2f)curLevel->getStartArr());
	bool checkDistance = (distance > 5);

	return checkTile && checkDistance;
}

// Convert from vector to world position
sf::Vector2f Game::convertToWorldPosition(sf::Vector2i pos)
{
	return sf::Vector2f(curLevel->getPosition().x + (pos.x * tileSize.x * globalScale.x) + (tileSize.x / 2.f * globalScale.x), curLevel->getPosition().y + ((pos.y * tileSize.y * globalScale.y) + (tileSize.y / 2.f * globalScale.y)));
}

//Fill each cell in moveArr with its distance from the cell in moveArr that the player is in 
void Game::updatePlayerMap()
{
	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int a = 0; a < WIDTH; ++a)
		{
			if (curLevel->getIndex(sf::Vector2i(a, i)) != 0)
			{
				moveArr[i][a] = HEIGHT * WIDTH; // Set wall indexes to maximum value

			}
			else
			{
				moveArr[i][a] = 0; // Set moveable places to 0
			}
		}
	}
	if (sf::Vector2i(-1, -1) != prevPosition) // Removes the previous player position from moveArr
	{
		moveArr[prevPosition.y][prevPosition.x] = 0;
	}
	std::queue<sf::Vector3i> points;
	sf::Vector2i curPos = calcSpritePos(delver->getPosition(), curLevel->getPosition()); // Calculate current pos of delver
	moveArr[curPos.y][curPos.x] = -1; // Set the position of the delver to -1 in the moveArr
	prevPosition = curPos;
	points.push(sf::Vector3i(curPos.x, curPos.y, 0)); // Push the first point (delver pos) to the queue
	while (points.size() != 0)
	{
		sf::Vector3i currPoint = points.front(); // Pop the first point off the queue
		points.pop();
		if (moveArr[currPoint.y][currPoint.x] >= 0 || moveArr[currPoint.y][currPoint.x] == -2)
		{
			moveArr[currPoint.y][currPoint.x] = currPoint.z;// Change the value of the moveArr to the correct distance, if its not the delver pos
		}
		for (int i = currPoint.y - 1; i <= currPoint.y + 1; ++i) // Iterate through all points that are within 1 space of current point
		{
			for (int a = currPoint.x - 1; a <= currPoint.x + 1; ++a)
			{
				if (i < HEIGHT && i > -1 && a > -1 && a < WIDTH)
				{
					if (a == currPoint.x || i == currPoint.y)
					{
						if (moveArr[i][a] == 0) // If the new point is unvisited, add it to the stack
						{
							moveArr[i][a] = -2; // Prevent the point from being readded to the stack by changing it from 0
							points.push(sf::Vector3i(a, i, currPoint.z + 1));
						}
					}
				}
			}
		}
	}
}

//Sets level and all sprites to the starting position
void Game::setLevel(sf::Vector2f movement)
{
	//Level starting transform
	curLevel->setPosition(sf::Vector2f(center.x - movement.x, center.y - movement.y));
	curLevel->move(sf::Vector2f(-(float)(tileSize.x * globalScale.x) / 2, -(float)(tileSize.y * globalScale.y) / 2)); //account for tilesize

	//Sprites
	curLevel->moveInteractables(sf::Vector2f(center.x - movement.x, center.y - movement.y));
}

//Whenever the delver gets knocked back, move level + enemies
void Game::moveLevelHit(sf::Vector2f move)
{
	curLevel->moveAll(move);
	for (int i = 0; i < enemyList.size(); ++i)
	{
		enemyList[i]->move(move);
	}
}

//Stops gameplay to generate new level
void Game::initNewLevel()
{
	//Begin countdown
	levelClock.restart();

	//Generate level
	if ((curLevel->getFloor() + 1) % 10 != 0)
	{
		curLevel->generateLevel();       //new level layout
		setLevel(curLevel->getStart());  //move to proper spot
		randomlySpawn((rand() % 3 + curLevel->getFloor()/5) + 1);  //spawn some enemies
	}
	//Boss every 10 floors
	else
	{
		curLevel->genericLevel();
		setLevel(curLevel->getStart());  //move to proper spot
		SpawnBoss();
	}
	curLevel->loadTiles();           //reload tiles
	curLevel->loadEdges();           //reload edges

	//SFX
	sounds[STAIRS].play();
	Sleep(400);
	sounds[STAIRS].play();

	//End countdown and wait remainder
	lt = levelClock.restart().asMilliseconds();
	if (1000 - lt > 0)
		Sleep(1000 - lt);
}

//Adds fade to/from black effect to screen
void Game::fade(int mode)
{
	sf::RectangleShape filter;
	filter.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));

	//Fade out
	if (mode == 0)
		for (int i = 0; i <= 5; i++)
		{
			filter.setFillColor(sf::Color(0, 0, 0, i * 50));
			window->draw(filter, sf::BlendAlpha);
			window->display();
			Sleep(45);
		}

	//Fade in
	else
		for (int i = 5; i >= 0; i--)
		{
			//Draw level
			window->clear(sf::Color(53, 63, 80));
			window->draw(*curLevel);
			curLevel->drawSprites(*window);
			for (int i = 0; i < enemyList.size(); i++)
				window->draw(*enemyList[i]);
			window->draw(*delver);
			curLevel->drawEdges(*window);
			gui->render(window);

			//Draw filter
			filter.setFillColor(sf::Color(0, 0, 0, i * 50));
			window->draw(filter, sf::BlendAlpha);
			window->display();
			Sleep(45);
		}
}




//UPDATES

//Updates dt to the time it took to update + render one frame
void Game::updateDt()
{
	dt = dtClock.restart().asSeconds();
}

//Update handlers for game objects
void Game::updateDelver()
{
	//Movement + Collision
	float xSpeed = delver->getMoveSpeed() * globalScale.x * dt;
	float ySpeed = delver->getMoveSpeed() * globalScale.y * dt;
	int horiBuffer = 4.f * globalScale.x;
	int vertBuffer = 4.f * globalScale.y;
	bool moving = false;
	sf::Vector2f moveVec(0, 0);

	//Preemptively check if player's move is valid. If it is, move them in the desired direction:
	//Up
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && checkValidPosition(calcSpritePos(delver->getPosition(), curLevel->getPosition() + sf::Vector2f(0, ySpeed))))
	{
		moveVec += sf::Vector2f(0, ySpeed);
	}

	//Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && checkValidPosition(calcSpritePos(delver->getPosition(), curLevel->getPosition() + sf::Vector2f(xSpeed + horiBuffer, 0))))
	{
		delver->changeDirection(0);
		moveVec += sf::Vector2f(xSpeed, 0);
	}

	//Down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && checkValidPosition(calcSpritePos(delver->getPosition(), curLevel->getPosition() + sf::Vector2f(0, -ySpeed - vertBuffer))))
	{
		moveVec += sf::Vector2f(0, -ySpeed);
	}

	//Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && checkValidPosition(calcSpritePos(delver->getPosition(), curLevel->getPosition() + sf::Vector2f(-xSpeed - horiBuffer, 0))))
	{
		delver->changeDirection(1);
		moveVec += sf::Vector2f(-xSpeed, 0);
	}

	//Move sprites
	curLevel->moveAll(moveVec);
	for (int i = 0; i < enemyList.size(); i++)
		enemyList[i]->move(moveVec);

	//Animation
	if (moveVec != sf::Vector2f(0, 0))
		delver->setMoving(true);
	else
		delver->setMoving(false);
	delver->animation();

	//Attacking
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && stateClock.getElapsedTime().asMilliseconds() >= 300)
		delver->attack();

	//Check if hit
	delver->checkHit();
	delver->updateWeapon();
}
void Game::updateChest()
{
	if (curLevel->interactingChest(delver->getPosition()))
	{
		//Open chest sound
		sounds[CHEST].play();

		//Push an upgrade state
		states.push(new upgradeState(800, 300, tileset, window, center, &gameFont));
	}
}
void Game::updateStairs()
{
	//Only allow progression once all enemies are dead
	bool enemies = enemyList.size();
	if (curLevel->interactingStairs(delver->getPosition(), enemies))
	{
		//Fade out
		fade(0);

		//Making new level
		initNewLevel();
		gui->update(delver->getHealth(), curLevel->getFloor());

		//Fade in
		fade(1);

		//Heal the player a bit
		if (delver->getHealth() != delver->getMaxHP())
		{
			delver->heal(25);
			sounds[HEAL].play();
		}

		//Refresh dt to account for this process taking much longer than the average frame
		updateDt();
	}
}
void Game::updateEnemies()
{
	if (rand() % 5 == 3)
		updatePlayerMap();
	for (int i = 0; i < enemyList.size(); i++) // Update all enemies
	{
		if (enemyList[i]->getHealth() <= 0) // Delete dead enemies
		{
			Enemy* enemy = enemyList[i];

			//Heal player if they beat the boss
			if (dynamic_cast<Nyarlathotep*>(enemy) != nullptr)
			{
				delver->heal(delver->getMaxHP());
				sounds[HEAL].play();
			}
			enemyList.erase(enemyList.begin() + i);
		}
		else
		{
			if (enemyList[i]->isupdateable())
			{
				enemyList[i]->update();
				if (dynamic_cast<Goblin*>(enemyList[i]) != nullptr) // Returns true if enemy is goblin type
				{
					dynamic_cast<Goblin*>(enemyList[i])->getWeapon()->update(); // If goblin, update their sword
				}
			}
		}


	}
}



//Global update + render
void Game::update()
{
	//Check for esc press (pausing)
	pausing();

	//Update game objects if not in another state
	if (states.empty())
	{
		//Game over if player hp = 0;
		if (delver->getHealth() <= 0)
		{
			states.push(new overState(window, center, &gameFont));
			sounds[DELV_DEAD].play();
			//add a death sprite to the enemy vector
		}

		updateDelver();
		updateChest();   //pushes an upgrade state if applicable
		updateStairs();  //goes to next level if interacting with stairs
		updateEnemies();

		//Update GUI
		gui->update(delver->getHealth(), curLevel->getFloor());
	}

	//Else update the topmost state and not the game objects
	else
	{
		states.top()->update();

		//Check if player is exiting the state and handle accordingly
		if (states.top()->getQuit())
		{
			std::string ID = states.top()->getID();

			//Main menu + pause states
			if (ID == "pause" || ID == "menu")
			{
				//Exit game if player chose to, otherwise continue on
				if (states.top()->getNextState() == 1)
					window->close();
				sounds[S1].play();

				//Change music if going from menu to game
				if (ID == "menu")
				{
					menuMusic.stop();
					bgMusic.play();
					fade(1);
					updateDt();
				}
			}

			//Upgrade state
			else if (ID == "upgrade")
			{
				//upgrade stat the player chose
				upgradeStats(states.top()->getNextState());
				sounds[S2].play();
			}

			//Game over
			else if (ID == "over")
			{
				//Retry -> go back to main menu
				if (!states.top()->getNextState())
					reset();
				else
					window->close();
			}

			//Pop the state
			if (ID != "over")
			{
				delete states.top();
				states.pop();
				stateClock.restart().asMilliseconds();
			}
		}
	}

}
void Game::render()
{
	//Clear window to bg color
	window->clear(sf::Color(53, 63, 80));

	//If no extra states, then draw game objects
	if (states.empty())
	{
		//Draw game objects
		window->draw(*curLevel);                       //tiles
		curLevel->drawSprites(*window);                //interactable sprites
		window->draw(*delver);                         //player
		for (int i = 0; i < enemyList.size(); i++)     //enemies + enemy weapons
		{
			window->draw(*enemyList[i]);
			if (dynamic_cast<Goblin*>(enemyList[i]) != nullptr) // Returns true if enemy is goblin type
				dynamic_cast<Goblin*>(enemyList[i])->getWeapon()->render(); // If goblin, draw sword
		}
		delver->getWeapon()->render();				   //player's weapon
		curLevel->drawEdges(*window);                  //edges
		gui->render(window);                           //GUI
	}

	//Else draw the state
	else
		states.top()->render();

	//Update the window
	window->display();
}