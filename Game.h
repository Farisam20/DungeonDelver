
#pragma once
#include "Header.h"

class Level;
class Delver;
class Slime;
class Enemy;
class GUI;
class State;

//Main wrapper class to operate game
class Game
{
private:
	//Setting variables
	sf::RenderWindow* window;
	std::stack<State*> states;
	sf::Font gameFont;
	sf::Cursor cursor;
	GUI* gui;

	//Frame tracking variables
	sf::Clock dtClock;
	float dt;

	//Textures
	sf::Texture* tileset;              //tiles
	sf::Texture* sprites;              //sprites
	sf::Vector2u tileSize;             //size of each texture in the sheets

	//Window values
	sf::Vector2f globalScale;
	sf::Vector2f center;               //center of window

	//Game objects
	Level *curLevel;				   //level layout
	Delver *delver;				       //main character
	std::vector<Enemy*> enemyList;     //enemy list

	//Enemy movement map
	int moveArr[HEIGHT][WIDTH];
	sf::Vector2i prevPosition;

	//SFX
	sf::Music menuMusic;
	sf::Music bgMusic;

	enum sfx {CHEST, STAIRS, S1, S2, LEVEL_COMPLETE, DELV_HIT, HEAL, DELV_DEAD, GAME_OVER};
	std::vector<sf::SoundBuffer> buffers;
	std::vector<sf::Sound> sounds;

	//Misc
	sf::Clock levelClock;
	sf::Clock stateClock;
	float lt;

	//Functions
	void initSettingVars();
	void initGameObjects();
	void initWindow();
	void initCursor();
	void reset();

public:
	//Construct/Destruct
	Game(sf::Texture*& ntileset, sf::Texture*& nsprites, sf::Vector2u nTileSize, sf::Vector2f nScale);
	~Game();

	//Getters
	bool running();
	int getTileSize() { return tileSize.x; }

	//Getters for test functions
	sf::Vector2f getLevelPos();
	std::vector<Enemy*> getEnemyList() { return enemyList; }
	void resetEnemyList();

	//Enemy Functions
	void addEnemy(Enemy* enemy);
	void randomlySpawn(int amount);
	void SpawnBoss();
	int getMoveIndex(sf::Vector2i pos);
	sf::Vector2i smallestIn3By3(sf::Vector2i pos);
	sf::Vector2i biggestIn3by3(sf::Vector2i pos);
	Enemy* spawnSpecific(int index);

	//Helpers
	void pausing();
	void upgradeStats(int choice);
	sf::Vector2i calcSpritePos(sf::Vector2f pos, sf::Vector2f levelPos);
	bool checkValidPosition(sf::Vector2i pos);
	bool checkValidSpawn(sf::Vector2i pos);
	sf::Vector2f convertToWorldPosition(sf::Vector2i pos);
	void updatePlayerMap();
	void moveLevelHit(sf::Vector2f move);
	void setLevel(sf::Vector2f movement);
	void initNewLevel();
	void fade(int mode);

	//Game updating each frame
	void updateDt();
	void updateDelver();
	void updateChest();
	void updateStairs();
	void updateEnemies();
	
	//Game-to-window updating
	void update();
	void render();
};