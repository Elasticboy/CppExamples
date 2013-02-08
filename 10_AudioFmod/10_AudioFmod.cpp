// 10_AudioFmod.cpp : Defines the entry point for the console application.
//

#include "fmod.hpp"
#include <Windows.h>
#include <iostream>

using namespace std;

// Prototypes
void checkFmodResult(FMOD_RESULT result);
FMOD::System* initFmodSystem();
void releaseFmodSystem(FMOD::System* system);
void playShortSound();
void playLongSound();
void spectrumFromFile();
void spectrumFromSoundBoard();
void displaySpectrum(float* spectrum, int spectrumSize);
void displaySpectrumSample(float* spectrum, int spectrumSize);

int main()
{
	cout << "playShortSound()" << endl;
	playShortSound();

	cout << "playLongSound()" << endl;
	playLongSound();
	
	cout << "spectrumFromFile()" << endl;
	spectrumFromFile();

	cout << "spectrumFromSoundBoard()" << endl;
	spectrumFromSoundBoard();

	return EXIT_SUCCESS;
}

void checkFmodResult(FMOD_RESULT result)
{
	if (result != FMOD_RESULT::FMOD_OK) {
		cerr << "Error : " << result << endl;
	}
}

/**
 * Init FMOD library.
 */
FMOD::System* initFmodSystem()
{
	FMOD::System* system;
	FMOD_RESULT result;
	
	result = System_Create(&system);
	checkFmodResult(result);

	result = system->init(2, FMOD_INIT_NORMAL, nullptr);
	checkFmodResult(result);

	return system;
}

/**
 * Release FMOD library.
 */
void releaseFmodSystem(FMOD::System* system)
{
	FMOD_RESULT result;
	
	result = system->close();
	checkFmodResult(result);

	result = system->release();
	checkFmodResult(result);
}

/**
 * Play a short sound on an available channel.
 */
void playShortSound()
{	
	FMOD::System* system = initFmodSystem();

	FMOD::Sound* gunSound = nullptr;
	FMOD_RESULT result;
	
	result = system->createSound("gun.wav", FMOD_CREATESAMPLE, nullptr, &gunSound);
	checkFmodResult(result);

	result = system->playSound(FMOD_CHANNEL_FREE, gunSound, false, nullptr);
	checkFmodResult(result);

	Sleep(1000);
	result = gunSound->release();
	checkFmodResult(result);

	releaseFmodSystem(system);
}

/**
 * Play a long sound on an available channel.
 */
void playLongSound()
{
	FMOD::System* system = initFmodSystem();

	FMOD::Sound* music = nullptr;
	FMOD_RESULT result;
	
	result = system->createSound("DieWalkure.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, nullptr, &music);
	checkFmodResult(result);

	result = system->playSound(FMOD_CHANNEL_FREE, music, false, nullptr);
	checkFmodResult(result);

	Sleep(5000);
	result = music->release();
	checkFmodResult(result);

	releaseFmodSystem(system);
}

/**
 * Get and display an acoustic spectrum from a file.
 */
void spectrumFromFile()
{
	FMOD::System* system = initFmodSystem();

	FMOD::Channel* channel;
	FMOD_RESULT result;
	
	result = system->getChannel(0, &channel);
	checkFmodResult(result);

	FMOD::Sound* music = nullptr;
	result = system->createSound("DieWalkure.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, nullptr, &music);
	checkFmodResult(result);

	result = system->playSound(FMOD_CHANNEL_REUSE, music, false, &channel);
	checkFmodResult(result);

	const int spectrumSize = 512;
	float spectrum[spectrumSize];

	for (int i = 0; i < 100; i++) {

		result = channel->getSpectrum(spectrum, spectrumSize, 0, FMOD_DSP_FFT_WINDOW_RECT);
		checkFmodResult(result);
		
		//displaySpectrum(spectrum, spectrumSize);
		displaySpectrumSample(spectrum, spectrumSize);
	}
	
	result = music->release();
	checkFmodResult(result);

	releaseFmodSystem(system);
}

/**
 * Get and display an acoustic spectrum directly from a the soundBoard.
 */
void spectrumFromSoundBoard()
{
	FMOD::System* system = initFmodSystem();

	// TODO

	releaseFmodSystem(system);
}

void displaySpectrum(float* spectrum, int spectrumSize)
{
	// display spectrumValue
	for (int spectrumValue = 0; spectrumValue < spectrumSize; spectrumValue++) {
		cout << spectrum[spectrumValue] << " | ";
	}
	cout << endl << endl;
	Sleep(50);
}

void displaySpectrumSample(float* spectrum, int spectrumSize)
{
	// display spectrumValue
	int spectrumValue = 0;
	
	while (spectrumValue < spectrumSize) { // display 1 value for 50
		cout << spectrum[spectrumValue] << " | ";
		spectrumValue += 50;
	}
	cout << endl << endl;
	Sleep(50);
}
