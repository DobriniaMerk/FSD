#pragma once
#include "Includes.h"

// operators
SDL_Color operator /(SDL_Color self, float n);
SDL_Color operator *(SDL_Color self, float n);
SDL_Color operator +(SDL_Color a, SDL_Color b);
bool operator ==(SDL_Color a, SDL_Color b);
// operators


// inner methods
void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
SDL_Color get_pixel(SDL_Surface* surface, int x, int y);
int clamp(int val, int min = 0, int max = 255);
float DistanceTo(SDL_Color self, SDL_Color other);

std::vector <SDL_Color> QuantizeMedian(SDL_Surface*& img, int colorNum);
std::vector<std::vector<SDL_Color> > QuantizeMedianSplit(std::vector<SDL_Color> _colors);
std::vector<SDL_Color> Quantize(SDL_Surface* img, int colorNum);
int GetNearest(SDL_Color color, std::vector<SDL_Color> search, int maxDist);
SDL_Surface* AddDebug(SDL_Surface* image, std::vector<SDL_Color> colors);
// inner methods

// for public use
std::vector<SDL_Color> Dither(SDL_Surface* image, int colorDepth);
void SaveToFile(SDL_Surface* img, std::vector<SDL_Color> colors, std::string filename);
