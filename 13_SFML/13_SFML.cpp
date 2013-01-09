// 13_SFML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Window.hpp>

int main()
{
	sf::Window windows;

	windows.Create(sf::VideoMode(800, 600, 32), "SFML Window");

	for (int i = 0; i < 5000; i++) {
		windows.Display();
	}
	return EXIT_SUCCESS;
}

