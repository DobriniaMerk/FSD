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
    std::wstring str;

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

    Quit();

    return 0;
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
