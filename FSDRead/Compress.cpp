#include "Compress.h"

void libzpaq::error(const char* msg)  // print error message and exit
{
    fprintf(stderr, "Oops: %s\n", msg);
    exit(1);
}

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 p)  // not needed for now; leaving if will be useful
{
    Uint32* const pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    *pixel = p;
}

std::string Compress::decompress()
{
    In in(&filein);
    Out out(&fileout);

    char tempfile[100];

    std::wstring filename;
    tmpnam_s(tempfile, 100); // generates temporary filename; almost guaraneed to be unique

    filename = getFile();

    filein = std::ifstream(filename, std::ios::in | std::ios::binary);
    fileout = std::ofstream(tempfile, std::ios::out | std::ios::binary | std::ios::trunc);  // trunc means clear file and then write to it

    libzpaq::decompress(&in, &out);

    return tempfile;
}

SDL_Surface* Compress::getImage()
{
    std::string decompressed = decompress();

    SDL_Surface* surface;

    std::ifstream file(decompressed, std::ios::in | std::ios::binary);
    unsigned int x, y;
    unsigned char colornum;

    file.read((char*)&x, sizeof(unsigned int));
    file.read((char*)&y, sizeof(unsigned int));        // get image x and y size
    file.read((char*)&colornum, sizeof(char));         // number of colors

    y /= 1.2;  // because of the bug that distorts bottom of the image
    surface = SDL_CreateRGBSurface(0, x, y, 32, 0, 0, 0, 0);  // using default colormode and depth; consider changing to 8 bit depth and working with palettes?

    std::cout << "-------------------" << std::endl;
    std::cout << "X: " << x << " Y: " << y << std::endl << "Number of colors: " << (int)colornum << std::endl;
    std::cout << "-------------------" << std::endl;

    //HERE; vector of colors named colors
    std::vector<Uint32> colors;

    for (int i = 0; i < colornum; i++)
    {
        unsigned char r, g, b;
        file.read((char*)&r, sizeof(char));
        file.read((char*)&g, sizeof(char));
        file.read((char*)&b, sizeof(char));
        std::cout << (int)r << ", " << (int)g << ", " << (int)b << std::endl;

        colors.push_back(SDL_MapRGB(surface->format, r, g, b));
    }

    std::cout << "-------------------" << std::endl;

    unsigned int n = 0;

    std::vector<std::vector<SDL_Rect>> fillRects(colornum);  // rects to fill for each color
    
    while (n < y * x)
    {
        if (file.eof())
            break;

        unsigned char num = 0, code = 0;
        file.read((char*)&num, sizeof(char));
        file.read((char*)&code, sizeof(char));

        while (num)
        {
            SDL_Rect rect;
            rect.x = n % x;
            rect.y = n / x;
            int t = min(num, x - n % x);  // dangerous line; bug can be or not be here
            rect.w = t;
            rect.h = 1;
            num -= t;
            n += t;

            fillRects[code].push_back(rect);
        }
    }

    for (int i = 0; i < colornum; i++)
    {
        SDL_FillRects(surface, &(fillRects[i][0]), fillRects[i].size(), colors[i]);
    }

    std::cout << n << ' ' << x * y;

    file.close();

    return surface;
}

std::wstring Compress::getFile()
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"FSD Image\0 *.FSD\0\0";  // mask for allowed extensions
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    std::string s = "";
    std::wstring empty(s.begin(), s.end());

    if (GetOpenFileName(&ofn) == TRUE)
    {
        std::wstring ws(ofn.lpstrFile);
        std::wcout << ws << std::endl;
        return ws;
    }
    return empty;
}