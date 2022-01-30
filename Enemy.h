
#pragma once
#include "Header.h"

class Delver;
class Game;
class Level;
class Weapon;

//Abstract class defining all enemies
class Enemy : public sf::Sprite

{
public:
	Enemy(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window);
	~Enemy();
	sf::Sprite* getSprite();
	sf::Vector2i getCurrentPos();
	void virtual moveToEnemy();
	int getHealth();
	bool virtual checkforHit();
	void virtual getHit(int angle, int hitstun, int damage);
	int getKnockBackLength();
	int getHitStun();
	int getDamage();
	void virtual updateTexture();
	void virtual update();
	void knockBack();
	bool isupdateable();
	void setUpdateable(bool update);


protected:
	// Member Values
	sf::Clock moveTimer;
	bool inHitstun;
	bool updateable;
	int damage;
	int health;
	int distanceFromMiddle;
	int aggroRange;
	int hitstunAmount;
	int angle;
	int knockBackLength;
	int contactDamage;
	int hitStun;
	int textureUpdate;
	int spriteCount;
	int texturePosition;
	float movespeed;
	std::stack<sf::Vector2i> moveStack;
	sf::Vector2i textPosition;
	sf::Clock textureTimer;
	sf::Vector2f targetVector;

	// Pointers
	sf::Sprite* model;
	sf::Texture* texture;
	Game* game;
	Level* level;
	sf::RenderWindow* window;
	Delver* delver;
};


class Slime : public Enemy
{
	friend class Nyarlathotep;
public:
	Slime(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window);
	void moveToEnemy();
};


class Goblin : public Enemy
{
	friend class Nyarlathotep;
public:

	Goblin(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window);
	~Goblin();
	void moveToEnemy();
	defaultSword* getWeapon();

private:
	defaultSword* sword;
	sf::Texture* text;
	int attackDistance;
	int attackCooldown;
	sf::Clock attackTimer;
};


class Projectile : public Enemy
{
	friend class Nyarlathotep;
public:
	Projectile(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window, int angle, std::string path);
	void moveToEnemy();
	bool checkforHit();
	void updateTexture();
	void setAngle(double angle);


protected:
	int projAngle;
};


class Eye : public Enemy
{
	friend class Nyarlathotep;
public:
	Eye(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window);
	void moveToEnemy();
	void shoot();
	void update();
	sf::Vector2i nextMovement(sf::Vector2i currentPos);
	sf::Clock attackTimer;

protected:
	int shootDistance;
	int runDistance;
	int attackCooldown;
};


class Nyarlathotep : public Enemy
{
public:
	Nyarlathotep(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window);
	~Nyarlathotep();
	void update();
	void summoningPortal();
	void moveToEnemy();
	void updateTexture();
	void channel();
	void setSummoningRects();
	void startChanneling();
	void pickChannelOption();
	void projectileCircle(int amount);

protected:
	bool channeling;
	bool spinning;
	bool charging;
	int angle;
	int summoningPosition;
	int projectileDistance;
	int overallAngle;
	sf::Texture* summonSprite;
	sf::Vector2i tileSize;
	sf::Texture* portalSprite;
	sf::IntRect summoningRects[14];
	std::vector<Enemy*> projList;
	sf::Clock spinTimer;
	sf::Clock spinShootTimer;
	sf::Clock channelTimer;
	sf::Clock chargeChargeup;
};