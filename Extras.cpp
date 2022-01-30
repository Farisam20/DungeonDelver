
#pragma once
#include "Extras.h"

//Returns index of minimum value in an int array
int getMinIndex(int* arr, int size)
{
	int minIndex = 0;
	for (int i = 1; i < size; i++)
		if (arr[i] < arr[minIndex])
			minIndex = i;
	return minIndex;
}

double pointTowardsCursor(sf::Sprite* model, sf::RenderWindow* window, sf::Sprite* modelTwo)
{
	sf::Vector2i mousePosition;
	if (modelTwo != nullptr)
	{
		mousePosition = sf::Vector2i(modelTwo->getPosition().x, modelTwo->getPosition().y);
	}
	else
	{
		mousePosition = sf::Mouse::getPosition(*window); // Get mouse position relative to window
	}
	sf::Vector2f spritePos = model->getPosition(); // Get sprite position
	sf::Vector2f distance(spritePos.x - mousePosition.x, mousePosition.y - spritePos.y); // Get the distance between sprite and mouse
	double rotation = (atan(distance.x / distance.y) * 180 / PI) - 90;

	if (mousePosition.y > spritePos.y)
	{
		rotation -= 180;
	}
	return rotation;
}

double distanceBetween(sf::Vector2f pos1, sf::Vector2f pos2)
{
	return sqrt(pow((pos1.x - pos2.x), 2) + pow((pos1.y - pos2.y), 2));
}