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
	static std::string decompress();
	static std::wstring getFile();

public:
	static SDL_Surface* getImage();
};