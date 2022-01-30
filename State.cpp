
#pragma once 
#include "State.hpp"

//BUTTON
Button::Button(float x, float y, float width, float height, sf::Font* nFont, std::string str, sf::Color idleCol, sf::Color hoverCol)
{
	//Shape/size
	shape.setPosition(sf::Vector2f(x, y));
	shape.setSize(sf::Vector2f(width, height));

	//Text
	font = nFont;
	text.setFont(*font);
	text.setString(str);
	text.setCharacterSize(24);
	text.setPosition(sf::Vector2f(x + width / 2 - text.getGlobalBounds().width / 2, y + height / 2 - text.getGlobalBounds().height / 2)); //center text


	//Colors
	idleColor = idleCol;;
	hoverColor = hoverCol;

	state = idle;
}

bool Button::isPressed()
{
	return state == pressed;
}
void Button::update(sf::Vector2f mousePos)
{
	//Idle
	state = idle;

	//Hovering
	if (this->shape.getGlobalBounds().contains(mousePos))
	{
		state = hover;

		//Pressed
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			state = pressed;
	}

	//Update values based on above criteria
	switch (state)
	{
	case idle:
		shape.setFillColor(idleColor);
		break;

	case hover:
		shape.setFillColor(hoverColor);
		break;

	case pressed:
		shape.setFillColor(idleColor);
		break;
	}
}
void Button::render(sf::RenderTarget* target)
{
	target->draw(shape);
	target->draw(text);
}


//UPGRADE BUTTON
upgButton::upgButton(float x, float y, float width, float height, sf::Font* nFont, std::string str, sf::Sprite* icon, sf::Color idleCol, sf::Color hoverCol)
	: Button(x, y, width, height, nFont, str, idleCol, hoverCol)
{
	//Sprite
	this->icon = icon;
	icon->setOrigin(sf::Vector2f(8, 8));
	icon->setPosition(sf::Vector2f(x + width / 2, y + height / 2));
	icon->setScale(sf::Vector2f(4, 4));

	//Shift the text to accomodate sprite
	text.setPosition(sf::Vector2f(x + width / 2 - text.getGlobalBounds().width / 2, y + 20));
}

void upgButton::render(sf::RenderTarget* target)
{
	target->draw(shape);
	target->draw(text);
	target->draw(*icon);
}


//MENU
menuState::menuState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont) : State(nWindow, nCenter, nFont)
{
	int width = sf::VideoMode::getDesktopMode().width;
	int height = sf::VideoMode::getDesktopMode().height;

	//Logo sprite
	logoTex.loadFromFile("assets/textures/logo.png");
	logo.setTexture(logoTex);
	logo.setScale(6, 6);
	logo.setPosition(sf::Vector2f(width / 2 - logo.getGlobalBounds().width / 2, 100));

	//Buttons
	options.resize(2);
	options[0] = new Button(nCenter.x - 200, nCenter.y + 50, 400, 100, font, "Begin", sf::Color(155, 173, 183), sf::Color(110, 136, 150));
	options[1] = new Button(nCenter.x - 200, nCenter.y + 200, 400, 100, font, "Exit", sf::Color(155, 173, 183), sf::Color(110, 136, 150));

	nextState = 2;
	ID = "menu";
}
void menuState::render()
{
	window->clear();                   //black background;
	window->draw(logo);                //logo
	for (int i = 0; i < 2; i++)
		options[i]->render(window);    //buttons
}

//PAUSING
pauseState::pauseState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont) : State(nWindow, nCenter, nFont)
{
	//Filter to darken rest of screen
	int width = sf::VideoMode::getDesktopMode().width;
	int height = sf::VideoMode::getDesktopMode().height;
	filter.setSize(sf::Vector2f(width, height));        //Make the filter the size of the screen
	filter.setFillColor(sf::Color(0, 0, 0, 160));       //Set color to a slightly transparent black

	//Screenshotting
	sf::Vector2u size = window->getSize();
	screenshot.create(size.x, size.y);
	screenshot.update(*window);
	screenSprite.setTexture(screenshot);

	//Text
	text.setString("Paused");
	text.setPosition(sf::Vector2f(width / 2 - text.getGlobalBounds().width / 2, height / 5)); //Centered on y axis, 80% of the way up the screen

	//Buttons
	options.resize(2);
	options[0] = new Button(nCenter.x - 200, nCenter.y - 200, 400, 100, font, "Resume", sf::Color(155, 173, 183), sf::Color(110, 136, 150));
	options[1] = new Button(nCenter.x - 200, nCenter.y - 50, 400, 100, font, "Quit", sf::Color(155, 173, 183), sf::Color(110, 136, 150));

	nextState = 2;
	ID = "pause";
}
void pauseState::render()
{
	window->draw(screenSprite);
	window->draw(filter, sf::BlendAlpha);
	window->draw(text);
	for (int i = 0; i < 2; i++)
		options[i]->render(window);
}

//UPGRADING
upgradeState::upgradeState(float width, float height, sf::Texture*& ntileset, sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont) : State(nWindow, nCenter, nFont)
{
	//Upgrade sprites
	tileset = ntileset;
	for (int i = 0; i < 3; i++)
	{
		icons[i] = new sf::Sprite;
		icons[i]->setTexture(*tileset);
	}
	icons[0]->setTextureRect(sf::IntRect(64, 80, 16, 16)); //hp upgrade
	icons[1]->setTextureRect(sf::IntRect(80, 80, 16, 16)); //speed upgrade
	icons[2]->setTextureRect(sf::IntRect(96, 80, 16, 16)); //dmg upgrade

	//Chest submenu setup
	menu.setSize(sf::Vector2f(width, height));
	menu.setPosition(sf::Vector2f(nCenter.x - width / 2, nCenter.y - height / 2)); //center menu
	menu.setFillColor(sf::Color(42, 49, 64));
	float x = menu.getPosition().x + 50;
	float y = menu.getPosition().y + 50;

	//Screenshotting
	sf::Vector2u size = window->getSize();
	screenshot.create(size.x, size.y);
	screenshot.update(*window);
	screenSprite.setTexture(screenshot);

	//Buttons
	options.resize(3);
	options[0] = new upgButton(x, y, 200, 200, nFont, "Hea lth", icons[0], sf::Color(155, 173, 183), sf::Color(110, 136, 150));
	options[1] = new upgButton(x + 250, y, 200, 200, nFont, "Movespeed", icons[1], sf::Color(155, 173, 183), sf::Color(110, 136, 150));
	options[2] = new upgButton(x + 500, y, 200, 200, nFont, "Damage", icons[2], sf::Color(155, 173, 183), sf::Color(110, 136, 150));

	nextState = 3;
	ID = "upgrade";
}
void upgradeState::render()
{
	window->draw(screenSprite);
	window->draw(menu);
	for (int i = 0; i < 3; i++)
		options[i]->render(window);
}

//GAME OVER
overState::overState(sf::RenderWindow* nWindow, sf::Vector2f nCenter, sf::Font* nFont) : State(nWindow, nCenter, nFont)
{
	//Filter to darken rest of screen
	int width = sf::VideoMode::getDesktopMode().width;
	int height = sf::VideoMode::getDesktopMode().height;
	filter.setSize(sf::Vector2f(width, height));        //Make the filter the size of the screen
	filter.setFillColor(sf::Color(0, 0, 0, 160));       //Set color to a slightly transparent black

	//Screenshotting
	sf::Vector2u size = window->getSize();
	screenshot.create(size.x, size.y);
	screenshot.update(*window);
	screenSprite.setTexture(screenshot);

	//Text
	text.setString("GAME OVER");
	text.setPosition(sf::Vector2f(width / 2 - text.getGlobalBounds().width / 2, height / 5)); //Centered on y axis, 80% of the way up the screen

	//Buttons
	options.resize(2);
	options[0] = new Button(nCenter.x - 200, nCenter.y - 200, 400, 100, font, "Retry", sf::Color(155, 173, 183), sf::Color(110, 136, 150));
	options[1] = new Button(nCenter.x - 200, nCenter.y - 50, 400, 100, font, "Quit", sf::Color(155, 173, 183), sf::Color(110, 136, 150));

	nextState = 2;
	ID = "over";
}
void overState::render()
{
	window->draw(screenSprite);
	window->draw(filter, sf::BlendAlpha);
	window->draw(text);
	for (int i = 0; i < 2; i++)
		options[i]->render(window);
}