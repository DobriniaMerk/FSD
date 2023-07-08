#include "Images.h"
#include "Files.h"

char tempfile[100];

SDL_Window* window = NULL;
SDL_Surface* windowSurface = NULL;
SDL_Surface* image = NULL;

struct color
{
    int r, g, b;

    color(int _r, int _g, int _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }

    SDL_Color toSDL()
    {
        SDL_Color c = { r, g, b };
        return c;
    }
};

color operator*(color c, float n)
{
    return color(c.r * n, c.g * n, c.b * n);
}

color operator+(color a, color b)
{
    return color(a.r + b.r, a.g + b.g, a.b +f a.b);
}

color fromSDL(SDL_Color c)
{
    return color(c.r, c.g, c.b);
}

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

    std::vector<SDL_Color> colors = Quantize(img, colornum);

    SDL_BlitSurface(img, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(window);

    bool quit = false;

    int x = 0, y = 0;
    SDL_Color pix;
    SDL_Color wanted;
    color error(0, 0, 0);
    color t(0, 0, 0);
    int i;

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
            case SDL_KEYDOWN:
                i = 0;
                while (++i < img->w)
                {
                    pix = get_pixel(img, x, y);
                    wanted = colors[GetNearest(pix, colors, 100000000)];
                    set_pixel(img, x, y, SDL_MapRGB(img->format, wanted.r, wanted.g, wanted.b));
                    error = color((int)pix.r - (int)wanted.r, (int)pix.g - (int)wanted.g, (int)pix.b - (int)wanted.b);

                    if (x < img->w - 1)
                    {
                        t = (error * (7.0 / 16.0)) + fromSDL(get_pixel(img, x + 1, y));
                        set_pixel(img, x + 1, y, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                    }
                    if (y < img->h - 1)
                    {
                        if (x < img->w - 1)
                        {
                            t = (error * (1.0 / 16.0)) + fromSDL(get_pixel(img, x + 1, y + 1));
                            set_pixel(img, x + 1, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                        }
                        if (x > 0)
                        {
                            t = (error * (3.0 / 16.0)) + fromSDL(get_pixel(img, x - 1, y + 1));
                            set_pixel(img, x - 1, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                        }

                        t = (error * (5.0 / 16.0)) + fromSDL(get_pixel(img, x, y + 1));
                        set_pixel(img, x, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                    }

                    ++x;
                    y += x / img->w;
                    x -= (x == img->w) * img->w;
                }
                SDL_BlitSurface(img, NULL, windowSurface, NULL);
                SDL_UpdateWindowSurface(window);
                break;
            case SDL_MOUSEBUTTONDOWN:
                std::string savefolder = getNewFile();
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

