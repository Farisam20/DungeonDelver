
#pragma once
#include "Header.h"

//Enumeration for layout array
//top = wall above another wall
enum Tile { empty, wall, top, chest, stairs };

//Class that defines current dungeon level
class Level : public sf::Drawable, public sf::Transformable
{
private:
	//Level data
	int floor;                       //Current floor
	int layout[WIDTH][HEIGHT];       //Int array that defines the structure of the level
	int visited[WIDTH][HEIGHT];      //Int array for checking visited tiles (level generation adjustment)

	//Texture stuff
	sf::Texture* tileset;            //Tile textures
	sf::Vector2u tileSize;           //Defines tile size (in pixels) for finding textures in the tileset
	sf::Vector2f startPos;           //Vector defining starting position of the level relative to the character
	sf::Vector2i startPosArr;        //Vector defining starting position in terms of i and j
	sf::Vector2f globalScale;

	//Drawables
	sf::VertexArray level;           //Array of tiles (quads) resembling the level
	sf::VertexArray edges;           //Similar array that defines the wall edge textures (3D effect)

	//Interactables (chest + stairs)
	sf::Sprite chestSprite;          //Actual sprites
	sf::Sprite stairSprite;
	sf::Sprite chestPrompt;          //Prompt sprites
	sf::Sprite stairPrompt;
	float promptRange;               //Distance at which prompt will appear
	sf::Keyboard::Key intKey;        //Key that must be pressed to interact with the object
	bool chestOpen;

	//Overload of draw() function provided by sf::Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = tileset;
		target.draw(level, states);
	}

public:
	//Constructor
	Level(sf::Texture*& ntileset, sf::Vector2u nTileSize, sf::Vector2f nScale);

	//Generation functions
	void genericLevel();
	void generateLevel();
	void generateTiles();
	void addSpecials();
	void placeSprites();

	//Generation helpers
	void generateWalls();
	void defineTops();
	int countIslands();
	void search(int i, int j);
	void resetVisited();
	void fillHoles(int i, int j);
	void fillDiagonals();
	void fillTops();
	void clear();

	//VertexArray stuff
	void loadTiles();
	void loadEdges();
	void storeQuad(int mode, int index, int i, int j, float x, float y, float xOff, float yOff);

	//Separate draw functions for other level objects
	void drawEdges(sf::RenderTarget& target) const
	{
		sf::RenderStates states;
		states.transform *= getTransform();
		states.texture = tileset;
		target.draw(edges, states);

		//Draw prompts on top of everything else
		target.draw(chestPrompt);
		target.draw(stairPrompt);
	}
	void drawSprites(sf::RenderTarget& target) const
	{
		target.draw(chestSprite);
		target.draw(stairSprite);
	}

	//Moves all sprites in level
	void moveAll(sf::Vector2f movement);

	//Chest+stair functions
	bool promptingChest(sf::Vector2f playerPos);
	bool promptingStairs(sf::Vector2f playerPos, bool enemies);
	bool interactingChest(sf::Vector2f playerPos);
	bool interactingStairs(sf::Vector2f playerPos, bool enemies);
	void moveInteractables(sf::Vector2f movement);

	//Getters
	int getFloor() { return floor; }
	sf::Vector2f getStart() { return sf::Vector2f(startPos.x * getScale().x, startPos.y * getScale().y); }
	sf::Vector2i getStartArr() { return startPosArr; }
	sf::Vector2f getSize() { return sf::Vector2f(WIDTH * tileSize.x * getScale().x, HEIGHT * tileSize.y * getScale().y); }
	int getIndex(sf::Vector2i pos)
	{
		//OOB
		if (pos.x > WIDTH-1 || pos.x < 1 || pos.y > HEIGHT-1 || pos.y < 1)
			return 1;

		//Else give tile value at layout
		return layout[pos.y][pos.x];
	}
};
