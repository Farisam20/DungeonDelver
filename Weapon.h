
#pragma once
#include "Header.h"

class Enemy;
class Delver;

//Weapon sprites
class Weapon
{
protected:
	int damage;
	int angle;
	int curAttackStage;
	int totalAttackStage;
	bool attacking;
	bool evil;

public:
	Weapon(Delver* player, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, bool evil, Enemy* enemy);
	virtual void attack() = 0;
	virtual void currentlyAttacking() = 0;
	virtual void update() = 0;
	bool isAttacking();
	sf::Sprite* getModel();

	//Pointer values
	Enemy* enemy;
	sf::RenderWindow* RW;
	sf::Sprite* model;
	std::vector<Enemy*>* enemyList;
	Delver* player;
	sf::Texture* texture;
};


//Sword subclass
class defaultSword : public Weapon
{
public:
	defaultSword(Delver* player, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, bool evil, Enemy* enemy);
	~defaultSword();
	void update();
	void render();
	void attack();
	void currentlyAttacking();
};