#include "Compress.h"

void libzpaq::error(const char* msg)  // print error message and exit
{
    fprintf(stderr, "Oops: %s\n", msg);
    exit(1);
}

class In : public libzpaq::Reader
{
public:
    std::ifstream* readfile;

    In(std::ifstream* f)
    {
        readfile = f;
    }

    int offset = 0;
    int get() {
        unsigned char t;
        if (!readfile->eof())
        {
            readfile->seekg(offset++);
            readfile->read((char*)&t, 1);
            return t;
        }

        return -1;
    }  // returns byte 0..255 or -1 at EOF
};

class Out : public libzpaq::Writer
{
public:
    std::ofstream* writefile;

    Out(std::ofstream* f)
    {
        writefile = f;
    }

    void put(int c) {
        unsigned char t = c;
        writefile->write((char*)&t, 1);
    }  // writes 1 byte 0..255
};

void decompress(std::string source, std::string dest)
{
    std::ifstream filein = std::ifstream(source, std::ios::in | std::ios::binary);
    std::ofstream fileout = std::ofstream(dest, std::ios::out | std::ios::binary | std::ios::trunc);  // trunc means clear file and then write to it

    In in(&filein);
    Out out(&fileout);

    libzpaq::decompress(&in, &out);

   // std::cout << "Decompressed file\n";

    filein.close();
    fileout.close();
}

SDL_Surface* readImage(std::string path)
{
    SDL_Surface* surface;

    std::ifstream file(path, std::ios::in | std::ios::binary);
    unsigned int x, y;
    unsigned char colornum;

    file.read((char*)&x, sizeof(unsigned int));
    file.read((char*)&y, sizeof(unsigned int));        // get image x and y size
    file.read((char*)&colornum, sizeof(char));         // number of colors

    //y /= 1.2;  // because of the bug that distorts bottom of the image  // decomment in case of missing pixels
    surface = SDL_CreateRGBSurface(0, x, y, 32, 0, 0, 0, 0);  // using default colormode and depth; consider changing to 8 bit depth and working with palettes?

    //std::cout << "-------------------" << std::endl;
    //std::cout << "X: " << x << " Y: " << y << std::endl << "Number of colors: " << (int)colornum << std::endl;
    //std::cout << "-------------------" << std::endl;

    //HERE; vector of colors named colors
    std::vector<Uint32> colors;

    for (int i = 0; i < colornum; i++)
    {
        unsigned char r, g, b;
        file.read((char*)&r, sizeof(char));
        file.read((char*)&g, sizeof(char));
        file.read((char*)&b, sizeof(char));

       // std::cout << "Color " << i+1 << ": (" << (int)r << ", " << (int)g << ", " << (int)b << ")" << std::endl;

        colors.push_back(SDL_MapRGB(surface->format, r, g, b));
    }

   // std::cout << "-------------------" << std::endl;

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
            int t = min(num, x - n % x);
            rect.w = t;
            rect.h = 1;
            num -= t;
            n += t;

            fillRects[code].push_back(rect);
        }
    }

    for (int i = 0; i < colornum; i++)
    {
        if (fillRects[i].size())
            SDL_FillRects(surface, &(fillRects[i][0]), fillRects[i].size(), colors[i]);
    }

  //  std::cout << n << ' ' << x * y;

    file.close();

    return surface;
}

std::string getFile()
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
    ofn.FlagsEx = OFN_EX_NOPLACESBAR; // makes things work but look XP

    std::string s = "";

    if (GetOpenFileName(&ofn) == TRUE)
    {
        s = (std::string)CW2A(ofn.lpstrFile);
    }
    return s;
}