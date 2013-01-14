// 14_Fmod_SFML_Equalizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <string>

using namespace std;

class Pointf {
public:
	float x;
	float y;
	Pointf() {};
	Pointf(float _x, float _y) : x(_x), y(_y) {}
};

class Rectf {
public:
	float left;
	float top;
	float right;
	float bottom;
	
	float width()	{ return right	- left; }
	float height()	{ return bottom	- top; }
	float centerX()	{ return left	+ width()	/ 2.0f; }
	float centerY()	{ return top	+ height()	/ 2.0f; }
	Pointf center()	{ return Pointf(centerX(), centerY()); }
};

const string NORMAL		= "NORMAL";
const string REVERSE	= "REVERSE";
const string CENTER		= "CENTER";
const string STEREO		= "STEREO";
const string CENTER_STEREO = "CENTER_STEREO";

const int STEREO_LEFT	= 0;
int STEREO_RIGHT		= 1;

FMOD_DSP_FFT_WINDOW g_modeAlgo;
sf::Color g_modeColor;

// Screen Size
Rectf g_screenRect;

// Padding of the drawing rectangle
float g_drawingRectPadding;

// Drawing rectangle
Rectf g_drawingRect;

// Spectrum Informations
string g_spectrumType;
const int g_spectrumSize = 1024;
int g_sampleBegin;
int g_sampleEnd;
int g_step;
int g_pipeWidth;
float g_coefInput;
float g_freq;

// Prototypes
void initParams(int argc, char** argv);
void checkFmodResult(FMOD_RESULT result, const string& source);
void correctRangeX(Rectf rect, float &value);
void correctRangeY(Rectf rect, float &value);
void computeAndDrawSpectrum(sf::RenderWindow& window, sf::Clock& clock, FMOD::Channel *channel);
void drawSpectrum(sf::RenderWindow& window, float spectrum[], Pointf startingPoint, bool centerX, bool reverse);
float displayedSpectrumValue(float spectrumValue, int position);
void setNextMode(sf::Clock& clock);

int main(int argc, char** argv)
{
	initParams(argc, argv);
	////////////////////////////////
	// Init FMOD 
	FMOD_RESULT result;

	FMOD::System* system	= nullptr;
	result = System_Create(&system);
	checkFmodResult(result, "main");

	FMOD_SPEAKERMODE  speakermode;
	result = system->getDriverCaps(0, 0, 0, &speakermode);
    checkFmodResult(result, "main");

    result = system->setSpeakerMode(speakermode);
    checkFmodResult(result, "main");

	result = system->init(2, FMOD_INIT_NORMAL, nullptr);
	checkFmodResult(result, "main");

	FMOD::Sound* music		= nullptr;
	result = system->createSound("music.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, nullptr, &music);
	checkFmodResult(result, "main");

	FMOD::Channel* channel	= nullptr;
	result = system->getChannel(0, &channel);
	checkFmodResult(result, "main");

	//////////////////////////////////
	// Init SFML
	sf::RenderWindow window;
	window.Create(sf::VideoMode((int)g_screenRect.width(), (int)g_screenRect.height(), 32), "Spectrum : " + g_spectrumType);
	
	///////////////////////////////////
	sf::Clock clock;
	sf::Clock changeModeClock;

	// Main loop
	while (window.IsOpened()) {

		bool isPlaying;
		channel->isPlaying(&isPlaying);

		if (!isPlaying) { // Play the sound if not playing
			result = system->playSound(FMOD_CHANNEL_REUSE, music, false, &channel);
			checkFmodResult(result, "main (system->playSound)");

			int channels;
			music->getFormat(nullptr, nullptr, &channels, nullptr);
			if (channels < 2) { // duplicate mono sound if only one channel is available
				STEREO_RIGHT = STEREO_LEFT;
			}

			result = channel->setSpeakerMix(1.0, 1.0, 1.0f, 0, 0, 0, 0, 0);
			checkFmodResult(result, "main (channel->setSpeakerMix)");
		}

		// Check for events
        sf::Event event;
        while (window.GetEvent(event)) {

            if (event.Type == sf::Event::Closed) {
				// Close button to stop the programm
                window.Close();

			} else if (event.Type == sf::Event::KeyPressed) {
				// Pause/resume if any key is pressed
				bool isPaused;
				channel->getPaused(&isPaused);
				channel->setPaused(!isPaused);
			}
        }

		bool isPaused;
		channel->isPlaying(&isPlaying);
		channel->getPaused(&isPaused);
		if (isPlaying && !isPaused) {
			// Change mode every 5 seconds
			if (changeModeClock.GetElapsedTime() > 5) {
				setNextMode(changeModeClock);
			}
			computeAndDrawSpectrum(window, clock, channel);
		}
    }

	// Release resources
	result = music->release();
	checkFmodResult(result, "main");

	result = system->close();
	checkFmodResult(result, "main");

	result = system->release();
	checkFmodResult(result, "main");
    return EXIT_SUCCESS;
}

/**
 * Initiate the variables.
 */
void initParams(int argc, char** argv)
{
	// Sizes and positions
	g_screenRect.left	= 0.0f;
	g_screenRect.top	= 0.0f;
	g_screenRect.right	= 1200.0f;
	g_screenRect.bottom	= 800.0f;
	
	// Padding from the edge of the screen
	g_drawingRectPadding			= 50;

	// Drawing Rect
	g_drawingRect.left		= g_drawingRectPadding;
	g_drawingRect.top		= g_drawingRectPadding;
	g_drawingRect.right		= g_screenRect.right - g_drawingRectPadding;
	g_drawingRect.bottom	= g_screenRect.bottom - g_drawingRectPadding;

	// Spectrum
	
	// *** Arg 0 => Programme name
	// *** Arg 1 => Render
	string displayType = (argc > 1) ? argv[1] : NORMAL;
	if (displayType == CENTER) {
		g_spectrumType = CENTER;

	} else if (displayType == REVERSE) {
		g_spectrumType = REVERSE;

	} else if (displayType == STEREO) {
		g_spectrumType = STEREO;

	} else if (displayType == CENTER_STEREO) {
		g_spectrumType = CENTER_STEREO;

	} else {
		g_spectrumType = NORMAL;
	}

	// *** Arg 2 => Begin of the range of the spectrum to display
	g_sampleBegin = (argc > 2) ? stoi(argv[2]) : 0;
	if (g_sampleBegin >= g_spectrumSize) {
		cout << "WARNING : g_sampleBegin >= g_spectrumSize" << endl;
		g_sampleBegin = 0;
	}
	
	// *** Arg 3 => End of the range of the spectrum to display
	g_sampleEnd= (argc > 3) ? stoi(argv[3]) : g_spectrumSize - 1;
	if (g_sampleEnd >= g_spectrumSize || g_sampleEnd <= g_sampleBegin) {
		cout << "WARNING : g_sampleEnd >= g_spectrumSize || g_sampleEnd <= g_sampleBegin" << endl;
		g_sampleEnd = g_spectrumSize - 1;
	}
	
	// *** Arg 4 => Coef
	const float g_coefInputMin		= 1.0f;
	const float g_coefInputMax		= 100000.0f;
	const float g_coefInputDefault	= 100.0f;
	g_coefInput = (argc > 4) ? stof(argv[4]) : g_coefInputDefault;
	if (g_coefInput < g_coefInputMin || g_coefInput > g_coefInputMax) {
		cout << "WARNING : g_coefInput < g_coefInputMin || g_coefInput > g_coefInputMax" << endl;
		g_coefInput = g_coefInputDefault;
	}

	// The step used to draw the pipes
	float tempStep = g_screenRect.width() / (float) (g_sampleEnd - g_sampleBegin);
	if (int (tempStep + 0.4f) > (int) tempStep) {
		tempStep += 0.4f;
	}
	g_step = tempStep;
	if (g_step < 1) {
		cout << "WARNING : g_step < 1" << endl;
		g_step = 1;
	}

	// The width of a pipe if 90% of the width of the step
	float tempWidth = (float) g_step * 0.9f;
	if (int (tempWidth + 0.4f) > (int) tempWidth) {
		tempWidth += 0.4f;
	}
	g_pipeWidth = tempWidth;
	if (g_pipeWidth < 1) {
		cout << "WARNING : g_pipeWidth < 1" << endl;
		g_pipeWidth = 1;
	}

	g_freq = 0.025f;
	
	g_modeAlgo = FMOD_DSP_FFT_WINDOW_RECT;
	g_modeColor = sf::Color::Red;
	
	// Display config
	cout << endl;
	cout << "Running Config : " << endl << endl;

	cout << " - Screen		: " << g_screenRect.width() << "x" << g_screenRect.height() << endl;
	cout << " - DrawingRect left	: " << g_drawingRect.left << endl;
	cout << " - DrawingRect top	: " << g_drawingRect.top << endl;
	cout << " - DrawingRect right	: " << g_drawingRect.right << endl;
	cout << " - DrawingRect Bottom	: " << g_drawingRect.bottom << endl << endl;

	cout << " - SpectrumType		: " << g_spectrumType	<< endl;
	cout << " - Spectrum		: " << g_spectrumSize	<< endl;
	cout << " - SampleBegin		: " << g_sampleBegin	<< endl;
	cout << " - SampleEnd		: " << g_sampleEnd	<< endl;
	cout << " - DrawingRect Padding	: " << g_drawingRectPadding		<< endl;
	cout << " - Drawing step		: " << g_step			<< endl;
	cout << " - PipeWidth		: " << g_pipeWidth		<< endl;
	cout << " - Coef			: " << g_coefInput << endl;
	cout << " - Freq			: " << g_freq << endl;
}

void checkFmodResult(FMOD_RESULT result, const string& source)
{
	if (result != FMOD_RESULT::FMOD_OK) {
		cerr << "Error in the function " << source << " : " << FMOD_ErrorString(result) << endl;
		system("PAUSE");
	}
}

bool correctRangeX(Rectf rect, int value)
{
	float left, right;

	// flip x if necessary
	if (rect.left <= rect.right) { // normal
		left = rect.left;
		right = rect.right;
	} else { // need flip
		left = rect.right;
		right = rect.left;
	}
	// Check and correct x
	if (value < left) {
		value = left;
		return true;
	} else if (value > right) {
		value = right;
		return true;
	}

	return false; // no correction done
}

bool correctRangeY(Rectf rect, int value)
{
	float top, bottom;

	// flip y if necessary (SFML origin is top left of the screen so bottom if bigger than top in rectangles)
	if (rect.bottom <= rect.top) { // normal
		top = rect.top;
		bottom = rect.bottom;
	} else { // need flip
		top = rect.bottom;
		bottom = rect.top;
	}

	// Check and correct y
	if (value < bottom) {
		value = bottom;
		return true;
	} else if (value > top) {
		value = top;
		return true;
	}

	return false; // no correction done
}

void computeAndDrawSpectrum(sf::RenderWindow& window, sf::Clock& clock, FMOD::Channel *channel)
{
	if (clock.GetElapsedTime() < g_freq) {
		// getSpectrum refreshes only every 25 ms. No need to redraw if the spectrum hasn't changed.
		return;
	}

	float spectrumLeft[g_spectrumSize];
	FMOD_RESULT result = channel->getSpectrum(spectrumLeft, g_spectrumSize, STEREO_LEFT, g_modeAlgo);
	checkFmodResult(result, "computeAndDrawSpectrum (channel->getSpectrum() LEFT)");

	float spectrumRight[g_spectrumSize];
	if (g_spectrumType == STEREO || g_spectrumType == CENTER_STEREO) {
		FMOD_RESULT result = channel->getSpectrum(spectrumRight, g_spectrumSize, STEREO_RIGHT, g_modeAlgo);
		checkFmodResult(result, "computeAndDrawSpectrum (channel->getSpectrum() RIGHT)");
	}

	clock.Reset();

	window.Clear(); // Fill with black

	if (g_spectrumType == NORMAL) {
		Pointf point;
		point.x = g_drawingRect.left;	// Starting from the left of the drawing rect.
		point.y = g_drawingRect.bottom;	// Starting from the bottom of the drawing rect.
		drawSpectrum(window, spectrumLeft, point, false, false);
			
	} else if (g_spectrumType == REVERSE) {
		Pointf point;
		point.x = g_drawingRect.right;	// Starting from the right of the drawing rect.
		point.y = g_drawingRect.bottom;	// Starting from the bottom of the drawing rect.
		drawSpectrum(window, spectrumLeft, point, false, true);

	} else if (g_spectrumType == CENTER) {
		Pointf point;
		point.x = g_drawingRect.left;	// Starting from the left of the drawing rect.
		point.y = g_drawingRect.centerY();	// Starting from the bottom of the drawing rect.
		drawSpectrum(window, spectrumLeft, point, true, false);

	} else if (g_spectrumType == STEREO) {
		Pointf point;
		point.x = g_drawingRect.centerX(); // Starting from the center of the drawing rect.
		point.y = g_drawingRect.bottom;
		drawSpectrum(window, spectrumRight, point, false, false);
		drawSpectrum(window, spectrumLeft, point, false, true);

	} else if (g_spectrumType == CENTER_STEREO) {
		Pointf point = g_drawingRect.center(); // Starting from the center of the drawing rect.
		drawSpectrum(window, spectrumRight, point, true, false);
		drawSpectrum(window, spectrumLeft, point, true, true);
	}

	window.Display(); // Refresh the window
}

/**
 * Draw the acoustic spectrum in the window.
 * @param window : The window to draw in.
 * @param spectrum : A table containing the acoustic sperctum values.
 * @param startingPoint : The point from where we start to draw the spectrum.
 * @param centerX : If centerX is set to true, the spectrum is center on the X axis. Otherwise, it sticks on the bottom of the screen.
 * @param reverse : If reverse is set to true, the spectrum is drawn from right to left. Otherwise, it is drawn from left to right.
 */
void drawSpectrum(sf::RenderWindow& window, float spectrum[], Pointf startingPoint, bool centerX, bool reverse)
{
	int x1 = startingPoint.x;
	int y1 = startingPoint.y;
	int x2;
	int y2;

	for (int current = g_sampleBegin; current < g_sampleEnd; current++) {
		x2 = (reverse) ? x1 - g_pipeWidth : x1 + g_pipeWidth;
		
		float value = displayedSpectrumValue(spectrum[current], current);
		if (centerX) {
			y1 = startingPoint.y + value / 2.0f;
			y2 = startingPoint.y - value / 2.0f;
			// Stay in the drawing rectangle
			correctRangeY(g_drawingRect, y1);
			correctRangeY(g_drawingRect, y2);
		} else {
			y2 = startingPoint.y - value;
			// Stay in the drawing rectangle
			correctRangeY(g_drawingRect, y2);
		}

		window.Draw(sf::Shape::Rectangle(x1, y1, x2, y2, g_modeColor));
		if (reverse) { x1 -= g_step; } else { x1 += g_step; }

		// Break if the next step is out of the drawing rectangle
		if (correctRangeX(g_drawingRect, x1)) {
			break;
		}
	}
}

/**
 * This method transform the value returned by fmod to obtain an usable value to display.
 */
float displayedSpectrumValue(float spectrumValue, int position)
{
	if (position == 0) { 
		position = 1; // Avoid multiplication by zero
	}
	const float reductionCoef = (float) position / (float) g_spectrumSize;

	float finalCoef = g_coefInput * reductionCoef;
	if (finalCoef < 1) {
		finalCoef = 1;
	}

	// if position == 0		=> returned value = spectrumValue * 4
	// if position == max/2	=> returned value = spectrumValue * g_coefInput/2 * 4
	// if position == max	=> returned value = spectrumValue * g_coefInput * 4
	return spectrumValue * finalCoef * 2000;
	//return spectrumValue * 10000;
}

void setNextMode(sf::Clock& clock) {
	if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_BLACKMAN) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS;
		g_modeColor = sf::Color::Blue;

	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_FORCEINT;
		g_modeColor = sf::Color::Green;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_FORCEINT) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_HAMMING;
		g_modeColor = sf::Color::Cyan;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_HAMMING) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_HANNING;
		g_modeColor = sf::Color::Magenta;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_HANNING) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_MAX;
		g_modeColor = sf::Color::Magenta;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_MAX) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_RECT;
		g_modeColor = sf::Color::Red;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_RECT) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_TRIANGLE;
		g_modeColor = sf::Color::Yellow;
	
	} else if (g_modeAlgo == FMOD_DSP_FFT_WINDOW_TRIANGLE) {
		g_modeAlgo = FMOD_DSP_FFT_WINDOW_BLACKMAN;
		g_modeColor = sf::Color::White;
	
	}
	clock.Reset();
}
