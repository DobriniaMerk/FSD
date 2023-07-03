#pragma once
#include "Includes.h"
#include <atlstr.h>

SDL_Surface* loadImage();
std::string getFile();
std::string getNewFile();
void compress(std::string source, std::string dest);
