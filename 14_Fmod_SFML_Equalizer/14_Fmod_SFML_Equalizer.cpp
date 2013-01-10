// 14_Fmod_SFML_Equalizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>
#include <fmod.hpp>
#include <iostream>

const int spectrumSize		= 1024;
const int spectrumSample	= 100;

const int sceenWidth = 1200;
const int sceenHeight = 800;
const int padding	= 50;

const float pas		= (float)(sceenWidth - padding * 2) / (float)spectrumSample;
const float pipeWidth	= pas * 0.9f;

const int bottom	= sceenHeight - padding;

const float coef = 10000;
const float freq = 0.025f;

// Prototypes
void checkFmodResult(FMOD_RESULT result);
void drawSpectrum(sf::RenderWindow* window, float spectrum[spectrumSize]);

int main()
{
	////////////////////////////////
	// Init FMOD 
	FMOD_RESULT result;

	FMOD::System* system	= nullptr;
	result = System_Create(&system);
	checkFmodResult(result);
	result = system->init(2, FMOD_INIT_NORMAL, nullptr);
	checkFmodResult(result);

	FMOD::Sound* music		= nullptr;
	result = system->createSound("music.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, nullptr, &music);
	checkFmodResult(result);

	FMOD::Channel* channel	= nullptr;
	result = system->getChannel(0, &channel);
	checkFmodResult(result);

	//////////////////////////////////
	// Init SFML
	sf::RenderWindow window;
	window.Create(sf::VideoMode(sceenWidth, sceenHeight, 32), "SFML Window");
	
	///////////////////////////////////
	sf::Clock clock;
	
	// Main loop
	while (window.IsOpened()) {

        sf::Event Event;
        while (window.GetEvent(Event)) {

            if (Event.Type == sf::Event::Closed)
                window.Close();
        }
		
		bool isPlaying;
		channel->isPlaying(&isPlaying);

		if (!isPlaying && clock.GetElapsedTime() > 3) { // play after 3 seconds 
			result = system->playSound(FMOD_CHANNEL_REUSE, music, false, &channel);
			checkFmodResult(result);
		}

        
		if (isPlaying) {
			if (clock.GetElapsedTime() > freq) {
				float spectrum[spectrumSize];
				result = channel->getSpectrum(spectrum, spectrumSize, 0, FMOD_DSP_FFT_WINDOW_RECT);
				checkFmodResult(result);
				clock.Reset();

				window.Clear(); // Fill with black
				drawSpectrum(&window, spectrum);
				window.Display(); // Refresh the window
				std::cout << std::endl;
			} else {
				// need regulation
				std::cout << "can make it smoother : " << clock.GetElapsedTime() * 1000.0f << " ms" << std::endl;
			}
		}

    }

	// Release resources
	result = music->release();
	checkFmodResult(result);

	result = system->close();
	checkFmodResult(result);

	result = system->release();
	checkFmodResult(result);
    return EXIT_SUCCESS;
}

void checkFmodResult(FMOD_RESULT result)
{
	if (result != FMOD_RESULT::FMOD_OK) {
		std::cerr << "Error : " << result << std::endl;
	}
}

void drawSpectrum(sf::RenderWindow* window, float spectrum[spectrumSize])
{
	for (int i = 0; i < spectrumSample; i++) {
		int x1 = i  * pas + padding;
		int x2 = x1 + pipeWidth;

		float value = spectrum[i] * coef;
		int y2 = bottom - value;
		if (y2 < padding) {
			y2 = padding;
		}

		//std::cout << value << " | ";
		window->Draw(sf::Shape::Rectangle(x1, bottom, x2, y2, sf::Color::Green));
	}
	//std::cout << std::endl << std::endl;
}
