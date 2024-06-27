#pragma once
#include "Includes.h"
#include <atlstr.h>

std::string getFile();
std::string getNewFile(std::string extention = ".FSD");
void compress(std::string source, std::string dest);
