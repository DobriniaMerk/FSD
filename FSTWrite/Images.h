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
void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
SDL_Color get_pixel(SDL_Surface* surface, int x, int y);
int clamp(int val, int min = 0, int max = 255);
float DistanceTo(SDL_Color self, SDL_Color other);

/// <summary>
/// Quatization by median cut
/// </summary>
/// <param name="img">Source image</param>
/// <param name="colorNum">Number of colors to return; Must be a power of two</param>
/// <returns>Array of Color[colorNum]</returns>
std::vector <SDL_Color> QuantizeMedian(SDL_Surface*& img, int colorNum);

/// <summary>
/// Splits "colors" array in halves by maximum color channel
/// </summary>
/// <param name="colors">Colors to split</param>
/// <returns></returns>
std::vector<std::vector<SDL_Color> > QuantizeMedianSplit(std::vector<SDL_Color> _colors);

/// <summary>
/// Searchs nearest but not farther than maxDist color to color in search array
/// </summary>
/// <param name="color">Base color</param>
/// <param name="search">Array for searching in</param>
/// <param name="maxDist">Maximum distance of nearest color</param>
/// <returns>Color</returns>
int GetNearest(SDL_Color color, std::vector<SDL_Color> search, int maxDist);

/// <summary>
/// Adds debug squares at the bottom of the image (you should not need this if all goes well)
/// </summary>
/// <param name="image"></param>
/// <param name="colors"></param>
/// <returns></returns>
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

/// <summary>
/// Color quantization by k-means clustering
/// </summary>
/// <param name="img">Sourse image to take colors out</param>
/// <param name="colorNum">Number of colors to return</param>
/// <returns></returns>
std::vector<std::vector<float>> Quantize(SDL_Surface* img, int colorNum);

/// <summary>
/// Save image in a FSD format to provided path, additionally compressing it with zpaq.
/// </summary>
/// <param name="img">Image to save</param>
/// <param name="path">Path to saved image</param>
void SaveToFile(SDL_Surface* img, std::vector<SDL_Color> colors, std::string filename);
