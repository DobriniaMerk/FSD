#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>

Uint8 clamp(int val)
{
    return (Uint8)std::max(0, std::min(255, val));
}

struct color
{
    int r = 0, g = 0, b = 0;

    color() = default;
    color(int r, int g, int b) : r(r), g(g), b(b) {};
    explicit color(SDL_Color c) : r(c.r), g(c.g), b(c.b) {};

    explicit operator SDL_Color()
    {
        SDL_Color c{clamp(r), clamp(g), clamp(b)};
        return c;
    }
};

color operator-(color const a, color const b)
{
    return color(a.r - b.r, a.g - b.g, a.b - a.b);
}

color operator*(color c, float n)
{
    return color(c.r * n, c.g * n, c.b * n);
}

color operator/(color c, float n)
{
    return color(c.r / n, c.g / n, c.b / n);
}

color operator+(color a, color b)
{
    return color(a.r + b.r, a.g + b.g, a.b + b.b);
}

SDL_Color operator*(SDL_Color self, float n)
{
    SDL_Color c = (SDL_Color)(color(self) * n);
    return c;
}

SDL_Color operator+(SDL_Color a, SDL_Color b)
{
    SDL_Color c = (SDL_Color)(color(a) + color(b));
    return c;
}

bool operator==(SDL_Color a, SDL_Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

std::vector<std::vector<float>> toColorVector(const std::vector<SDL_Color>& in)
{
    std::vector<std::vector<float>> ret(in.size());
    for (int i = 0; i < in.size(); i++)
    {
        ret[i] = std::vector<float>{((float)in[i].r) / 255.0f, ((float)in[i].g) / 255.0f, ((float)in[i].b) / 255.0f };
    }
    return ret;
}

std::vector<SDL_Color> fromColorVector(const std::vector<std::vector<float>>& in)
{
    std::vector<SDL_Color> ret(in.size());
    for (int i = 0; i < in.size(); i++)
    {
        SDL_Color t = { (Uint8)(in[i][0]*255), (Uint8)(in[i][1]*255), (Uint8)(in[i][2]*255) };
        ret[i] = t;
    }
    return ret;
}

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel) // stackoverflow's balck magic
{
    Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}

SDL_Color get_pixel(SDL_Surface* surface, int x, int y) // stackoverflow's balck magic, modified
{
    Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    SDL_Color ret = {0, 0, 0};
    SDL_GetRGB(*target_pixel, surface->format, &ret.r, &ret.g, &ret.b);
    return ret;
}

/// Distance betwen two colors in 3D RGB colorspace
/// Note: resulted distance is actually a square of real distance for performance reasons
float DistanceTo(SDL_Color self, SDL_Color other)
{
    color a(self);
    color b(other);
    return (a.r - b.r) * (a.r - b.r) + (a.g - b.g) * (a.g - b.g) + (a.b - b.b) * (a.b - b.b);
}

int GetNearest(SDL_Color color, const std::vector<SDL_Color>& search, int maxDist)
{
    float dist = -1;
    int ret = -1;
    SDL_Color c;

    for(int i = 0; i < search.size(); i++)
    {
        float  tDist = 0;
        c = search[i];
        tDist = DistanceTo(color, c);

        if (tDist < maxDist && (dist == -1 || tDist < dist))
        {
            dist = tDist;
            ret = i;
        }
    }

    return ret;
}

std::vector<SDL_Color> SampleColors(SDL_Surface* img, int skip = 10)
{
    // TODO: add smart calculation of skip so the number of colors will not be dependent on image size
    // TODO: sample more smartly. Maybe choose more of rare colors?
    std::vector<SDL_Color> ret;
    int n = 0;
    for (int i = 0; i < img->w; i += skip)
        for (int j = 0; j < img->h; j += skip)
        {
            SDL_Color c = get_pixel(img, i, j);
            ret.push_back(c);
        }
    return ret;
}

std::vector<std::vector<SDL_Color>> QuantizeMedianSplit(std::vector<SDL_Color> colors)
{
    std::vector<std::vector<SDL_Color>> ret(2);
    SDL_Color c;
    int r = 0, g = 0, b = 0;

    for (int i = 0; i < colors.size(); i++)
    {
        c = colors[i];
        r += c.r;
        g += c.g;
        b += c.b;
    }

    if (r > g && r > b)
        std::sort(colors.begin(), colors.end(), [](SDL_Color x, SDL_Color y) { return x.r < y.r; });
    else if (g > r && g > b)
        std::sort(colors.begin(), colors.end(), [](SDL_Color x, SDL_Color y) { return x.g < y.g; });
    else if (b > r && b > g)
        std::sort(colors.begin(), colors.end(), [](SDL_Color x, SDL_Color y) { return x.b < y.b; });


    for (int i = 0; i < colors.size(); i++)
    {
        if (i < colors.size() / 2)
        {
            ret[0].push_back(colors[i]);
        }
        else
        {
            ret[1].push_back(colors[i]);
        }
    }

    return ret;
}

std::vector<SDL_Color> QuantizeMedian(const std::vector<SDL_Color>& colors, int colorNum)
{
    int filledRows = 1;

    std::vector<std::vector<SDL_Color>> oldColors(colorNum);
    std::vector<std::vector<SDL_Color>> newColors(colorNum);
    std::vector<std::vector<SDL_Color>> t;

    oldColors[0] = colors;

    while (filledRows < colorNum)  // while not all colors are done
    {
        for (int j = filledRows / 2; j < filledRows; j++)
        {
            t = QuantizeMedianSplit(oldColors[j]);  // split each filled row
            newColors[j * 2] = t[0];
            newColors[j * 2 + 1] = t[1];  // assign them to newColors
        }

        filledRows *= 2;

        oldColors = newColors;
        for (int y = 0; y < filledRows; y++)
            newColors[y].clear();
    }

    std::vector<SDL_Color> ret(colorNum);  // colors to return

    for (int i = 0; i < colorNum; i++)  // calculate mean color of each array and return them
    {
        color sum;
        int n;

        for(int j = 0; j < oldColors[i].size(); j++)
        {
            sum = sum + color(oldColors[i][j]);
            n++;
        }

        ret[i] = SDL_Color(sum / n);
    }

    for (int i = 0; i < ret.size(); i++)
        std::cout << (int)ret[i].r << ", " << (int)ret[i].g << ", " << (int)ret[i].b << std::endl;

    return ret;
}

// Set take_root to false for more performance. Might have unexpected results, not tested
std::vector <SDL_Color> QuantizeWeightedRandom(const std::vector<SDL_Color>& colors, int colorNum, bool take_root = true)
{
    std::random_device r;
    std::mt19937 generator(r());

    std::vector<SDL_Color> points;
    points.push_back(colors[generator() % colors.size()]);

    while (points.size() < colorNum)
    {
        std::vector<float> dist(colors.size(), 0);
        for (int i = 0; i < colors.size(); i++)
        {
            for (int j = 0; j < points.size(); j++)
            {
                float d = DistanceTo(colors[i], points[j]);
                dist[i] = d > dist[i] ? d : dist[i];
            }
            if (take_root)
                dist[i] = std::sqrtf(dist[i]);
        }

        std::discrete_distribution<> pick(dist.begin(), dist.end());

        points.push_back(colors[pick(generator)]);
    }

    return points;
}

std::vector<std::vector<float>> Quantize(SDL_Surface* img, int colorNum, int init_type, int max_steps = 100)
{
    std::vector<SDL_Color> means;
    std::vector<SDL_Color> colors = SampleColors(img);

    switch (init_type)
    {
    case 0:
        means = QuantizeMedian(colors, colorNum);
        break;
    case 1:
        means = QuantizeWeightedRandom(colors, colorNum);
        break;
    }

    std::vector<SDL_Color> old_means = means;

    int imgSize = img->w * img->h;

    std::cout << "----------Quantizing----------" << std::endl;
    for (int i = 0; i < max_steps; i++)
    {
        std::vector<color> sum(colorNum);
        std::vector<int> n(colorNum, 0);

        for (int k = 1; k < imgSize; k += 30)
        {
            SDL_Color col = get_pixel(img, k % img->w, k / img->w);
            int nearestmean = GetNearest(col, means, 300*300*300);

            if (nearestmean < 0)
                continue;

            sum[nearestmean] = sum[nearestmean] + (color)col;
            n[nearestmean]++;
        }

        for (int i = 0; i < colorNum; ++i)
            if (n[i] != 0)
                means[i] = SDL_Color(sum[i] / n[i]);

        if (means == old_means)
        {
            std::cout << "Done after " << i << " quantization cycles\n";
            break;
        }
        old_means = means;
    }

    for (int i = 0; i < means.size(); i++)
        std::cout << (int)means[i].r << ", " << (int)means[i].g << ", " << (int)means[i].b << std::endl;


    return toColorVector(means);
}

// Add debug block to the bottom of the image
// Unused
SDL_Surface* AddDebug(SDL_Surface* image, std::vector<SDL_Color> colors)
{
    int h = image->h;
    int w = image->w;

    SDL_Surface* img = SDL_CreateRGBSurface(0, w, h * 1.2, 32, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, img, NULL);

    int blocksize = 42;
    std::vector<std::vector<SDL_Rect>> fillRects(colors.size());
    int t = 0;

    for (int y = 0; y < image->h * 0.2; y += blocksize)
    {
        for (int x = 0; x < image->w; x += blocksize)
        {
            SDL_Rect rect;
            rect.x = x;
            rect.y = y + h;
            rect.w = std::min(blocksize, w - x);
            rect.h = std::min(blocksize, img->h - (y + h));
            fillRects[t++].push_back(rect);
            t %= colors.size();
        }
    }

    for(int i = 0; i < colors.size(); i++)
        SDL_FillRects(img, &(fillRects[i][0]), fillRects[i].size(), SDL_MapRGB(img->format, colors[i].r, colors[i].g, colors[i].b));

    return img;
}

void Dither(SDL_Surface* img, const std::vector<std::vector<float>>& cls)
{
    // SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    // SDL_BlitSurface(orig, NULL, img, NULL);

    std::vector<SDL_Color> colors = fromColorVector(cls);

    for (int y = 0; y < img->h; y++)
    {
        for (int x = 0; x < img->w; x++)
        {
            SDL_Color pix = get_pixel(img, x, y);
            SDL_Color wanted = colors[GetNearest(pix, colors, 100000000)];
            set_pixel(img, x, y, SDL_MapRGB(img->format, wanted.r, wanted.g, wanted.b));
            color error = (color)pix - (color)wanted;
            color t;

            if (x < img->w - 1)  // TODO: Store modified colors without clamping as they could be corrected again and not need clamping after
            {
                t = (error * (7.0 / 16.0)) + (color)get_pixel(img, x + 1, y);
                set_pixel(img, x + 1, y, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
            }
            if (y < img->h - 1)
            {
                if (x < img->w - 1)
                {
                    t = (error * (1.0 / 16.0)) + (color)get_pixel(img, x + 1, y + 1);
                    set_pixel(img, x + 1, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                }
                if (x > 0)
                {
                    t = (error * (3.0 / 16.0)) + (color)get_pixel(img, x - 1, y + 1);
                    set_pixel(img, x - 1, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
                }

                t = (error * (5.0 / 16.0)) + (color)get_pixel(img, x, y + 1);
                set_pixel(img, x, y + 1, SDL_MapRGB(img->format, clamp(t.r), clamp(t.g), clamp(t.b)));
            }
        }
    }

    // SDL_BlitSurface(img, NULL, orig, NULL);
}

void SaveToFile(SDL_Surface* img, const std::vector<std::vector<float>>& cls, const std::string& filename)
{
    // SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    // SDL_BlitSurface(orig, NULL, img, NULL);

    std::vector<SDL_Color> colors = fromColorVector(cls);

    std::ofstream filestream(filename, std::ios::in|std::ios::binary|std::ios::trunc);  // std::ios::trunc is for writing file over instead of appending
    unsigned char w = img->w, h = img->h;
    filestream.write((char*)&w, sizeof(unsigned int));  // first 4 bytes is x of image
    filestream.write((char*)&h, sizeof(unsigned int));  // second 4 bytes is y

    unsigned char colornum = colors.size();
    filestream.write((char*)&colornum, sizeof(char));  // next byte is number of colors


    char r, g, b;
    for (int i = 0; i < (int)colornum; i++)  // then colornum*3 bytes representing colors
    {
        r = (char)colors[i].r;
        g = (char)colors[i].g;
        b = (char)colors[i].b;
        filestream.write(&r, sizeof(r));
        filestream.write(&g, sizeof(g));
        filestream.write(&b, sizeof(b));
    }


    SDL_Color color = get_pixel(img, 0, 0);  // write first pixel in memory
    unsigned char rowLength = 1;

    unsigned int x, y;
    unsigned char code;
    unsigned char maxrow = 254;

    int counter = 0;

    for (int n = 1; n < w * h; n++)
    {
        counter++;

        x = n % w, y = n / w;
        SDL_Color pixelColor = get_pixel(img, x, y);

        if (pixelColor == color && rowLength <= maxrow)  // if current pixel color matches color of row     // 255 is reserved
            rowLength++;
        else  // if not, write current row length and color to file and start new row
        {
            code = 255;

            for (unsigned int i = 0; i < colors.size(); i++)   // search for matching color code
            {
                if (color == colors[i])
                {
                    code = i;
                    break;
                }
            }

            if (code == 255)
                std::cout << "Something went terribly wrong! Pixel " << x << ", " << y << " is not in the colorlist!\n";

            filestream.write((char*)&rowLength, sizeof(char));
            filestream.write((char*)&code, sizeof(char));
            color = pixelColor;
            rowLength = static_cast <char>(1);
        }
    }

    for (unsigned char i = 0; i < colors.size(); i++)   // search for matching color code
    {
        if (color == colors[i])
        {
            code = i;
            break;
        }
    }

    std::cout << counter << std::endl;

    filestream.write((char*)&rowLength, sizeof(char));
    filestream.write((char*)&code, sizeof(char));

    filestream.flush();
    filestream.close();
}
