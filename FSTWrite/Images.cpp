#include "Images.h"

/// <summary>
/// Color structure with color range of int, not unsigned byte, initially made for diffrence operations.
/// </summary>
struct color
{
    int r = 0, g = 0, b = 0;

    color() {};
    color(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {};
    explicit color(SDL_Color c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
    }

    explicit operator SDL_Color()
    {
        SDL_Color c = { clamp(r), clamp(g), clamp(b) };
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


SDL_Color operator *(SDL_Color self, float n)
{
    SDL_Color c = { clamp((int)self.r * n, 0, 255), clamp((int)self.g * n, 0, 255), clamp((int)self.b * n, 0, 255) };
    return c;
}

SDL_Color operator +(SDL_Color a, SDL_Color b)
{
    SDL_Color c = { clamp((int)a.r + (int)b.r, 0, 255), clamp((int)a.g + (int)b.g, 0, 255), clamp((int)a.b + (int)b.b, 0, 255) };
    return c;
}

bool operator ==(SDL_Color a, SDL_Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

std::vector<std::vector<float>> toColorVector(std::vector<SDL_Color> in)
{
    std::vector<std::vector<float>> ret(in.size());
    for (int i = 0; i < in.size(); i++)
    {
        ret[i] = std::vector<float>{((float)in[i].r) / 255.0f, ((float)in[i].g) / 255.0f, ((float)in[i].b) / 255.0f };
    }
    return ret;
}

std::vector<SDL_Color> fromColorVector(std::vector<std::vector<float>> in)
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

int clamp(int val, int mn, int mx)
{
    return max(mn, min(mx, val));
}

/// <summary>
/// Distance betwen two colors in 3D RGB colorspace
/// Note: resulted distance is actually a square of real distance, for performance reasons
/// </summary>
/// <param name="self"></param>
/// <param name="other"></param>
/// <returns></returns>
float DistanceTo(SDL_Color self, SDL_Color other)  // to get proper distance you need sqare root of result; not using for optimisation
{
    return (self.r - other.r) * (self.r - other.r) + (self.g - other.g) * (self.g - other.g) + (self.b - other.b) * (self.b - other.b);
}

std::vector<SDL_Color> SampleColors(SDL_Surface* img, int skip)
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

std::vector <SDL_Color> QuantizeMedian(SDL_Surface* img, int colorNum)
{
    int filledRows = 1;

    std::vector< std::vector<SDL_Color> > oldColors(colorNum);
    std::vector< std::vector<SDL_Color> > newColors(colorNum);
    std::vector< std::vector<SDL_Color> > t;

    oldColors[0] = SampleColors(img);

    while (filledRows < colorNum)  // while not all colors are done
    {
        for (int j = 0; j < filledRows; j++)
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
    std::vector<int> sum = { 0, 0, 0 };
    SDL_Color c;
    float n;

    for (int i = 0; i < colorNum; i++)  // calculate mean color of each array and return them
    {
        n = 0;
        sum[0] = 0;
        sum[1] = 0;
        sum[2] = 0;

        for(int j = 0; j < oldColors[i].size(); j++)
        {
            c = oldColors[i][j];
            sum[0] += c.r;
            sum[1] += c.g;
            sum[2] += c.b;
            n++;
        }

        sum[0] = sum[0] / n;
        sum[1] = sum[1] / n;
        sum[2] = sum[2] / n;
        SDL_Color t = { sum[0], sum[1], sum[2] };
        ret[i] = t;
    }

    for (int i = 0; i < ret.size(); i++)
        std::cout << (int)ret[i].r << ", " << (int)ret[i].g << ", " << (int)ret[i].b << std::endl;

    return ret;
}

std::vector<std::vector<SDL_Color> > QuantizeMedianSplit(std::vector<SDL_Color> _colors)
{
    std::vector<std::vector<SDL_Color> > ret(2);
    std::vector<SDL_Color> colors = _colors;
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

std::vector <SDL_Color> QuantizeWeightedRandom(SDL_Surface* img, int colorNum, bool take_root)
{
    std::random_device r;
    std::mt19937 generator(r());

    std::vector<SDL_Color> colors = SampleColors(img);
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

std::vector<std::vector<float>> Quantize(SDL_Surface* orig, int colorNum, int init_type)  // transfer to Files.cpp?
{
    SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, img, NULL);

    std::vector<SDL_Color> means(colorNum);

    switch (init_type)
    {
    case 0:
        means = QuantizeMedian(img, colorNum);
        break;
    case 1:
        means = QuantizeWeightedRandom(img, colorNum);
        break;
    }

    std::vector<SDL_Color> old_means = means;

    int imgSize = img->w * img->h;

    for (int i = 0; i < 100; i++)
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
        {
            if (n[i] != 0)
            {
                SDL_Color t = (SDL_Color)(sum[i] / n[i]);
                means[i] = t;
            }
        }

        if (means == old_means)
        {
            std::cout << "Done " << i << " quantization cycles\n";
            break;
        }
        old_means = means;
    }

    std::cout << "----------------------" << std::endl;
    for (int i = 0; i < means.size(); i++)
        std::cout << (int)means[i].r << ", " << (int)means[i].g << ", " << (int)means[i].b << std::endl;


    return toColorVector(means);
}

int GetNearest(SDL_Color color, std::vector<SDL_Color> search, int maxDist)
{
    float dist = -1, tDist = 0;
    int ret = -1;
    SDL_Color c;

    //foreach (sf::Color c in search)
    for(int i = 0; i < search.size(); i++)
    {
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
            rect.w = min(blocksize, w - x);
            rect.h = min(blocksize, img->h - (y + h));
            fillRects[t++].push_back(rect);
            t %= colors.size();
        }
    }

    for(int i = 0; i < colors.size(); i++)
        SDL_FillRects(img, &(fillRects[i][0]), fillRects[i].size(), SDL_MapRGB(img->format, colors[i].r, colors[i].g, colors[i].b));
    
    return img;
}

void Dither(SDL_Surface* orig, std::vector<std::vector<float>> cls)
{
    SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, img, NULL);

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

            if (x < img->w - 1)
            {
                t = (error * (7.0 / 16.0)) + (color)get_pixel(img, x + 1, y);
                set_pixel(img, x + 1, y, SDL_MapRGB(img->format, clamp(t.r, 0, 255), clamp(t.g, 0, 255), clamp(t.b, 0, 255)));
            }
            if (y < img->h - 1)
            {
                if (x < img->w - 1)
                {
                    t = (error * (1.0 / 16.0)) + (color)get_pixel(img, x + 1, y + 1);
                    set_pixel(img, x + 1, y + 1, SDL_MapRGB(img->format, clamp(t.r, 0, 255), clamp(t.g, 0, 255), clamp(t.b, 0, 255)));
                }
                if (x > 0)
                {
                    t = (error * (3.0 / 16.0)) + (color)get_pixel(img, x - 1, y + 1);
                    set_pixel(img, x - 1, y + 1, SDL_MapRGB(img->format, clamp(t.r, 0, 255), clamp(t.g, 0, 255), clamp(t.b, 0, 255)));
                }

                t = (error * (5.0 / 16.0)) + (color)get_pixel(img, x, y + 1);
                set_pixel(img, x, y + 1, SDL_MapRGB(img->format, clamp(t.r, 0, 255), clamp(t.g, 0, 255), clamp(t.b, 0, 255)));
            }
        }
    }
  
    SDL_BlitSurface(img, NULL, orig, NULL);
}

void SaveToFile(SDL_Surface* orig, std::vector<std::vector<float>> cls, std::string filename)
{
    SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, img, NULL);

    std::vector<SDL_Color> colors = fromColorVector(cls);

    std::ofstream filestream(filename, std::ios::in|std::ios::binary|std::ios::trunc);  // std::ios::trunc is for writing file over instead of appending
    int w = img->w, h = img->h;
    filestream.write((char*)&w, sizeof(unsigned int));  // first 4 bytes is x of image
    filestream.write((char*)&h, sizeof(unsigned int));  // second 4 bytes is y

    unsigned char colornum = colors.size();
    filestream.write((char*)&colornum, sizeof(char));               // next byte is number of colors

    std::cout << "-------------------" << std::endl;

    char r, g, b;
    for (int i = 0; i < (int)colornum; i++)                  // then colornum*3 bytes representing colors
    {
        r = (char)colors[i].r;
        g = (char)colors[i].g;
        b = (char)colors[i].b;
        filestream.write(&r, sizeof(r));
        filestream.write(&g, sizeof(g));
        filestream.write(&b, sizeof(b));
        std::cout << (int)r << ", " << (int)g << ", " << (int)b << std::endl;
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

        //Uint8 r, g, b;
        //SDL_GetRGB(((Uint32*)img->pixels)[n], img->format, &r, &g, &b);
        //SDL_Color pixelColor = { r, g, b };
        x = n % w, y = n / w;
        if (x == 0)
            std::string breakoint = "something broke";
        SDL_Color pixelColor = get_pixel(img, x, y);

        if (pixelColor == color && rowLength <= maxrow)    // if current pixel color matches color of row     // 255 is reserved
            rowLength++;
        else                                           // if not, write current row length and color to file and start new row
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