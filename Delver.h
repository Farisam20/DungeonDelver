
#pragma once
#include "Header.h"

class Game;
class Weapon;
class defaultSword;
class Enemy;
class Level;

//Class defining the player character (Delver)
class Delver : public sf::Sprite
{
protected:
	//Stats
	float maxHP;
	float health;
	float damage;
	float atkSpeed;
	float moveSpeed;       //Target movespeed in pixels per second
	int iframes;
	int numSpeedUps;       //Number of times player has upgraded speed (diminishing returns)
	int numDmgUps;         //Number of times player has upgraded dmg (diminishing returns)

	//Animation stuff
	bool moving;           //0 = standing, 1 = moving
	bool direction;        //0 = left, 1 = right
	int animationNum;      //Current tile offset on spritesheet
	int animFrames;        //# of frames to wait before updating animation
	int framesWaited;      //# of frames spent in current animation frame
	sf::Vector2u tileSize; //defines tilesize of spritesheet (16x16)
	sf::Vector2u globalScale;

	//This makes me sad
	sf::RenderWindow* window;
	std::vector<Enemy*>* enemyList;
	Level* level;
	Game* game;

	//Attacking
	defaultSword* weapon;
	bool firstAttack;
	sf::Clock attackCooldown;
	sf::Vector2f hitPos;
	double knockBackDistance;
	int hitLength;

public:
	Delver(float nMaxHP, float nDamage, float nAtkSpeed, float nMoveSpeed, int nIframes, int nAnimFrames, sf::Vector2u scale, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, Level* nLevel, Game* nGame);

	//Getters
	float getHealth() { return health; }
	float getMaxHP() { return maxHP; }
	float getDamage() { return damage; }
	float getAtkSpeed() { return atkSpeed; }
	float getMoveSpeed() { return moveSpeed; }
	int getInvuln() { return iframes; }
	int getAnim() { return animationNum; }
	int getDirection() { return direction; }
	defaultSword* getWeapon() { return weapon; }

	//Setters
	void setInvuln(int nframes) { iframes = nframes; }
	void setMiddle();
	bool setMoving(bool nMoving);
	void incAnim();
	bool incWaited();
	void changeDirection(bool nDirection);
	void heal(int hp);

	//Stat upgrades from chests
	void healthUp();
	void speedUp();
	void dmgUp();

	//Combat
	void getHit(sf::Vector2f pos, int knockBackDistance, int length, float damage);
	void updateWeapon();
	bool checkHit();
	void attack();

	//Updates idle/moving animations each frame
	void animation();
};