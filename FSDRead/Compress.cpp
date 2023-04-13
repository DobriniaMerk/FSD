// #include "Compress.h" // not working

#include <string>
#include <fstream>

#include <string.h>
#include <iostream>
#include <algorithm>
#include <windows.h>

#include <SDL.h>

//Zpaq
#include "libzpaq.h"
#include <stdio.h>
#include <stdlib.h>
//Zpaq

#define _CRT_SECURE_NO_WARNINGS

void libzpaq::error(const char* msg)  // print message and exit
{
    fprintf(stderr, "Oops: %s\n", msg);
    exit(1);
}

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 p) // i still need to figure out how single pixels are stored
{
    Uint32* const pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    *pixel = p;
}

static class Compress
{
    static std::ifstream filein;
    static std::ofstream fileout;

    class In : public libzpaq::Reader
    {
    public:
        std::ifstream* filein;

        In(std::ifstream* f)
        {
            filein = f;
        }

        int offset = 0;
        int get() {
            unsigned char t;
            if (!filein->eof())
            {
                filein->seekg(offset++);
                filein->read((char*)&t, 1);
                return t;
            }

            return -1;
        }  // returns byte 0..255 or -1 at EOF
    };

    class Out : public libzpaq::Writer
    {
    public:
        std::ofstream* fileout;

        Out(std::ofstream* f)
        {
            fileout = f;
        }

        void put(int c) {
            unsigned char t = c;
            fileout->write((char*)&t, 1);
        }  // writes 1 byte 0..255
    };

    static void decompress(std::string path)
    {
        // TODO: transef this to .cpp file
        In in(&filein);
        Out out(&fileout);

        char tempfile[100];

        std::wstring filename;
        tmpnam_s(tempfile, 100); // generates temporary filename; almost guaraneed to be unique


        filename = getFile();

        filein = std::ifstream(filename, std::ios::in | std::ios::binary);
        fileout = std::ofstream(tempfile, std::ios::out | std::ios::binary | std::ios::trunc);

        libzpaq::decompress(&in, &out);
    }

    static SDL_Surface* getImage(std::string filename)
    {
        decompress(filename);
        SDL_Surface* surface;

        std::ifstream file(filename, std::ios::in | std::ios::binary);
        unsigned int x, y;
        unsigned char colornum;

        file.read((char*)&x, sizeof(unsigned int));
        file.read((char*)&y, sizeof(unsigned int));        // get image x and y size
        file.read((char*)&colornum, sizeof(char));         // number of colors
        //HERE; create SDL_Surface x by (y / 1.2) pixels

        std::cout << "-------------------" << std::endl;
        std::cout << "X: " << x << " Y: " << y << std::endl << "Number of colors: " << (int)colornum << std::endl;
        std::cout << "-------------------" << std::endl;


        //HERE; vector of colors named colors

        for (int i = 0; i < colornum; i++)
        {
            unsigned char r, g, b;
            file.read((char*)&r, sizeof(char));
            file.read((char*)&g, sizeof(char));
            file.read((char*)&b, sizeof(char));
            std::cout << (int)r << ", " << (int)g << ", " << (int)b << std::endl;
            //HERE; colors.push_back(color(r, g, b));
        }

        std::cout << "-------------------" << std::endl;

        unsigned int n = 0;
        //HERE; writeColor; // color to write now

        while (n < y * x)
        {
            if (file.eof())
                break;

            unsigned char num = 0, code = 0;
            file.read((char*)&num, sizeof(char));
            file.read((char*)&code, sizeof(char));
            //HERE; writeColor = colors[code];

            for (int i = 0; i < num; i++) // SDL has fill_rect; rewrite with it
            {
                //HERE; surface set x, y pixel to writeColor
                // surface is smaller than image in file; rest of image ignore
                n++;
            }

        }

        std::cout << n << ' ' << x * y;

        file.close();

        return surface;
    }
    
    static std::wstring getFile()
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
};