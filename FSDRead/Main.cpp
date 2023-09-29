#include "Compress.h"

#include <SDL_image.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#define _CRT_SECURE_NO_WARNINGS

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* image = NULL;
SDL_Texture* texture = NULL;

/// <summary>
/// Initialize window
/// </summary>
/// <param name="w">Window width</param>
/// <param name="h">Window height</param>
/// <returns></returns>

int InitIMG(int imgflags)
{
	int t = IMG_Init(imgflags);
	if ((t & imgflags) != imgflags)
	{
		std::cout << "Something terrible has happened!\nIMG_Init says: " << IMG_GetError() << '\n';
		return 1;
	}
	return 0;
}

int InitImGui()
{
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	return 0;
}

int InitSDL(int w, int h)
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

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	if (renderer == NULL)
	{
		std::cout << "Failed to create renderer";
		return 1;
	}

	return 0;
}

int InitWindow(int w, int h)
{
	if (InitSDL(w, h))
		return 1;
	InitImGui();
	return 0;
}

void Quit()
{
	SDL_FreeSurface(image);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_Quit();
	IMG_Quit();
}

int main(int argc, char** argv)
{
	std::string path = "";
	bool open = argc > 1; // if argc > 1: open imege immediately; else: wait for user to open

	if (argc > 1)
		path = argv[1];

	if (InitWindow(800, 600))
	{
		std::cout << "Failed to initialize window";
		return 1;
	}

	if (InitIMG(IMG_INIT_PNG | IMG_INIT_JPG))
	{
		std::cout << "SDL_image failed do initiaize successfully. Is says: " << IMG_GetError() << '\n';
		Quit();
		return -1;
	}

	bool quit = false;
	while (!quit)
	{
		// event processing
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			}
		}


		// render image before interface
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		// imgui
		{
			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open"))
					{
						path = getFile();
						open = true;
					}
					if (ImGui::BeginMenu("Save as..."))
					{
						if (ImGui::MenuItem("png"))
						{
							std::string path = getNewFile(".png");
							IMG_SavePNG(image, path.c_str());
						}
						if (ImGui::MenuItem("jpg"))
						{
							std::string path = getNewFile(".jpg");
							int quality = 88;
							IMG_SaveJPG(image, path.c_str(), quality);
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			ImGui::Render();
			ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
			SDL_RenderPresent(renderer);
		}

		if(open)
		{
			char tempfile[100];
			tmpnam_s(tempfile, 100);

			decompress(path, tempfile);
			image = readImage(tempfile);

			SDL_SetWindowSize(window, image->w, image->h);
			texture = SDL_CreateTextureFromSurface(renderer, image);

			open = false;
		}
	}

	
	return 0;
}