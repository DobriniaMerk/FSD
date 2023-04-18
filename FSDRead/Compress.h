#pragma once

#include <string>
#include <fstream>
#include <vector>

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

static class Compress
{
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

	static std::ifstream filein;
	static std::ofstream fileout;

	static std::string decompress();
	static std::wstring getFile();

public:
	static SDL_Surface* getImage();
};