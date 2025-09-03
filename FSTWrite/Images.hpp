#include <SDL.h>
#include <vector>
#include <string>

/// Color quantization by k-means clustering
/// init_type - 0: Median split; 1: k-means++
std::vector<std::vector<float>> Quantize(SDL_Surface* img, int colorNum, int init_type, int max_steps = 100);

/// Draws image with suppied colors.
/// Important: This method rewrites the image, not returns a copy.
void Dither(SDL_Surface* img, const std::vector<std::vector<float>>& cls);

/// Save image in a FSD format to provided path, additionally compressing it with zpaq.
void SaveToFile(SDL_Surface* img, const std::vector<std::vector<float>>& cls, const std::string& filename);
