
#pragma once
#include "Header.h"

//Graphical User Interface which displays relevant game state info
class GUI
{
private:
	sf::Font* font;
	sf::Text health;
	sf::Text floor;

public:
	GUI(sf::Vector2f center, sf::Texture*& ntileset, sf::Font* nFont)
	{
		//Info setup
		font = nFont;
		health.setFont(*font);
		health.setCharacterSize(50);
		floor.setFont(*font);
		floor.setCharacterSize(50);
		
		health.setPosition(sf::Vector2f(15, 70));
		floor.setPosition(sf::Vector2f(15, 10));
	}

	void update(int nHealth, int nFloor)
	{
		//Update hp
		std::string hp = std::to_string(nHealth);
		hp += "  HP";
		health.setString(hp);

		//Update floor number
		std::string fl = "F loor  ";
		fl += std::to_string(nFloor);
		floor.setString(fl);
	}
	void render(sf::RenderTarget* target)
	{
		//Draw text
		target->draw(health);
		target->draw(floor);
	}
};