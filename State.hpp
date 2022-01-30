
#pragma once
#include "Header.h"



//State enumeration for button class
enum btnState { idle, hover, pressed };

class Button : public sf::Sprite
{
protected:
	//Base shape + text
	sf::RectangleShape shape;
	sf::Font* font;
	sf::Text text;

	//Color changes
	sf::Color idleColor;
	sf::Color hoverColor;

	short unsigned state;

public:
	//Constructor
	Button(float x = 0, float y = 0, float width = 0, float height = 0, sf::Font* font = nullptr, std::string str = "", sf::Color idleCol = sf::Color::Black, sf::Color hoverCol = sf::Color::Black);

	//Getters
	bool isPressed();

	//Other functions
	virtual void update(const sf::Vector2f mousePos);
	virtual void render(sf::RenderTarget* target);
};


//Special button that contains a sprite.
//Used when opening chests to upgrade a stat
class upgButton : public Button
{
protected:
	sf::Sprite* icon;

public:
	upgButton(float x, float y, float width, float height, sf::Font* nFont, std::string str, sf::Sprite* icon, sf::Color idleCol, sf::Color hoverCol);

	void render(sf::RenderTarget* target);
};





//Pure virtual class defining a certain
//state in the game (e.g. menu, paused, etc)
class State
{
protected:
	//Window vars
	sf::RenderWindow* window;
	sf::Vector2f center;

	//Text
	sf::Font* font;
	sf::Text text;

	//Button array
	std::vector<Button*> options;

	//Misc
	std::string ID;
	bool quit;
	int nextState;

public:
	State(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont)
	{
		//Window vars
		window = nWindow;
		center = nCenter;

		//Text
		font = nFont;
		text.setFont(*font);

		//Misc
		ID = "";
		quit = false;
		nextState = 0;
	}
	virtual ~State()
	{
		//Deallocate all the buttons
		for (int i = 0; i < options.size(); i++)
			delete options[i];
	}

	//Getters
	int getNextState() { return nextState; }
	std::string getID() { return ID; }

	//Functions to quit out of current state
	bool getQuit() { return quit; }
	virtual void checkQuit()
	{
		if (nextState != options.size())
			quit = true;
	}

	//Rendering
	virtual void update()
	{
		//Update all buttons in array
		for (int i = 0; i < options.size(); i++)
		{
			options[i]->update((sf::Vector2f)sf::Mouse::getPosition());

			//Check if player pressed a button
			if (options[i]->isPressed())
				nextState = i;
		}
		checkQuit();
	}
	virtual void render()
	{
		window->draw(text);
		for (int i = 0; i < options.size(); i++)
			options[i]->render(window);
	}
};


//State for starting the game, can either begin or exit
class menuState : public State
{
protected:
	sf::Texture logoTex;
	sf::Sprite logo;     //game logo sprite

public:
	menuState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont);

	void render();
};


//State for a paused game, gives user the option to resume or quit
class pauseState : public State
{
protected:
	sf::RectangleShape filter;  //transparent rectangle to darken rest of screen
	sf::Texture screenshot;
	sf::Sprite screenSprite;

public:
	pauseState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont);

	void render();
};


//State for when user is interacting with a chest
class upgradeState : public State
{
protected:
	//Chest submenu layout
	sf::Texture* tileset;
	sf::RectangleShape menu;
	sf::Sprite* icons[3];

	//Screenshotting
	sf::Texture screenshot;
	sf::Sprite screenSprite;

public:
	upgradeState(float width, float height, sf::Texture*& ntileset, sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont);

	void render();
};


//State for when player dies, gives option to go back to main menu or quit
class overState : public State
{
protected:
	sf::RectangleShape filter;
	sf::Texture screenshot;
	sf::Sprite screenSprite;

public:
	overState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont);

	void render();
};