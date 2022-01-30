
#pragma once
#include "Delver.h"

Delver::Delver(float nMaxHP, float nDamage, float nAtkSpeed, float nMoveSpeed, int nIframes, int nAnimFrames, sf::Vector2u scale, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, Level* nLevel, Game* nGame)
{
	maxHP = nMaxHP;
	health = maxHP;
	damage = nDamage;
	atkSpeed = nAtkSpeed;
	moveSpeed = nMoveSpeed;
	iframes = nIframes;
	moving = false;
	direction = 1;
	animationNum = 0;
	framesWaited = 0;
	animFrames = nAnimFrames;
	tileSize = sf::Vector2u(16, 16);
	globalScale = scale;
	attackCooldown = sf::Clock();
	firstAttack = true;
	hitLength = 0;
	this->window = window;
	this->enemyList = enemyList;
	this->game = nGame;
	this->level = nLevel;
	numSpeedUps = 0;
	numDmgUps = 0;
	weapon = new defaultSword(this, window, enemyList, false, nullptr);
}

//Setters
void Delver::setMiddle()
{
	this->setOrigin(this->getTexture()->getSize().x / 2, this->getTexture()->getSize().y / 2);
}
bool Delver::setMoving(bool nMoving)
{
	if (nMoving != moving)
	{
		animationNum = 0;
		moving = nMoving;
		return true;
	}
	return false;
}
void Delver::incAnim()
{
	//loops through animation cycle, each sprite has 6 for moving/standing
	animationNum++;
	if (animationNum > 5)
		animationNum = 0;
}
bool Delver::incWaited()
{
	framesWaited++;
	if (framesWaited >= animFrames)
	{
		framesWaited = 0;
		return true;
	}
	return false;
}
void Delver::changeDirection(bool nDirection)
{
	//If direction is changing
	if (nDirection != direction)
	{
		this->scale(-1, 1); //flip sprite about vertical axis
		direction = nDirection;
	}
}
void Delver::heal(int hp)
{
	health += hp;
	if (health > maxHP)
		health = maxHP;
}

//Upgrade functions
void Delver::healthUp()
{
	maxHP += 5;
	health += 5;
}
void Delver::speedUp()
{
	//Base speed = 60 PPS (pixels per sec), += ~3 PPS. 66% diminishing returns after 5 upgrades (75 PPS)
	numSpeedUps++;
	if (numSpeedUps <= 5)
		moveSpeed += 3;
	else if (numSpeedUps <= 10)
		moveSpeed += 1;
}
void Delver::dmgUp()
{
	//Base damage = 5, += 1. 25% DR after 5 upgrades, 50% after 10
	numDmgUps++;
	if (numDmgUps <= 5)
		damage++;
	else if (numDmgUps <= 10)
		damage += .75;
	else if (numDmgUps <= 20)
		damage += .5;
}

//Combat functions
void Delver::getHit(sf::Vector2f pos, int knockBackDistance, int length, float damage)
{
	iframes = 50;
	hitPos = pos;
	this->knockBackDistance = knockBackDistance;
	hitLength = length;
	health -= damage;
}
void Delver::updateWeapon()
{
	this->weapon->update();
}
bool Delver::checkHit()
{
	if (iframes != 0)
	{
		iframes--;
	}
	if (hitLength != 0)
	{

		int horiMove = hitPos.x > getPosition().x ? 1 : -1;
		int vertMove = hitPos.y > getPosition().y ? 1 : -1;
		int horiBuffer = 4.f * globalScale.x;
		int vertBuffer = 4.f * globalScale.y;
		sf::Vector2f move(0, 0);
		if (game->checkValidPosition(game->calcSpritePos(getPosition(), level->getPosition() + sf::Vector2f(knockBackDistance * horiMove * globalScale.x + (horiBuffer * horiMove), 0))))
		{
			move.x = knockBackDistance * horiMove * globalScale.x;
		}
		if (game->checkValidPosition(game->calcSpritePos(getPosition(), level->getPosition() + sf::Vector2f(0, knockBackDistance * vertMove * globalScale.y + (vertBuffer * vertMove)))))
		{
			move.y = knockBackDistance * vertMove * globalScale.y;
		}
		game->moveLevelHit(move);
		hitLength--;
		setColor(sf::Color::Red);
		return true;
	}
	setColor(sf::Color(255, 255, 255, 255));
	return false;
}
void Delver::attack()
{
	//Checks against spamming attacks
	if (attackCooldown.getElapsedTime().asMilliseconds() > 650 || firstAttack)
	{
		firstAttack = false;
		weapon->attack();
		attackCooldown.restart();
	}
}

//Changing sprite
void Delver::animation()
{
	if (!this->setMoving(moving) && this->incWaited())
	{
		this->incAnim();
		float y = moving * tileSize.y;            //idle sprites at y = 0, running at y = 16 in spritesheet
		float x = this->getAnim() * tileSize.x;   //6 animation frames, so do 16*(current animation) for proper sprite
		this->setTextureRect(sf::IntRect(x, y, tileSize.x, tileSize.y));
	}
}