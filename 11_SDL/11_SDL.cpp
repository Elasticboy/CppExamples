// 11_SDL.cpp : Defines the entry point for the console application.
//

#include "SDL.h"

int wmain(int argc, char* argv[])
{
	int res = SDL_Init(SDL_INIT_VIDEO);
	SDL_Quit();
	return 0;
}

