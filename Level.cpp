
#pragma once
#include "Level.h"

Level::Level(sf::Texture*& ntileset, sf::Vector2u nTileSize, sf::Vector2f nScale)
{
	//Seed the rng
	srand(time(NULL));

	//Initialize vars
	floor = 0;
	tileset = ntileset;
	tileSize = nTileSize;
	globalScale = nScale;
	promptRange = 1.5 * tileSize.x * globalScale.x;
	intKey = sf::Keyboard::E;

	//Load layout with generic level
	genericLevel();

	//Chest sprite
	chestSprite.setTexture(*tileset);
	chestSprite.setOrigin(tileSize.x / 2, tileSize.y / 2);
	chestSprite.setScale(globalScale);

	//Stair sprite
	stairSprite.setTexture(*tileset);
	stairSprite.setTextureRect(sf::IntRect(80, 32, 16, 16));
	stairSprite.setOrigin(tileSize.x / 2, tileSize.y / 2);
	stairSprite.setScale(globalScale);

	//Chest prompt
	chestPrompt.setTexture(*tileset);
	chestPrompt.setTextureRect(sf::IntRect(80, 64, 16, 16));
	chestPrompt.setOrigin(tileSize.x / 2, tileSize.y / 2);
	chestPrompt.setScale(globalScale);

	//Stair prompt
	stairPrompt.setTexture(*tileset);
	stairPrompt.setTextureRect(sf::IntRect(80, 64, 16, 16));
	stairPrompt.setOrigin(tileSize.x / 2, tileSize.y / 2);
	stairPrompt.setScale(globalScale);
}

//Generates generic level with outer walls, no chest, and a centralized staircase. Used as first level and boss arena (every 10 levels)
void Level::genericLevel()
{
	floor++;
	if (floor > 1)
	{
		level.clear();
		edges.clear();
	}
	generateWalls();
	defineTops();
	layout[5][WIDTH / 2] = stairs;
	startPos = sf::Vector2f((WIDTH / 2) * tileSize.x + tileSize.x / 2, (HEIGHT - 3) * tileSize.y + tileSize.y / 2);
	startPosArr = sf::Vector2i(WIDTH / 2, HEIGHT - 3);
	placeSprites();
}

//Generates entire dungeon level
void Level::generateLevel()
{
	//Clear the vertexArrays as each level progresses
	floor++;
	if (floor > 1)
	{
		level.clear();
		edges.clear();
	}

	//Generate the level
	generateTiles();
	addSpecials();
	placeSprites();
}

//Randomly generates a dungeon room (only walls + empty tiles)
void Level::generateTiles()
{
	//Make border
	generateWalls();

	//Continue generating levels until there is only one connected island of floor tiles
	int islands = 0;
	while (islands != 1)
	{
		//Generate random rectangles to fill level
		clear();
		int shape, xSize, ySize, x = WIDTH / 2, y = HEIGHT / 2, xMod, yMod, filled = 0;
		int fillCounts[4] = { 0,0,0,0 };
		int area = WIDTH * HEIGHT;
		while (filled < area * .25)
		{
			//Random side length
			xSize = (rand() % 2) + 2;
			ySize = (rand() % 1) + 3;

			//Random draw direction 
			xMod = rand() % 2 == 0 ? 1 : -1;
			yMod = rand() % 2 == 0 ? 1 : -1;

			//Random starter corner coordinate
			//Pick coordinate based on least populated quadrant
			int xMin, yMin;
			int index = getMinIndex(fillCounts, 4);
			switch (index)
			{
			case 0:
				xMin = 1; yMin = 1;
				break;
			case 1:
				xMin = WIDTH / 2; yMin = 1;
				break;
			case 2:
				xMin = WIDTH / 2; yMin = HEIGHT / 2;
				break;
			case 3:
				xMin = 1; yMin = HEIGHT / 2;
				break;
			}

			x = rand() % (WIDTH / 2 - 1) + xMin;
			y = rand() % (HEIGHT / 2 - 1) + yMin;
			x *= xMod;
			y *= yMod;

			//Update layout
			for (int j = y; (j != y + ySize) && (j != y - ySize); j += yMod)
				for (int i = x; (i != x + xSize) && (i != x - xSize); i += xMod)
					if (j < HEIGHT && i < WIDTH && j > 0 && i > 0)
					{
						layout[j][i] = 1;
						filled++;
						if (i < WIDTH / 2)
							j < HEIGHT / 2 ? fillCounts[0]++ : fillCounts[3]++;
						else
							j < HEIGHT / 2 ? fillCounts[1]++ : fillCounts[2]++;
					}
		}

		//Clean up small holes
		for (int i = 1; i < WIDTH - 1; i++)
			for (int j = 1; j < HEIGHT - 1; j++)
				fillHoles(i, j);

		//Clean diagonals and make sure there aren't unconnected floor pockets
		fillDiagonals();
		islands = countIslands();
	}
}

//Goes back through layout adding things that aren't walls or floor tiles
void Level::addSpecials()
{
	defineTops();

	//Add staircase near top
	int iters = 0;
	int x = rand() % (WIDTH / 2) + 1;
	int y = rand() % 2 + 1;
	while (1)
	{
		if (layout[y][x] == empty)
		{
			layout[y][x] = stairs;
			break;
		}

		x++; iters++;
		if (x >= WIDTH - 1)
			x = 1;
		if (iters > WIDTH - 1)
		{
			y++;
			iters = 0;
		}
	}

	//Spawn point near bottom
	iters = 0;
	x = (rand() % (WIDTH - 1)) + 1;
	y = HEIGHT - (rand() % 2 + 1);
	while (1)
	{
		if (layout[y][x] == empty)
		{
			//Define starting level transform
			startPos = sf::Vector2f(x * tileSize.x + tileSize.x / 2, y * tileSize.y + tileSize.y / 2);
			startPosArr = sf::Vector2i(x, y);
			break;
		}

		//Change (x,y) if needed
		x++; iters++;
		if (x >= WIDTH)
			x = 1;
		if (iters > WIDTH)
		{
			y--;
			iters = 0;
		}
	}

	//Spawn a chest in a corner
	bool placed = false;
	chestSprite.setTextureRect(sf::IntRect(80, 48, 16, 16));
	iters = 0;
	while (!placed && iters <= 5)
	{
		int xStart = rand() % (WIDTH - 2) + 1; 
		int yStart = rand() % (HEIGHT - HEIGHT / 2) + 3;
		for (int i = xStart; i < WIDTH - 2; i++)
			for (int j = yStart; j < HEIGHT - HEIGHT / 2; j++)
			{
				if (placed)
					break;
				if (layout[j][i] == empty)
				{
					//Count number of neighbors
					int neighbors = 0;
					bool left = false;
					if (layout[j - 1][i] == wall || layout[j - 1][i] == top)
						neighbors++;
					if (layout[j][i - 1] == wall || layout[j][i - 1] == top)
					{
						neighbors++;
						left = true;
					}
					if (!left && (layout[j][i + 1] == wall || layout[j][i + 1] == top))
						neighbors++;

					//Place chest if the current spot is a corner
					if (neighbors >= 2)
					{
						layout[j][i] = chest;
						chestOpen = false;
						placed = true;
					}
				}
			}
		iters++;
	}
}

//Places sprites of the chair/stair for player to interact with
void Level::placeSprites()
{
	bool chestCheck = false;
	int x; int y;
	for (int i = 1; i < WIDTH - 1; i++)
		for (int j = 1; j < HEIGHT - 1; j++)
			if (layout[j][i] == stairs || layout[j][i] == chest)
			{
				//Coordinates defining the position from top left of level
				x = (i * tileSize.x * globalScale.x) + (tileSize.x / 2 * globalScale.x);
				y = (j * tileSize.y * globalScale.y) + (tileSize.y / 2 * globalScale.y);
				if (layout[j][i] == stairs)
				{
					stairSprite.setPosition(x, y);
					stairPrompt.setPosition(x, y - 1.25 * tileSize.y * globalScale.y);
				}
				else
				{
					chestCheck = true;
					chestSprite.setPosition(x, y);
					chestPrompt.setPosition(x, y - 1.25 * tileSize.y * globalScale.y);
				}
			}
	if (!chestCheck)
		chestSprite.setPosition(sf::Vector2f(3 * WIDTH * tileSize.x * globalScale.x, 3 * HEIGHT * tileSize.y * globalScale.y)); //send chest far, far away
}

//Generates border of level
void Level::generateWalls()
{
	for (int j = 0; j < HEIGHT; j++)
		for (int i = 0; i < WIDTH; i++)
			if (i == 0 || j == 0 || i == WIDTH - 1 || j == HEIGHT - 1)
				layout[j][i] = wall;
			else
				layout[j][i] = empty;
}

//Loops through and makes walls above other walls into tops
void Level::defineTops()
{
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < HEIGHT; j++)
			if (layout[j][i] == wall && j < HEIGHT - 1 && (layout[j + 1][i] == wall || layout[j + 1][i] == top) || j == HEIGHT - 1)
				layout[j][i] = top;
	fillTops();
}



//Island counting functions
int Level::countIslands()
{
	resetVisited();
	int islands = 0;
	for (int i = 1; i < WIDTH - 1; i++)
		for (int j = 1; j < HEIGHT - 1; j++)
			if (layout[j][i] == empty && visited[j][i] == empty)
			{
				islands++;
				search(i, j);
			}
	return islands;
}
void Level::search(int i, int j)
{
	visited[j][i] = 1;

	//Up
	if (j > 1 && layout[j - 1][i] == 0 && visited[j - 1][i] == 0)
		search(i, j - 1);

	//Down
	if (j < HEIGHT - 1 && layout[j + 1][i] == 0 && visited[j + 1][i] == 0)
		search(i, j + 1);

	//Left
	if (i > 1 && layout[j][i - 1] == 0 && visited[j][i - 1] == 0)
		search(i - 1, j);

	//Right
	if (i < WIDTH - 1 && layout[j][i + 1] == 0 && visited[j][i + 1] == 0)
		search(i + 1, j);
}
void Level::resetVisited()
{
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < HEIGHT; j++)
			visited[j][i] = 0;
}

//Fills empty spaces surrounded by 3+ walls
void Level::fillHoles(int i, int j)
{
	if (layout[j][i] == 0)
	{
		int neighbors = 0;
		int xOff = 0, yOff = 0;

		//up
		if (layout[j - 1][i] == 1)
			neighbors++;
		else
			yOff--;

		//down
		if (layout[j + 1][i] == 1)
			neighbors++;
		else
			yOff++;

		//left
		if (layout[j][i - 1] == 1)
			neighbors++;
		else
			xOff--;

		//right
		if (layout[j][i + 1] == 1)
			neighbors++;
		else
			xOff++;

		if (neighbors >= 3)
			layout[j][i] = 1;
		if (neighbors == 3)
			fillHoles(i + xOff, j + yOff);
	}
}

//Fills in space between two purely diagonal tops
void Level::fillDiagonals()
{
	for (int i = 1; i < WIDTH - 2; i++)
		for (int j = 2; j < HEIGHT - 1; j++)
		{
			if (layout[j][i] == wall)
			{
				//iterating downwards, so only need to check down left or down right
				//down left
				if (layout[j + 1][i - 1] == wall && layout[j][i - 1] != wall && layout[j + 1][i] != wall)
					layout[j][i - 1] = wall;
				//down right
				if (layout[j + 1][i + 1] == wall && layout[j][i + 1] != wall && layout[j + 1][i] != wall)
					layout[j][i + 1] = wall;
			}
		}
}

//Fills in space between two purely diagonal tops
void Level::fillTops()
{
	for (int i = 1; i < WIDTH - 2; i++)
		for (int j = 2; j < HEIGHT - 1; j++)
		{
			if (layout[j][i] == top)
			{
				//iterating downwards, so only need to check down left or down right
				//down left
				if (layout[j + 1][i - 1] == top && layout[j][i - 1] != top && layout[j + 1][i] != top)
					layout[j][i - 1] = top;
				//down right
				if (layout[j + 1][i + 1] == top && layout[j][i + 1] != top && layout[j + 1][i] != top)
					layout[j][i + 1] = top;
			}
		}
}

//Reset layout
void Level::clear()
{
	for (int i = 1; i < WIDTH-1; i++)
		for (int j = 1; j < HEIGHT-1; j++)
			layout[j][i] = 0;
}




//Loads the level's vertexArray with textures from the tileset
void Level::loadTiles()
{
	level.setPrimitiveType(sf::Quads); //define level vertexarray of tiles (quads)
	level.resize(WIDTH * HEIGHT * 4);  //adjust size to fit level's elements

	//Iterate through layout and define positions + textures for each tile
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < HEIGHT; j++)
		{
			//Get texture vector for current tile
			int tileNumber = layout[j][i];
			int x = 0, y = 0;
			switch (tileNumber)
			{
			case chest:
			case stairs:
			case empty:
				x = (rand() % 4) * tileSize.x; //pick random floor offset along x-axis
				y = (rand() % 4) * tileSize.y; //pick random orientation
				break;

				/*case chest:
					x = 80;
					y = 48;
					break;*/

			case wall:
				x = 64;
				y = (rand() % 3) * tileSize.y;
				break;

			case top:
				x = 64;
				y = 64;
				break;
			}

			//Store tile in vertexarray
			int index = (i + j * HEIGHT);
			storeQuad(0, index, i, j, x, y, 0, 0);
		}
}

//Creates an edge map to draw over walls for 3D effect
void Level::loadEdges()
{
	edges.setPrimitiveType(sf::Quads); //define level vertexarray of tiles (quads)
	edges.resize((2 * HEIGHT + 2 * WIDTH) * 16);

	float x = 0, y = 0;       //tileset location for corresponding texture
	float xOff = 0, yOff = 0; //slight location offsets to better place edge textures
	int index = 0;

	//Top lip
	for (int i = 0; i < WIDTH; i++)
	{
		if (layout[0][i] == wall)
		{
			x = 48.f;
			y = 64.f;
		}
		else
		{
			x = 96.f;
			y = 64.f;
		}

		storeQuad(1, index, i, 0, x, y, 0, -(float)tileSize.y + 3);

		//Pointer to current tile's quad in the vertexArray
		sf::Vertex* quad = &edges[index * 4];
		index++;
	}

	//Rest of level layout (1st pass for top and bottom edges, 2nd for left/right edges)
	for (int n = 0; n < 2; n++)
		for (int i = 0; i < WIDTH; i++)
			for (int j = 0; j < HEIGHT; j++)
			{
				//Only look at tiles that are tops and draw an edge around them
				int flag = 0;
				if (layout[j][i] == top)
				{
					if (n == 0)
					{
						//Front
						if (j < HEIGHT - 1 && layout[j + 1][i] == wall)
						{
							x = 48; xOff = 0;
							y = 64; yOff = 4;
							flag++;
						}
						if (flag)
						{
							storeQuad(1, index, i, j, x, y, xOff, yOff);
							index++;
							flag = 0;
						}

						//Back
						if (j > 0 && layout[j - 1][i] != top)
						{
							x = 32; xOff = 0;
							y = 64; yOff = -12;
							flag++;
						}
						if (flag)
						{
							storeQuad(1, index, i, j, x, y, xOff, yOff);
							index++;
							flag = 0;
						}
					}

					else if (n == 1)
					{
						//Left
						if (layout[j][i - 1] != top && (i > 0 && i < WIDTH - 1))
						{
							x = 0; xOff = -3;
							y = 64; yOff = 0;
							flag++;
						}
						if (flag)
						{
							storeQuad(1, index, i, j, x, y, xOff, yOff);
							index++;
							flag = 0;
						}

						//Right
						if (layout[j][i + 1] != top && (i > 0 && i < WIDTH - 1))
						{
							x = 16; xOff = 3;
							y = 64; yOff = 0;
							flag++;
						}
						if (flag)
						{
							storeQuad(1, index, i, j, x, y, xOff, yOff);
							index++;
						}

						//Far left
						if (i == 0 && layout[j][i + 1] != top)
						{
							x = 32; xOff = 0;
							y = 80; yOff = 0;
							flag++;
						}

						//Far right
						else if (i == WIDTH - 1 && layout[j][i - 1] != top)
						{
							x = 48; xOff = 0;
							y = 80; yOff = 0;
							flag++;
						}
						if (flag)
						{
							storeQuad(1, index, i, j, x, y, xOff, yOff);
							index++;
							flag = 0;
						}
					}
				}
			}
}

//Stores a quad's position/texture at index of desired vertexArray (0 = tiles, else = edges)
void Level::storeQuad(int mode, int index, int i, int j, float x, float y, float xOff, float yOff)
{
	//Pointer to current tile's quad in the vertexArray
	sf::Vertex* quad;
	if (mode == 0)
		quad = &level[index * 4];
	else
		quad = &edges[index * 4];

	//Define position
	quad[0].position = sf::Vector2f(i * tileSize.x + xOff, j * tileSize.y + yOff);               //top left
	quad[1].position = sf::Vector2f((i + 1) * tileSize.x + xOff, j * tileSize.y + yOff);         //top right
	quad[2].position = sf::Vector2f((i + 1) * tileSize.x + xOff, (j + 1) * tileSize.y + yOff);   //bottom right
	quad[3].position = sf::Vector2f(i * tileSize.x + xOff, (j + 1) * tileSize.y + yOff);         //bottom left

	//Define texture
	quad[0].texCoords = sf::Vector2f(x, y);
	quad[1].texCoords = sf::Vector2f(x + tileSize.x, y);
	quad[2].texCoords = sf::Vector2f(x + tileSize.x, y + tileSize.y);
	quad[3].texCoords = sf::Vector2f(x, y + tileSize.y);
}



//Interactable stuff//

void Level::moveAll(sf::Vector2f movement)
{
	//Move entire level
	this->move(movement);

	//Move sprites
	chestSprite.move(movement);
	stairSprite.move(movement);
	chestPrompt.move(movement);
	stairPrompt.move(movement);
}

bool Level::promptingChest(sf::Vector2f playerPos)
{
	//Calculate distance between chest and player
	sf::Vector2f vec;
	vec.x = (chestSprite.getPosition().x) - tileSize.x / 2 - (playerPos.x) - tileSize.x / 2;
	vec.y = (chestSprite.getPosition().y) - tileSize.y / 2 - (playerPos.y) - tileSize.y / 2;
	float distance = sqrt(vec.x * vec.x + vec.y * vec.y);

	//If in range, display prompt and return true
	if (distance <= promptRange && !chestOpen)
	{
		if (chestPrompt.getColor() == sf::Color::Transparent)
			chestPrompt.setColor(sf::Color::White);
		return true;
	}
	//Else make sure prompt is off
	if (chestPrompt.getColor() != sf::Color::Transparent)
		chestPrompt.setColor(sf::Color::Transparent);
	return false;
}

bool Level::promptingStairs(sf::Vector2f playerPos, bool enemies)
{
	//Only show if all enemies are dead
	if (!enemies)
	{
		//Calculate distance between stairs and player
		sf::Vector2f vec;
		vec.x = (stairSprite.getPosition().x) - tileSize.x / 2 - (playerPos.x) - tileSize.x / 2;
		vec.y = (stairSprite.getPosition().y) - tileSize.y / 2 - (playerPos.y) - tileSize.y / 2;
		float distance = sqrt(vec.x * vec.x + vec.y * vec.y);

		//If in range, display prompt and return true
		if (distance <= promptRange)
		{
			if (stairPrompt.getColor() == sf::Color::Transparent)
				stairPrompt.setColor(sf::Color::White);
			return true;
		}
	}
	//Else make sure prompt is off
	if (stairPrompt.getColor() != sf::Color::Transparent)
		stairPrompt.setColor(sf::Color::Transparent);
	return false;
}

bool Level::interactingChest(sf::Vector2f playerPos)
{
	//If prompting and pressing corresponding key
	if (promptingChest(playerPos) && sf::Keyboard::isKeyPressed(intKey) && !chestOpen)
	{
		chestOpen = true;
		chestSprite.setTextureRect(sf::IntRect(96, 48, 16, 16)); //change to open chest sprite
		return true;
	}
	return false;
}

bool Level::interactingStairs(sf::Vector2f playerPos, bool enemies)
{
	//If prompting and pressing corresponding key
	if (promptingStairs(playerPos, enemies) && sf::Keyboard::isKeyPressed(intKey))
		return true;
	return false;
}

void Level::moveInteractables(sf::Vector2f movement)
{
	chestSprite.move(movement);
	stairSprite.move(movement);
	chestPrompt.move(movement);
	stairPrompt.move(movement);

	//Account for tilesize
	chestSprite.move(sf::Vector2f(-(float)(tileSize.x * globalScale.x) / 2, -(float)(tileSize.y * globalScale.y) / 2));
	stairSprite.move(sf::Vector2f(-(float)(tileSize.x * globalScale.x) / 2, -(float)(tileSize.y * globalScale.y) / 2));
	chestPrompt.move(sf::Vector2f(-(float)(tileSize.x * globalScale.x) / 2, -(float)(tileSize.y * globalScale.y) / 2));
	stairPrompt.move(sf::Vector2f(-(float)(tileSize.x * globalScale.x) / 2, -(float)(tileSize.y * globalScale.y) / 2));
}
