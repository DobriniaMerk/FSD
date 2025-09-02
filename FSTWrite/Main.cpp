#include "Images.hpp"
#include "Files.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstddef>
#include <iostream>

char tempfile[200];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Surface* drawImage = NULL;
SDL_Texture* texture = NULL;

SDL_Surface* image = NULL;


int InitIMG(int imgflags)
{
    int t = IMG_Init(imgflags);
    if ((t & imgflags) != imgflags)
    {
        std::cerr << "Something terrible has happened!\nIMG_Init says: " << IMG_GetError() << '\n';
        return 1;
    }
    return 0;
}

int InitSDLWindow(int w, int h)
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cerr << "Failed to initialize SDL";
        return 1;
    }

    window = SDL_CreateWindow("Convert to FSD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        std::cerr << "Failed to create window\n";
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        std::cerr << "Failed to create renderer\n";
        return 1;
    }

    return 0;
}

void InitImGui()
{
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

int InitWindow(int w, int h)
{
    if (InitSDLWindow(w, h))
    {
        std::cerr << "Something terrible has just happened! Maybe the rules of universe changed exactry so that SDL library is no longer working, but more likely some bytes in the Window object failed to arrange themselves as the Programmer wanted.\nIn that case, if you restart the program, all likely shall be well\n";
        return 1;
    }

    InitImGui();
    return 0;
}

void Quit(bool suspend = false)
{
    SDL_DestroyWindow(window);
    SDL_FreeSurface(drawImage);
    SDL_DestroyTexture(texture);

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_Quit();
    IMG_Quit();

    if (suspend)
      system("pause");
}

void DitherAndDraw(std::vector<std::vector<float>> colors)
{
    drawImage = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, drawImage, NULL);
    Dither(drawImage, colors);
    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, drawImage);
}

void Sep()
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

// checks for non-standard ASCII symbols in path
bool CheckPath(std::string path)
{
    for (int i = 0; i < path.length(); i++)
    {
        unsigned char c = char(path[i]);
        if (c > 127)
            return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    std::string path = "";
    if (argc > 1)
        path = argv[1];

    if (InitIMG(IMG_INIT_PNG | IMG_INIT_JPG))
    {
        Quit();
        return -1;
    }

    if (InitWindow(800, 600))
    {
        Quit();
        return -3;
    }

    ImGuiIO& io = ImGui::GetIO();

    int init_type = 1;
    int colornum = 8;
    bool quit = false;
    bool colorPanelOpen = false;
    bool ditherAfterQuantize = true;
    bool pathWarning = false;  // possibly not needed

    std::vector<std::vector<float>> colors(colornum, std::vector<float>(3, 0));

    while (!quit)
    {
        // event processing
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);

            switch (e.type)
            {
                // TODO: Add shortcuts for menu items
            case SDL_QUIT:
                quit = true;
                break;
            }
        }


        // render image before interface
        if (path != "")
        {
            // if (CheckPath(path))
            // {
            //     // for problems with non-ASCII paths
            // }
            SDL_FreeSurface(image);
            image = IMG_Load(path.c_str());
            SDL_SetWindowSize(window, image->w, image->h);
            if (texture)
              SDL_DestroyTexture(texture);
            texture = SDL_CreateTextureFromSurface(renderer, image);
            path = "";
        }

        SDL_RenderClear(renderer);

        if (texture)
        {
            SDL_RenderSetViewport(renderer, NULL);
            SDL_RenderSetLogicalSize(renderer, 0, 0); // disable logical scaling
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            SDL_Rect destrect = {0, 0, w, h};
            SDL_RenderCopy(renderer, texture, nullptr, &destrect);
        }

        // imgui
        {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open", "Ctrl+O"))
                    {
                        path = getImageFile();
                    }

                    if (ImGui::MenuItem("Save FSD", "Ctrl+S"))
                    {
                        std::string savefolder = saveFile();
                        tmpnam(tempfile);  // create temp filename for intermediate result
                        SaveToFile(drawImage, colors, tempfile);
                        compress(tempfile, savefolder);
                    }

                    if (ImGui::BeginMenu("Save as..."))
                    {
                        if (ImGui::MenuItem("png"))
                        {
                            std::string path = saveFile(".png");
                            IMG_SavePNG(drawImage, path.c_str());
                        }
                        if (ImGui::MenuItem("jpg"))
                        {
                            std::string path = saveFile(".jpg");
                            int quality = 88;
                            IMG_SaveJPG(drawImage, path.c_str(), quality);
                        }
                        ImGui::EndMenu();
                    }

                    ImGui::EndMenu();
                }

                // color management menu
                if (ImGui::BeginMenu("Colors"))
                {
                    if (ImGui::MenuItem("Manage"))
                        colorPanelOpen = true;

                    // TODO: implement saving and loading presets from file
                    if (ImGui::BeginMenu("Presets"))
                    {
                        if (ImGui::MenuItem("None"))
                        {

                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Dither!", "Ctrl+D"))
                {
                    DitherAndDraw(colors);
                }

                ImGui::EndMainMenuBar();
            }


            if (colorPanelOpen)
            {
                ImGui::Begin("Color panel", &colorPanelOpen);

                ImGui::InputInt("Number of colors", &colornum);

                for (int i = 0; i < colornum; i++)
                {
                    if (i >= colors.size())
                        colors.push_back(std::vector<float>{0, 0, 0});
                    ImGui::ColorEdit3(("color #" + std::to_string(i)).c_str(), &colors[i][0]);
                }

                Sep();


                ImGui::Combo("Quantization init method", &init_type, "Median split (consistent, non-random)\0k - means++ (faster, can give better results but random)\0\0");

                // QuantizeMedian method doesnt support numbers other than power of two
                bool disable = false;
                if (colornum & (colornum - 1) && !init_type || image == NULL)
                {
                    ImGui::Text(image == NULL ? "No image yet selected" : "Median split method only works if number of colors is a power of 2");
                    ImGui::BeginDisabled();
                    disable = true;
                }

                if (ImGui::Button("Quantize", ImVec2(ImGui::GetWindowSize().x - 20, 60)))
                {
                    colors = Quantize(image, colornum, init_type);
                    if (ditherAfterQuantize)
                        DitherAndDraw(colors);
                }

                if (disable)
                    ImGui::EndDisabled();

                ImGui::Checkbox("Dither after quantizing", &ditherAfterQuantize);

                ImGui::End();
            }

            /*if (pathWarning)
            {
                ImGui::OpenPopup()
            }
            if (ImGui::BeginPopupModal("Invalid file path", &pathWarning))
            {
                ImGui::Text("Regrettably the current implementation of file opening does not support characters other than ASCII");
                ImGui::EndPopup();
            }*/

            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        }

        SDL_RenderPresent(renderer);
    }
    Quit();
    return 0;
}
