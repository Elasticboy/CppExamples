// 14_Fmod_SFML_Equalizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>
#include <fmod.hpp>
#include <iostream>

const int g_spectrumSize = 1024;

int g_sampleBegin;
int g_sampleEnd;
int g_sceenWidth;
int g_sceenHeight;
int g_padding;
float g_pas;
float g_pipeWidth;
int g_right;
int g_bottom;
float g_coefInput;
float g_freq;

// Prototypes
void checkFmodResult(FMOD_RESULT result);
void drawSpectrum(sf::RenderWindow* window, float spectrum[]);
float displayedSpectrumValue(float spectrumValue, int position);
//void initParams(int argc, char** argv);

/**
 * Initiate all the variables the programme.
 */
void initParams(int argc, char** argv) {
	g_sceenWidth		= 1200;
	g_sceenHeight		= 800;

	g_sampleBegin		= (argc >= 2) ? std::stoi(argv[1]) : 0;
	if (g_sampleBegin >= g_spectrumSize) {
		std::cout << "WARNING : g_sampleBegin >= g_spectrumSize" << std::endl;
		g_sampleBegin = 0;
	}

	g_sampleEnd			= (argc >= 3) ? std::stoi(argv[2]) : g_spectrumSize - 1;
	if (g_sampleEnd >= g_spectrumSize || g_sampleEnd <= g_sampleBegin) {
		std::cout << "WARNING : g_sampleEnd >= g_spectrumSize || g_sampleEnd <= g_sampleBegin" << std::endl;
		g_sampleEnd = g_spectrumSize - 1;
	}
	
	const float g_coefInputMin		= 0.0f;
	const float g_coefInputMax		= 100000.0f;
	const float g_coefInputDefault	= 50000.0f;
	g_coefInput				= (argc >= 4) ? std::stof(argv[3]) : g_coefInputDefault;
	if (g_coefInput < g_coefInputMin || g_coefInput > g_coefInputMax) {
		std::cout << "WARNING : g_coefInput < g_coefInputMin || g_coefInput > g_coefInputMax" << std::endl;
		g_coefInput = g_coefInputDefault;
	}

	// Padding from the edge of the screen
	g_padding			= 50; 
	// The pas used to draw the pipes
	g_pas				= (float)(g_sceenWidth - g_padding * 2) / (float) (g_sampleEnd - g_sampleBegin);
	if (int (g_pas + 0.4f) > int (g_pas)) {
		g_pas += 0.4f;
	}
	if (g_pas < 1) {
		std::cout << "WARNING : g_pas < 1" << std::endl;
		g_pas = 1;
	}
	// The width of a pipe if 90% of the width of the pas
	g_pipeWidth			= g_pas * 0.9f;
	if (int (g_pipeWidth + 0.4f) > int (g_pipeWidth)) {
		g_pipeWidth += 0.4f;
	}
	if (g_pipeWidth < 1) {
		std::cout << "WARNING : g_pipeWidth < 1" << std::endl;
		g_pipeWidth = 1;
	}
	
	g_right		= g_sceenWidth - g_padding;
	g_bottom	= g_sceenHeight - g_padding;
	g_freq		= 0.025f;
	
	std::cout << "Running Config : " << std::endl;
	std::cout << " - Screen	: " << g_sceenWidth << "x" << g_sceenHeight << std::endl;
	std::cout << " - Spectrum	: " << g_spectrumSize	<< std::endl;
	std::cout << " - SampleBegin	: " << g_sampleBegin	<< std::endl;
	std::cout << " - SampleEnd	: " << g_sampleEnd	<< std::endl;
	std::cout << " - Padding	: " << g_padding		<< std::endl;
	std::cout << " - Pas		: " << g_pas			<< std::endl;
	std::cout << " - PipeWidth	: " << g_pipeWidth		<< std::endl;
	std::cout << " - Bottom	: " << g_bottom << std::endl;
	std::cout << " - Coef		: " << g_coefInput << std::endl;
	std::cout << " - Freq		: " << g_freq << std::endl;
}

int main(int argc, char** argv)
{
	initParams(argc, argv);
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
	window.Create(sf::VideoMode(g_sceenWidth, g_sceenHeight, 32), "SFML Window");
	
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

		if (!isPlaying) {
			result = system->playSound(FMOD_CHANNEL_REUSE, music, false, &channel);
			checkFmodResult(result);
		}

        
		if (isPlaying) {
			if (clock.GetElapsedTime() > g_freq) {
				float spectrum[g_spectrumSize];
				result = channel->getSpectrum(spectrum, g_spectrumSize, 0, FMOD_DSP_FFT_WINDOW_RECT);
				checkFmodResult(result);
				clock.Reset();

				window.Clear(); // Fill with black
				drawSpectrum(&window, spectrum);
				window.Display(); // Refresh the window
				//std::cout << std::endl;
			} else {
				// need regulation
				//std::cout << "can make it smoother : " << clock.GetElapsedTime() * 1000.0f << " ms" << std::endl;
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

/**
 * Draw the acoustic spectrum in the window.
 * @param window The window to draw in.
 * @param spectrum A table containing the acoustic sperctum values.
 */
void drawSpectrum(sf::RenderWindow* window, float spectrum[])
{
	int x1 = g_padding;
	for (int current = g_sampleBegin; current < g_sampleEnd; current++) {
		int x2 = x1 + g_pipeWidth;
		
		float value = displayedSpectrumValue(spectrum[current], current);
		int y2 = g_bottom - value;
		if (y2 < g_padding) {
			y2 = g_padding;
		}

		if (x1 + g_pas > g_right) {
			break;
		}
		//std::cout << value << " | ";
		window->Draw(sf::Shape::Rectangle(x1, g_bottom, x2, y2, sf::Color::Green));
		x1 += g_pas;
	}
	//std::cout << std::endl << std::endl;
}

/**
 * This method get the value returned by fmod and transforms it
 * to get an corret value to display.
 */
float displayedSpectrumValue(float spectrumValue, int position) {
	if (position == 0) { 
		position = 1; // Avoid multiplication by zero
	}
	const float reductionCoef = 1.0f/(float) g_spectrumSize * (float)position + 2;
	// if position == 1 => => returned value = spectrumValue * 2
	// if position == g_spectrumSize (max) => returned value = spectrumValue * g_coefInput * 2

	const float finalCoef = g_coefInput * reductionCoef;
	
	return spectrumValue * finalCoef;
}
