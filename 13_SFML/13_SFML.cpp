// 13_SFML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

void drawRandom(sf::RenderWindow& window);

int main()
{
	sf::RenderWindow window;
	
	window.Create(sf::VideoMode(800, 600, 32), "SFML Window");
	
	// Main loop
	while (window.IsOpened()) {

        sf::Event Event;
        while (window.GetEvent(Event)) {

            if (Event.Type == sf::Event::Closed)
                window.Close();
        }

		window.Clear(sf::Color::White); // Fill with black

		drawRandom(window);
		window.Draw(sf::Shape::Rectangle(50, 50, 150, 150, sf::Color::Green));
		window.Draw(sf::Shape::Rectangle(150, 300, 50, 200, sf::Color::Blue));
        window.Display(); // Refresh the window
    }

    return EXIT_SUCCESS;
}


void drawRandom(sf::RenderWindow& window)
{
	const int bottom = 790;

	const int length = 128;
	const int pipeWidth = 8;
	const int pas = 10;
	
	for (int i = 0; i < length; i++) {
		int x1 = i * pas;
		int x2 = x1 + pipeWidth;
		int spectrumValue = bottom - sf::Randomizer::Random(0, i*10);

		window.Draw(sf::Shape::Rectangle(x1, bottom, x2, spectrumValue, sf::Color::Green));
	}
}