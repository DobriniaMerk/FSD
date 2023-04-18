#include <SDL.h>

#include "Compress.cpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <windows.h>
#include <string.h>

// zpaq
#include "libzpaq.h"
#include <stdio.h>
#include <stdlib.h>
// zpaq

#define _CRT_SECURE_NO_WARNINGS

SDL_Window* window = NULL;
SDL_Renderer* render = NULL;

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

	window = SDL_CreateWindow("FSD Reader", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		std::cout << "Failed to create window";
		return 1;
	}

	render = SDL_CreateRenderer(window, -1, 0);

	return 0;
}

int main(int argc, char** argv)
{
	std::wstring str;

	int windowx = 800, windowy = 600;  // TODO: set based on image size

	if (InitWindow(windowx, windowy))
	{
		std::cout << "Failed to initialize window";
		return 1;
	}

	SDL_Surface* image = Compress::getImage();
	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, image);

	SDL_RenderCopy(render, texture, NULL, NULL);
	SDL_UpdateWindowSurface(window); // after all drawing is done

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
			case SDL_WINDOWEVENT_RESIZED:
				SDL_RenderClear(render);
				SDL_RenderCopy(render, texture, NULL, NULL);
				SDL_UpdateWindowSurface(window); // after all drawing is done
				break;
			}
		}
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}