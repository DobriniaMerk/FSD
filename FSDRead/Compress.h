#pragma once
#include <string>
#include <fstream>

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



class Compress
{
	std::ifstream filein;
	std::ofstream fileout;

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
    } in = In(&filein);

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
    } out = Out(&fileout);

    std::string Decompress(std::string);

    SDL_Surface* GetImage(std::string);
};
