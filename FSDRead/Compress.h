#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <vector>

#include <string.h>
#include <iostream>
#include <algorithm>
#include <windows.h>

#include <SDL.h>

#include <atlstr.h>

//Zpaq
#include "libzpaq.h"
#include <stdio.h>
#include <stdlib.h>
//Zpaq

#define _CRT_SECURE_NO_WARNINGS

void decompress(std::string source, std::string dest);
std::string getFile();
std::string getNewFile(std::string extention);
SDL_Surface* readImage(std::string path);
