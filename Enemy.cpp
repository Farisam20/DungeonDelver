
#pragma once
#include "Enemy.h"

//ABSTRACT ENEMY
Enemy::Enemy(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window)
{
	// Texture & Position
	this->texture = new sf::Texture();
	this->setPosition(position);
	this->setOrigin(8,8);

	// Pointers to game values
	this->game = game;
	this->delver = delver;
	this->level = level;
	this->window = window;

	// Enemy Specific Values
	this->targetVector = game->convertToWorldPosition(game->smallestIn3By3(getCurrentPos()));
	this->distanceFromMiddle = 200; // How far the enemy has to be in order to pathfind again
	this->spriteCount = 5; // How many distinct sprites there are
	this->textureUpdate = 200; // How many miliseconds pass until the texture changes
	this->aggroRange = 25; // How close the enemy has to be to start pathing towards player
	this->updateable = true;

}
Enemy::~Enemy()
{
	delete texture;
	delete this;
}

void Enemy::setUpdateable(bool update)
{
	this->updateable = update;
}
int Enemy::getHealth() // Get enemy health
{
	return health;
}
bool Enemy::isupdateable()
{
	return updateable;
}
sf::Sprite* Enemy::getSprite() // Get enemy this
{
	return this;
}
sf::Vector2i Enemy::getCurrentPos() // Get current pos
{
	return  game->calcSpritePos(this->getPosition(), level->getPosition()); // Get Enemy position
}
int Enemy::getKnockBackLength() // Get knockback
{
	return knockBackLength;
}
int Enemy::getHitStun() // Get hitstun
{
	return hitStun;
}
int Enemy::getDamage()
{
	return damage;
}
void Enemy::update()
{
	updateTexture(); // Update texture
	checkforHit(); // Check for contact damage
	moveToEnemy(); // Move towards player
	if (inHitstun)
	{
		knockBack(); // Get knocked back
	}
}
bool Enemy::checkforHit()
{
	if (contactDamage) // Only check for contact if contact damage
	{
		if (this->getGlobalBounds().intersects(delver->getGlobalBounds())) // Check for intersection with delver and enemy
		{
			if (delver->getInvuln() == 0)
			{
				delver->getHit(this->getPosition(), knockBackLength, hitStun, damage); // Hit delver
				delver->setInvuln(50);
			}
			return true; // Return enemy was it
		}
	}
	return false; // Enemy was not hit

}
void Enemy::moveToEnemy()
{
	if (!inHitstun) // If not in hitstun
	{
		sf::Vector2f currentPos = this->getPosition();
		if (distanceBetween(currentPos, targetVector) <= distanceFromMiddle) // If close enough, find new middle to path too
		{
			targetVector = game->convertToWorldPosition(game->smallestIn3By3(getCurrentPos()));
		}

		int horiMove = currentPos.x > targetVector.x ? -1 : abs(ceil(currentPos.x) - ceil(targetVector.x)) <= 3 ? 0 : 1; // Move left, right or 0
		int vertMove = currentPos.y > targetVector.y ? -1 : abs(ceil(currentPos.y) - ceil(targetVector.y)) <= 3 ? 0 : 1; // Move up, down or 0
		this->move(sf::Vector2f(horiMove * movespeed, vertMove * movespeed)); // Move
	}
}

void Enemy::knockBack()
{

	sf::Vector2f move(0, 0);
	// Calculate the movement of the knockback of the enemy
	if (game->checkValidPosition(game->calcSpritePos(this->getPosition() + sf::Vector2f((cos((angle * -1) * (PI / 180))) * movespeed, 0), level->getPosition())))
	{
		move.x = (cos((angle * -1) * (PI / 180))) * movespeed;
	}
	if (game->checkValidPosition(game->calcSpritePos(this->getPosition() + sf::Vector2f(0, -(sin((angle * -1) * (PI / 180))) * movespeed), level->getPosition())))
	{
		move.y = -(sin((angle * -1) * (PI / 180))) * movespeed;
	}
	this->move(move); // Move knockback amount
	hitstunAmount -= 1; // Decrease hitstun
	if (hitstunAmount <= 0)
	{
		inHitstun = false; // Get out of hitstun
		targetVector = game->convertToWorldPosition(game->smallestIn3By3(getCurrentPos())); // Set new target vector
		this->setColor(sf::Color(255, 255, 255, 255)); // Reset color
	}

}
void Enemy::getHit(int angle, int hitstun, int damage)
{
	if (!inHitstun) // If not in hitstun, start hit process
	{
		this->angle = angle; // Set angle to be knocked back at
		this->hitstunAmount = hitstun; // Set hitstun
		this->inHitstun = true;
		health -= damage; // Decrease health
		this->setColor(sf::Color::Red);
	}

}
void Enemy::updateTexture()
{
	//Update texture and change to face player
	if (textureTimer.getElapsedTime().asMilliseconds() > textureUpdate)
	{
		this->setTextureRect(sf::IntRect(textPosition.x + texturePosition * game->getTileSize(), textPosition.y, game->getTileSize(), game->getTileSize()));
		if (this->getPosition().x < delver->getPosition().x)
		{
			if (this->getScale().x < 0)
			{
				this->scale(-1 * 1.f, 1.f);
			}
		}
		else
		{
			if (this->getScale().x > 0)
			{
				this->scale(-1.f, 1.f);
			}
		}
		texturePosition++;
		if (texturePosition > spriteCount) // Reset to original texture
		{
			texturePosition = 0;
		}
		textureTimer.restart();
	}
}



//SLIME
Slime::Slime(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window) : Enemy(position, delver, game, level, window)
{
	// Scale and Texture
	int randScale = (rand() % 4) + 3; //3 to 6
	this->scale(randScale, randScale);
	this->texture->loadFromFile("assets/textures/spritesheet.png");
	this->setTexture(*texture);
	textPosition = sf::Vector2i(0, 32);
	this->setTextureRect(sf::IntRect(textPosition.x, textPosition.y, game->getTileSize(), game->getTileSize()));

	// Enemy values
	this->health = 10 + (rand() % 10) + level->getFloor()/2 + randScale;
	this->inHitstun = false;
	this->contactDamage = true;
	this->hitStun = 10;
	this->knockBackLength = 1.5;
	this->texturePosition = 1;
	this->damage = 3 + (rand() % 3) + level->getFloor()/2; // Random amount of damage, at least 3 + floor/2
	this->movespeed = 2.5;
}
void Slime::moveToEnemy()
{
	if (moveTimer.getElapsedTime().asMilliseconds() > 750 && !inHitstun) // Slime is moveable
	{
		Enemy::moveToEnemy();
		if (moveTimer.getElapsedTime().asMilliseconds() > 1300) // Reset move timer
		{
			moveTimer.restart();
		}
	}
	else
	{
		targetVector = game->convertToWorldPosition(game->smallestIn3By3(getCurrentPos()));

	}
}



//GOBLIN
Goblin::~Goblin()
{
	delete sword;
}
Goblin::Goblin(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window) : Enemy(position, delver, game, level, window)
{
	//Texture
	int randScale = (rand() % 3);
	this->scale(2.f + randScale, 2.f + randScale);
	this->texture->loadFromFile("assets/textures/spritesheet.png");
	textPosition = sf::Vector2i(0, 80);
	this->setTextureRect(sf::IntRect(textPosition.x, textPosition.y, game->getTileSize(), game->getTileSize()));
	this->setTexture(*texture);
	this->setOrigin(game->getTileSize() / 2, game->getTileSize() / 2);
	randScale = rand() % 12;
	this->setScale(3.f + randScale / 3, 3.f + randScale / 3);
	this->texturePosition = 1;

	// Sword Values
	this->knockBackLength = 1.5;
	this->sword = new defaultSword(delver, window, nullptr, true, this);
	this->attackDistance = (randScale)+150;
	this->text = new sf::Texture();
	sf::Sprite* model = sword->getModel();
	this->attackCooldown = 1500;

	// Member Values
	this->damage = 10 + level->getFloor() / 2;
	this->movespeed = 2.5;
	this->health = 10 + (rand() % 15) + level->getFloor() / 2;
	this->inHitstun = false;
	this->hitStun = 10;
}
defaultSword* Goblin::getWeapon() // Weapon getter
{
	return sword;
}
void Goblin::moveToEnemy()
{
	// Attack if not in hitstun, player is close enough, and attack cooldown is done
	if (!inHitstun && distanceBetween(this->getPosition(), delver->getPosition()) <= attackDistance && attackTimer.getElapsedTime().asMilliseconds() > attackCooldown)
	{
		sword->attack();
		attackTimer.restart();
	}
	else // Otherwise move
	{
		Enemy::moveToEnemy();
	}

}



//FLOATING EYE
Eye::Eye(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window) : Enemy(position, delver, game, level, window)
{
	// Texture and Scale
	this->scale(3.5f, 3.5f);
	this->texture->loadFromFile("assets/textures/spritesheet.png");
	textPosition = sf::Vector2i(0, 96);
	this->setTextureRect(sf::IntRect(textPosition.x, textPosition.y, game->getTileSize(), game->getTileSize()));
	this->setTexture(*texture);

	// Member Values
	this->health = 3 + (rand() % 2) + level->getFloor() / 2;
	this->hitStun = 10;
	this->knockBackLength = 0;
	this->texturePosition = 1;
	this->damage = 7 + level->getFloor() / 2;
	this->movespeed = 1.5;
	this->shootDistance = 4;
	this->spriteCount = 3;
	this->textureUpdate = 100;
	this->runDistance = std::max(shootDistance - 1, 1); // Run distance can never be 0
	this->attackCooldown = 1000 + (rand() % 500); // Everything is more fun when its random
}
void Eye::update()
{
	Enemy::update();
	if (!inHitstun) // If not in hitstun, shoot
	{
		shoot();
	}

}
sf::Vector2i Eye::nextMovement(sf::Vector2i currentPos) // Check if the eye should move closer, keep distance, or run
{
	int smallest = game->getMoveIndex(game->smallestIn3By3(getCurrentPos()));
	if (smallest <= shootDistance)
	{
		if (smallest < runDistance)
		{
			return game->biggestIn3by3(currentPos); // Eye runs from player
		}
		return sf::Vector2i(-1, -1); // Eye stays the same position
	}
	return game->smallestIn3By3(currentPos); // Eye moves closer

}
void Eye::shoot()
{
	if (attackTimer.getElapsedTime().asMilliseconds() > attackCooldown) // Shoot at player if cooldown is low enough
	{
		Enemy* shot = new Projectile(this->getPosition(), delver, game, level, window, -1 * pointTowardsCursor(this, window, delver), "assets/textures/eyeProjectile.png");
		game->addEnemy(shot); // Add projectile to enemylist
		attackTimer.restart();
	}
}
void Eye::moveToEnemy()
{
	if (nextMovement(getCurrentPos()) != sf::Vector2i(-1, -1)) // The vector will be -1,-1 when the eye is the correct distance from player, i.e. no need to move
	{
		if (!inHitstun) // Besides the run away function, the rest of this is the same from the default move function
		{
			sf::Vector2f currentPos = this->getPosition();
			int smallest = game->getMoveIndex(game->smallestIn3By3(getCurrentPos()));
			if (smallest <= shootDistance)
			{
				if (smallest < runDistance)
				{
					targetVector = game->convertToWorldPosition(game->biggestIn3by3(getCurrentPos())); // Run from the player
				}
			}
			else
			{
				if (distanceBetween(currentPos, targetVector) <= distanceFromMiddle)
				{
					targetVector = game->convertToWorldPosition(game->smallestIn3By3(getCurrentPos())); // Move towards player

				}
			}
			int horiMove = currentPos.x > targetVector.x ? -1 : abs(ceil(currentPos.x) - ceil(targetVector.x)) <= 3 ? 0 : 1; // Move left, right or 0
			int vertMove = currentPos.y > targetVector.y ? -1 : abs(ceil(currentPos.y) - ceil(targetVector.y)) <= 3 ? 0 : 1; // Move up, down or 0
			this->move(sf::Vector2f(horiMove * movespeed, vertMove * movespeed));
		}
	}

}



//PROJECTILES
Projectile::Projectile(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window, int angle, std::string path) : Enemy(position, delver, game, level, window)
{
	// Texture and Scale
	this->scale(2.f, 2.f);
	this->texture->loadFromFile(path);
	this->setTexture(*texture);
	this->setOrigin(this->getTexture()->getSize().x / 2, this->getTexture()->getSize().y / 2);

	// Member Values
	this->health = 1; // One health so it gets destroyed by sword
	this->movespeed = 5;
	this->hitStun = 14;
	this->knockBackLength = 0;
	this->damage = 7 + level->getFloor() / 2;
	this->projAngle = angle; // Angle that the projectile moves at
	this->setRotation(90 - angle); // Angle the projectile faces
	this->contactDamage = true;
}

void Projectile::moveToEnemy()
{
	sf::Vector2f move = sf::Vector2f(cos(projAngle * (PI / 180)) * movespeed, -sin(projAngle * (PI / 180)) * movespeed); // Move towards enemy
	this->move(move);
	if (!game->checkValidPosition(getCurrentPos())) // If out of bounds, health = 0
	{
		health = 0;
	}
}

void Projectile::updateTexture() // This is necessary to prevent the texture from changing
{
	// DO NOTHING
}

void Projectile::setAngle(double angle)
{
	this->projAngle = angle;
}

bool Projectile::checkforHit()
{
	if (Enemy::checkforHit()) // If hit, delete projectile
	{
		health = 0;
		return true;
	}
	return false;
}



//BOSS
Nyarlathotep::Nyarlathotep(sf::Vector2f position, Delver* delver, Game* game, Level* level, sf::RenderWindow* window) : Enemy(position, delver, game, level, window)
{
	// Texture and Scale
	this->scale(0.6f, 0.6f);
	this->tileSize = sf::Vector2i(200, 450);
	this->summonSprite = new sf::Texture();
	this->portalSprite = new sf::Texture();
	this->texture->loadFromFile("assets/textures/nyarWalking.png");
	this->summonSprite->loadFromFile("assets/textures/nyarSummoning.png");
	//this->portalSprite->loadFromFile("assets/textures/nyarPortal.png");
	this->setTexture(*texture);
	this->setOrigin(tileSize.x / 2, tileSize.y / 2);
	this->textPosition = sf::Vector2i(0, 0);
	this->setTextureRect(sf::IntRect(textPosition.x, textPosition.y, tileSize.x, tileSize.y));
	this->texturePosition = 1;
	this->textureUpdate = 500;
	setSummoningRects();

	// Member Values
	this->health = 150 + level->getFloor()*5;
	this->movespeed = 1;
	this->spriteCount = 3;
	this->inHitstun = false;
	this->contactDamage = true;
	this->hitStun = 10;
	this->damage = 10 + level->getFloor() / 2;
	this->knockBackLength = 2;
	this->summoningPosition = 0;
	this->projectileDistance = 140;
	this->spinning = false;
	this->charging = false;

}
Nyarlathotep::~Nyarlathotep()
{
	delete summonSprite;
	delete portalSprite;
}

void Nyarlathotep::summoningPortal()
{

}
void Nyarlathotep::startChanneling()
{
	if (summoningPosition < 14)
	{
		if (summoningPosition == 0)
			this->setTexture(*summonSprite);

		if (textureTimer.getElapsedTime().asMilliseconds() > 400)
		{
			int x = summoningPosition % 5; //0-4
			int y = summoningPosition / 5; //0-2
			this->setTextureRect(sf::IntRect(x*tileSize.x, y*tileSize.y, tileSize.x, tileSize.y));
			this->setOrigin(tileSize.x/2, tileSize.y/2);
			summoningPosition++;
			textureTimer.restart();
		}
	}
	else
	{
		summoningPosition = 0;
		channeling = false;
		this->setColor(sf::Color(255, 255, 255, 255));
		this->setTexture(*texture);
		this->setTextureRect(sf::IntRect(0, 0, tileSize.x, tileSize.y));
		pickChannelOption();

		textureTimer.restart();
		channelTimer.restart();
	}
}

void Nyarlathotep::update()
{
	if (charging && chargeChargeup.getElapsedTime().asMilliseconds() > 3000)
	{
		sf::Vector2f move = sf::Vector2f(cos(angle * (PI / 180)) * (movespeed * 13), sin(angle * (PI / 180)) * (movespeed * 13)); // Move towards enemy
		sf::Vector2f buffer(50, 130);
		buffer.x *= move.x > 0 ? 1 : -1;
		buffer.y *= move.y > 0 ? 1 : -1;
		if (buffer.y < 0)
		{
			buffer.y += 130;
		}
		if (!game->checkValidPosition(game->calcSpritePos(this->getPosition() + (move)+buffer, level->getPosition())))
		{
			charging = false;
			return;
		}
		this->move(move);

	}
	else if (charging)
	{
		this->setColor(sf::Color::Green);
		angle = pointTowardsCursor(this, window, delver);
	}
	else
	{
		if (!channeling)
		{
			updateTexture();
			moveToEnemy();
		}
		else
		{
			startChanneling();
		}
		if (spinning)
		{
			if (spinShootTimer.getElapsedTime().asMilliseconds() > 1500)
			{
				for (int i = 0; i < projList.size(); ++i)
				{
					((Projectile*)projList[i])->setAngle(90 - ((Projectile*)projList[i])->projAngle);
					((Projectile*)projList[i])->updateable = true;


				}
				spinning = false;
			}

		}

		if (channelTimer.getElapsedTime().asMilliseconds() > 8000)
		{
			channeling = true;
		}
	}

	checkforHit(); // Check for contact damage
	if (inHitstun)
	{
		knockBack(); // Get knocked back
	}

}

void Nyarlathotep::pickChannelOption()
{
	int option = rand() % 8;
	switch (option)
	{
	case 0:
		game->randomlySpawn(4);
		break;
	case 1:
	case 2:
	case 3:
		projectileCircle(option * 4);
		break;
	case 4:
	{
		Enemy* enemy = game->spawnSpecific(0);
		((Slime*)enemy)->scale(2.f, 2.f);
		((Slime*)enemy)->health *= 2;
		((Slime*)enemy)->movespeed /= 1.25;
		break;
	}
	case 5:
	{
		Enemy* enemy = game->spawnSpecific(1);
		((Goblin*)enemy)->scale(2.f, 2.f);
		((Goblin*)enemy)->getWeapon()->getModel()->scale(2.f, 2.f);
		((Goblin*)enemy)->attackDistance *= 2;
		((Goblin*)enemy)->attackCooldown = 3000;
		((Goblin*)enemy)->health *= 1.5;
		((Goblin*)enemy)->movespeed /= 1.5;
		break;
	}
	case 6:
	{
		Enemy* enemy = game->spawnSpecific(2);
		((Eye*)enemy)->scale(2.f, 2.f);
		((Eye*)enemy)->health *= 2;
		((Eye*)enemy)->movespeed *= 1.5;
		((Eye*)enemy)->attackCooldown *= 0.8;
		break;
	}
	case 7:
		charging = true;
		chargeChargeup.restart();
		break;


	}
}

void Nyarlathotep::moveToEnemy()
{

	sf::Vector2f move(0, 0);
	sf::Vector2f currentPos = this->getPosition();
	targetVector = delver->getPosition();

	int horiMove = currentPos.x > targetVector.x ? -1 : abs(ceil(currentPos.x) - ceil(targetVector.x)) <= 3 ? 0 : 1; // Move left, right or 0
	if (game->checkValidPosition(game->calcSpritePos(this->getPosition() + sf::Vector2f(horiMove * (movespeed + tileSize.x / 2), 0), level->getPosition())))
	{
		move.x = horiMove * movespeed;
	}
	int vertMove = currentPos.y > targetVector.y ? -1 : abs(ceil(currentPos.y) - ceil(targetVector.y)) <= 3 ? 0 : 1; // Move up, down or 0
	if (game->checkValidPosition(game->calcSpritePos(this->getPosition() + sf::Vector2f(0, vertMove * (movespeed + tileSize.y / 3)), level->getPosition())))
	{
		move.y = vertMove * movespeed;
	}

	if (spinning)
	{
		for (int i = 0; i < projList.size(); ++i)
		{
			projList[i]->move(move);
			if (spinTimer.getElapsedTime().asMilliseconds() > 30)
			{
				double rotAngle = (0.1);
				//double rotAngle = 0.1;
				projList[i]->setPosition(sf::Vector2f(this->getPosition().x + cos((((Projectile*)projList[i])->projAngle) * (PI / 180) + rotAngle * overallAngle) * projectileDistance, this->getPosition().y + sin(((((Projectile*)projList[i])->projAngle) * (PI / 180)) + rotAngle * overallAngle) * projectileDistance));
				((Projectile*)projList[i])->setRotation(((((Projectile*)projList[i])->projAngle) + ((rotAngle * overallAngle) * (180 / PI))) - 270);
				if (this->getGlobalBounds().intersects(delver->getGlobalBounds()))
				{
					((Projectile*)projList[i])->checkforHit();
				}
			}

		}

		if (spinTimer.getElapsedTime().asMilliseconds() > 30)
		{
			++overallAngle;
			spinTimer.restart();
		}
	}

	this->move(move); // Move

}

void Nyarlathotep::setSummoningRects()
{
	summoningRects[0] = sf::IntRect(10, 75, 155, 345);
	summoningRects[1] = sf::IntRect(165, 45, 165, 375);
	summoningRects[2] = sf::IntRect(335, 0, 165, 420);
	summoningRects[3] = sf::IntRect(510, 0, 165, 420);
	summoningRects[4] = sf::IntRect(685, 0, 165, 420);
	summoningRects[5] = sf::IntRect(0, 420, 165, 420);
	summoningRects[6] = sf::IntRect(165, 420, 170, 420);
	summoningRects[7] = sf::IntRect(335, 420, 175, 420);
	summoningRects[8] = sf::IntRect(510, 420, 175, 420);
	summoningRects[9] = sf::IntRect(685, 420, 175, 420);
	summoningRects[10] = sf::IntRect(0, 840, 175, 419);
	summoningRects[11] = sf::IntRect(175, 840, 175, 419);
	summoningRects[12] = sf::IntRect(350, 920, 175, 339);
	summoningRects[13] = sf::IntRect(525, 920, 175, 339);
}

void Nyarlathotep::channel()
{

}

void Nyarlathotep::updateTexture()
{
	//Update texture and change to face player
	if (textureTimer.getElapsedTime().asMilliseconds() > textureUpdate)
	{
		this->setTextureRect(sf::IntRect(0 + 200*texturePosition, 0, 200, 450));
		if (this->getPosition().x < delver->getPosition().x)
		{
			if (this->getScale().x < 0)
			{
				this->scale(-1 * 1.f, 1.f);
			}
		}
		else
		{
			if (this->getScale().x > 0)
			{
				this->scale(-1.f, 1.f);
			}
		}
		texturePosition++;
		if (texturePosition > spriteCount) // Reset to original texture
		{
			texturePosition = 0;
		}
		textureTimer.restart();
	}
}

void Nyarlathotep::projectileCircle(int amount)
{

	double currAngle = 0;
	spinning = true;
	overallAngle = 0;
	spinShootTimer.restart();
	for (int i = 0; i < amount; ++i)
	{
		//sf::Vector2f(this->getPosition().x * cos(currAngle) * projectileDistance, this->getPosition().y * sin(currAngle) * projectileDistance)
		Enemy* newProj = new Projectile(sf::Vector2f(this->getPosition().x + cos(currAngle) * projectileDistance, this->getPosition().y + sin(currAngle) * projectileDistance), delver, game, level, window, 360 - (currAngle * 180 / PI), "assets/textures/nyarProj.png");
		//newProj->isupdateable = false;
		newProj->setUpdateable(false);
		//newProj->setColor(sf::Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, 255));
		newProj->setColor(sf::Color(rand() % 255 + 1, 255, 255, 255));
		currAngle += ((2 * PI) / amount);
		projList.push_back(newProj);
		game->addEnemy(newProj);
	}
}

