#include "Images.h"
#include "Files.h"

char tempfile[100];

SDL_Window* window = NULL;
SDL_Surface* windowSurface = NULL;
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

int InitWindow(int w, int h)
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cout << "Failed to initialize SDL";
        return 1;
    }

    window = SDL_CreateWindow("Convert to FSD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

void Quit(int suspend = 0)
{
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    SDL_Quit();
    IMG_Quit();
    if (suspend)
        system("pause");
}

int main(int argc, char** argv)
{
    std::string path;
    if (argc > 1)
    {
        path = argv[1];
    }
    else
    {
        path = getFile();
    }

    if (InitIMG(IMG_INIT_PNG | IMG_INIT_JPG))
    {
        std::cout << "SDL_image failed do initiaize successfully. Is says: " << IMG_GetError() << '\n';
        Quit(-1);
        return -1;
    }
    image = IMG_Load(path.c_str());
    if (image == NULL)
    {
        std::cout << "Either your image is not good enough to load it, or the SDL is somehow broken.\nIt says: " << IMG_GetError() << '\n';
        Quit(-1);
        return -2;
    }

    SDL_Surface* img = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, img, NULL);

    if (InitWindow(img->w, img->h))
    {
        std::cout << "Something terrible has just happened! Maybe the rules of universe changed exactry so that SDL library is no longer working, but more likely, some bytes in the Window object failed to arrange themselves as the Programmer wanted.\n";
        std::cout << "In that case, if you will restart the program, all likely shall be well";
        Quit();
        return -3;
    }

    SDL_BlitSurface(img, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(window);

    int colornum;
    std::cout << "Input number of colors (must be a power of 2 and no more than 256): ";
    std::cin >> colornum;

    std::vector<SDL_Color> colors = Dither(img, colornum);

    SDL_BlitSurface(img, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(window);

    bool quit = false;

    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                std::string savefolder = getNewFile();
                if (savefolder == "")
                {
                    std::cout << "You provided no path for file to save. Please, try harder next time.\n";
                    break;
                }
                tmpnam_s(tempfile, 100);  // create temp filename for intermediate result
                SaveToFile(img, colors, tempfile);
                compress(tempfile, savefolder);
                quit = true;
                break;
            }
        }
    }
    Quit();
    return 0;
}

