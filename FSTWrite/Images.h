#pragma once
#include "Includes.h"

// operators
SDL_Color operator /(SDL_Color self, float n);
SDL_Color operator *(SDL_Color self, float n);
SDL_Color operator +(SDL_Color a, SDL_Color b);
bool operator ==(SDL_Color a, SDL_Color b);
// operators

// inner methods
std::vector<std::vector<float>> toColorVector(std::vector<SDL_Color> in);
std::vector<SDL_Color> fromColorVector(std::vector<std::vector<float>> in);
SDL_Color getPixel(SDL_Surface* img, int x, int y);
int clamp(int val, int min, int max);
float DistanceTo(SDL_Color self, SDL_Color other);

std::vector <SDL_Color> QuantizeMedian(SDL_Surface*& img, int colorNum);
std::vector<std::vector<SDL_Color> > QuantizeMedianSplit(std::vector<SDL_Color> _colors);
int GetNearest(SDL_Color color, std::vector<SDL_Color> search, int maxDist);
SDL_Surface* AddDebug(SDL_Surface* image, std::vector<SDL_Color> colors);
// inner methods

// for public use

/// <summary>
/// Draws image with colors suppied only.
/// Important: This method rewrites the image, not returns a copy.
/// </summary>
/// <param name="image"></param>
/// <param name="colors"></param>
void Dither(SDL_Surface* image, std::vector<std::vector<float>> colors);
std::vector<std::vector<float>> Quantize(SDL_Surface* img, int colorNum);
void SaveToFile(SDL_Surface* img, std::vector<std::vector<float>> cls, std::string filename);
