//#include <SDL.h>
#include "Compress.h"

#define _CRT_SECURE_NO_WARNINGS

SDL_Window* window = NULL;
SDL_Surface* windowSurface = NULL;

/// <summary>
/// Initialize window
/// </summary>
/// <param name="w">Window width</param>
/// <param name="h">Window height</param>
/// <returns></returns>
int InitWindow(int w, int h)
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		std::cout << "Failed to initialize SDL";
		return 1;
	}

	window = SDL_CreateWindow("FSD Reader", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		std::cout << "Failed to create window";
		return 1;
	}

	SDL_SetWindowMinimumSize(window, w, h);

	windowSurface = SDL_GetWindowSurface(window);

	if (windowSurface == NULL)
	{
		std::cout << "Failed to get window surface";
		return 1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	std::wstring str;

	SDL_Surface* image = Compress::getImage();
	int windowx = image->w, windowy = image->h;

	if (InitWindow(windowx, windowy))
	{
		std::cout << "Failed to initialize window";
		return 1;
	}

	SDL_BlitSurface(image, NULL, windowSurface, NULL);
	SDL_UpdateWindowSurface(window);

	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:  // currently not working as intended
				SDL_BlitSurface(image, NULL, windowSurface, NULL);
				SDL_UpdateWindowSurface(window);
				break;
			}
		}
	}

	SDL_FreeSurface(image);
	SDL_FreeSurface(windowSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}