#pragma once
#include <string>

std::string getImageFile();
char const* saveFile(char const* defaultpath = nullptr, char const** filterpatterns = nullptr, char const* filterdescr = nullptr);
void compress(std::string source, std::string dest);
