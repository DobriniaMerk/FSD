#pragma once

#include <string>
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
SDL_Surface* readImage(std::string path);
