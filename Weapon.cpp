
#pragma once
#include "Weapon.h"


//Weapon parent class
Weapon::Weapon(Delver* player, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, bool evil, Enemy* enemy)
{
	// Member values
	this->attacking = false;
	this->player = player;
	this->RW = window;
	this->angle = -1;
	this->curAttackStage = 1;
	this->enemyList = enemyList;
}
bool Weapon::isAttacking()
{
	return attacking;
}


//defaultSword child class
defaultSword::defaultSword(Delver* player, sf::RenderWindow* window, std::vector<Enemy*>* enemyList, bool evil, Enemy* enemy) : Weapon(player, window, enemyList, evil, enemy)
{
	this->evil = evil;

	//Texture & Model stuff
	this->texture = new sf::Texture();
	if (!evil)
	{
		texture->loadFromFile("assets/textures/sword.png"); // If player sword, sword texture
	}
	else
	{
		texture->loadFromFile("assets/textures/knife.png"); // If enemy sword, knife texture
	}
	this->model = new sf::Sprite(*texture);
	this->model->setOrigin(this->model->getTexture()->getSize().x / 2, this->model->getTexture()->getSize().y);
	if (!evil)
	{
		this->model->scale(4.f, 4.f);
	}
	else
	{
		this->model->scale(3.f, 3.f);
	}

	// Member values
	this->totalAttackStage = 12;
	this->enemy = enemy;
	this->damage = 10;
}
defaultSword::~defaultSword()
{
	delete texture;
	delete model;
}

void defaultSword::update()
{
	currentlyAttacking(); // If currently attacking
	if (attacking)
	{
		if (evil)
		{
			model->setPosition(enemy->getPosition()); // If evil, set positition to enemy
		}
	}
}

void defaultSword::render()
{
	if (attacking)
		RW->draw(*model);
}

void defaultSword::attack()
{
	if (!attacking)
	{
		attacking = true; // Start attacking
		if (!evil)
		{
			this->model->setPosition(player->getPosition()); // Set position
			angle = (int)pointTowardsCursor(player, RW, nullptr); // Set angle between cursor and player
			this->model->setRotation(angle); // Set rotation
		}
		else
		{
			this->model->setPosition(enemy->getPosition()); // Set position
			angle = angle = (int)pointTowardsCursor(enemy, RW, player); // Set angle btween player and enemy
			this->model->setRotation(angle); // Set rotation
		}

	}
}
sf::Sprite* Weapon::getModel() // Getter
{
	return this->model;
}
void defaultSword::currentlyAttacking()
{
	if (attacking)
	{
		if (curAttackStage < totalAttackStage) // Sword is still swinging
		{
			int currAngle = angle + (180 / totalAttackStage) * curAttackStage;
			this->model->setRotation(currAngle); // Rotate sword a fraction of the total distance
			curAttackStage++;
			if (!evil) // If the player is using the sword, check each enemy in the list to see if it needs to be hit
			{
				for (int i = 0; i < enemyList->size(); ++i)
				{

					if (model->getGlobalBounds().intersects(((*enemyList)[i])->getSprite()->getGlobalBounds()))
					{
						((*enemyList)[i])->getHit(angle, 20, player->getDamage());
					}
				}
			}
			else // If the enemy is using the sword, only check for player intersection
			{
				if (model->getGlobalBounds().intersects(player->getGlobalBounds()) && player->getInvuln() == 0)
				{
					player->getHit(enemy->getPosition(), enemy->getKnockBackLength(), enemy->getHitStun(), enemy->getDamage());
				}
			}

		}
		else // Sword is done swinging
		{
			curAttackStage = 1;
			attacking = false;
		}
	}
}