#include "Images.h"
#include "Files.h"

char tempfile[100];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Surface* drawImage = NULL;
SDL_Texture* texture = NULL;

SDL_Surface* image = NULL;


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

int InitSDLWindow(int w, int h)
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cout << "Failed to initialize SDL";
        return 1;
    }

    window = SDL_CreateWindow("Convert to FSD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);

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

int InitImGui()
{
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    return 0;
}

int InitWindow(int w, int h)
{
    if (InitSDLWindow(w, h))
        return 1;
    InitImGui();
    return 0;
}

void Quit(int suspend = 0)
{
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
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
    texture = SDL_CreateTextureFromSurface(renderer, drawImage);
}

void Sep()
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

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
        std::cout << "SDL_image failed do initiaize successfully. Is says: " << IMG_GetError() << '\n';
        Quit(-1);
        return -1;
    }

    if (InitWindow(800, 600))
    {
        std::cout << "Something terrible has just happened! Maybe the rules of universe changed exactry so that SDL library is no longer working, but more likely, some bytes in the Window object failed to arrange themselves as the Programmer wanted.\n";
        std::cout << "In that case, if you will restart the program, all likely shall be well";
        Quit();
        return -3;
    }

    ImGuiIO& io = ImGui::GetIO();

    int init_type = 1;
    int colornum = 8;
    bool quit = false;
    bool colorPanel = false;  // windows open
    bool quantizeDither = true;  // options
    bool pathWarning = false;

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
                // TODO: Add shortcuts from menu items
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
                    if (ImGui::MenuItem("Open", "Ctrl+O"))
                    {
                        path = getFile();
                    }

                    if (ImGui::MenuItem("Save FSD", "Ctrl+S"))
                    {
                        std::string savefolder = getNewFile();
                        tmpnam_s(tempfile, 100);  // create temp filename for intermediate result
                        SaveToFile(drawImage, colors, tempfile);
                        compress(tempfile, savefolder);
                    }

                    if (ImGui::BeginMenu("Save as..."))
                    {
                        if (ImGui::MenuItem("png"))
                        {
                            std::string path = getNewFile(".png");
                            IMG_SavePNG(drawImage, path.c_str());
                        }
                        if (ImGui::MenuItem("jpg"))
                        {
                            std::string path = getNewFile(".jpg");
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
                        colorPanel = true;

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


            if (colorPanel)
            {
                ImGui::Begin("Color panel", &colorPanel);

                ImGui::InputInt("Number of colors", &colornum);

                for (int i = 0; i < colornum; i++)
                {
                    if (i >= colors.size())
                        colors.push_back(std::vector<float>{0, 0, 0});
                    ImGui::ColorEdit3(("color #" + std::to_string(i)).c_str(), &colors[i][0]);
                }

                Sep();


                ImGui::Combo("Quantization init method", &init_type, "Median split (reliable, non-random)\0k - means++ (faster, can give better results but random)\0\0");

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
                    if (quantizeDither)
                        DitherAndDraw(colors);
                }

                if (disable)
                    ImGui::EndDisabled();

                ImGui::Checkbox("Dither after quantizing", &quantizeDither);

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
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(renderer);
        }

        if (path != "")
        {
            if (CheckPath(path))
            {
                image = IMG_Load(path.c_str());
                SDL_SetWindowSize(window, image->w, image->h);
                texture = SDL_CreateTextureFromSurface(renderer, image);
            }
            else
                pathWarning = true;
            path = "";
        }
        
    }
    Quit();
    return 0;
}

