#include <SDL.h>

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

sf::Image ReadFile(std::string);
std::wstring getFile();

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;


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

    window = SDL_CreateWindow("FSD Reader", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowx, windowy, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        std::cout << "Failed to create window";
        return 1;
    }

    surface = SDL_GetWindowSurface(window);

    return 0;
}

int Quit()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    /* image decompression
    *  handle later
    sf::String filename;
    tmpnam_s(tempfile, 100);

    for (int i = 0; i < argc; i++)
        std::cout << argv[i];

    if (argc > 1)
    {
        std::string f = argv[1];
        filename = f;
    }
    else
        filename = getFile();
    
    filein = std::ifstream(filename.toAnsiString(), std::ios::in | std::ios::binary);
    fileout = std::ofstream(tempfile, std::ios::out | std::ios::binary | std::ios::trunc);

    libzpaq::decompress(&in, &out);

    sf::Image img = ReadFile(tempfile);

    sf::Image out_img;
    out_img.create(img.getSize().x, img.getSize().y / 1.2, sf::Color::Red);
    for (int i = 0; i < out_img.getSize().x * out_img.getSize().y; i++)
    {
        int x = i % out_img.getSize().x;
        int y = i / out_img.getSize().x;

        out_img.setPixel(x, y, img.getPixel(x, y));
    }
    */
    
    int windowx = 800, windowy = 600;
    
    if (InitWindow(windowx, windowy))
    {
        std::cout << "Failed to initialize window";
        return 1;
    }

    SDL_UpdateWindowSurface(window); // after all drawing is done

    sf::Texture t;
    t.loadFromImage(out_img);
    sf::Sprite s;
    s.setTexture(t);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(s);
        window.display();
    }

    Quit();

    return 0;
}

sf::Image ReadFile(std::string filename = "out.fsd")
{
    sf::Image img;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    unsigned int x, y;
    unsigned char colornum;


    file.read((char*)&x, sizeof(unsigned int));
    file.read((char*)&y, sizeof(unsigned int));        // get image x and y size
    file.read((char*)&colornum, sizeof(char));                // number of colors
    img.create(x, y, sf::Color(255, 0, 0));  // filled with red for debugging

    std::cout << "-------------------" << std::endl;
    std::cout << "X: " << x << " Y: " << y << std::endl << "Number of colors: " << (int)colornum << std::endl;
    std::cout << "-------------------" << std::endl;


    std::vector<sf::Color> colors;

    for (int i = 0; i < colornum; i++)
    {
        unsigned char r, g, b;
        file.read((char*)&r, sizeof(char));
        file.read((char*)&g, sizeof(char));
        file.read((char*)&b, sizeof(char));
        std::cout << (int)r << ", " << (int)g << ", " << (int)b << std::endl;
        colors.push_back(sf::Color(r, g, b));
    }

    std::cout << "-------------------" << std::endl;

    unsigned int n = 0;
    sf::Color writeColor;

    while (n < y * x)
    {
        if (file.eof())
            break;

        unsigned char num = 0, code = 0;
        file.read((char*)&num, sizeof(char));
        file.read((char*)&code, sizeof(char));
        writeColor = colors[code];

        for (int i = 0; i < num; i++)
        {
            img.setPixel(n % x, n / x, writeColor);
            n++;
        }

    }

    std::cout << n <<' ' << x * y;

    file.close();

    return img;
}

std::wstring getFile()
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"FSD Image\0 *.FSD\0\0";
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
