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


std::string decompress();
std::wstring getFile();
SDL_Surface* getImage();
