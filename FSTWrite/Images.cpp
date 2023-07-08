#include "Images.h"

SDL_Color operator /(SDL_Color self, float n)
{
    SDL_Color c = { (self.r / n), (self.g / n), (self.b / n) };
    return c;
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

SDL_Color Multiply(SDL_Color self, float n)
{
    return self * n;
}

SDL_Color Add(SDL_Color a, SDL_Color b)
{
    return a + b;
}

bool operator ==(SDL_Color a, SDL_Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

SDL_Color getPixel(SDL_Surface* img, int x, int y)
{
    Uint8 r, g, b;
    SDL_GetRGB(((Uint32*)img->pixels)[x + y * img->w], img->format, &r, &g, &b);
    SDL_Color pix = { r, g, b };
    return pix;
}


int clamp(int val, int mn, int mx)
{
    return max(mn, min(mx, val));
}

float DistanceTo(SDL_Color self, SDL_Color other)  // to get proper distance you need sqare root of result; not using for optimisation
{
    return (self.r - other.r) * (self.r - other.r) + (self.g - other.g) * (self.g - other.g) + (self.b - other.b) * (self.b - other.b);
}

/// <summary>
/// Quatization by median cut
/// </summary>
/// <param name="img">Source image</param>
/// <param name="colorNum">Number of colors to return; Must be a power of two</param>
/// <returns>Array of Color[colorNum]</returns>
std::vector <SDL_Color> QuantizeMedian(SDL_Surface*& img, int colorNum)
{
    int skip = 10;
    int filledRows = 1;

    std::vector< std::vector<SDL_Color> > oldColors(colorNum);
    std::vector< std::vector<SDL_Color> > newColors(colorNum);
    std::vector< std::vector<SDL_Color> > t;

    for (int i = 0; i < img->w; i += skip)  // set first array of oldColors to img pixels, with interval of skip
        for (int j = 0; j < img->h; j += skip)
        {
            SDL_Color c = getPixel(img, i, j);
            oldColors[0].push_back(c);
        }


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


/// <summary>
/// Splits "colors" array in halves by maximum color channel
/// </summary>
/// <param name="colors">Colors to split</param>
/// <returns></returns>
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


/// <summary>
/// Color quantization by clustering (very slow)
/// </summary>
/// <param name="img">Sourse image to take colors out</param>
/// <param name="colorNum">Number of colors to return</param>
/// <returns>Color[colorNum]</returns>
std::vector<SDL_Color> Quantize(SDL_Surface* orig, int colorNum)
{
    SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, img, NULL);

    std::vector<SDL_Color> means(colorNum);

    means = QuantizeMedian(img, colorNum);
    std::vector<SDL_Color> old_means = means;

    int imgSize = img->w * img->h;
    Uint32* pixels = (Uint32*)img->pixels;

    for (int i = 0; i < 100; i++)
    {
        std::vector<std::tuple<long long, long long, long long>> sum(colorNum, { 0, 0, 0 });
        std::vector<int> n(colorNum, 0);

        for (int k = 1; k < imgSize; k += 30)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[k], img->format, &r, &g, &b);
            SDL_Color color = { r, g, b };
            int nearestmean = GetNearest(color, means, 300*300*300);

            if (nearestmean < 0)
                continue;

            std::get<0>(sum[nearestmean]) += r;
            std::get<1>(sum[nearestmean]) += g;
            std::get<2>(sum[nearestmean]) += b;

            n[nearestmean]++;
        }

        for (int i = 0; i < colorNum; ++i)
        {
            if (n[i] != 0)
            {
                SDL_Color t = { std::get<0>(sum[i]) / n[i], std::get<1>(sum[i]) / n[i], std::get<2>(sum[i]) / n[i] };
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


    return means;
}

/// <summary>
/// Searchs nearest but not farther than maxDist color to color in search array
/// </summary>
/// <param name="color">Base color</param>
/// <param name="search">Array for searching in</param>
/// <param name="maxDist">Maximum distance of nearest color</param>
/// <returns>Color</returns>
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

std::vector<SDL_Color> Dither(SDL_Surface* orig, int colorDepth)
{
    SDL_Surface* image = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, image, NULL);

    std::vector<SDL_Color> colors;
    colors = Quantize(image, colorDepth);
    Uint32* pixels = (Uint32*)image->pixels;
    auto* format = image->format;
    int w = image->w;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < image->h; y++)
        {
            SDL_Color pix = getPixel(image, x, y);
            SDL_Color wanted = colors[GetNearest(pix, colors, 100000000)];
            pixels[x + y*w] = SDL_MapRGB(format, wanted.r, wanted.g, wanted.b);
            SDL_Color error = { clamp(std::abs((int)pix.r - (int)wanted.r), 0, 255), clamp(std::abs((int)pix.g - (int)wanted.g), 0, 255), clamp(std::abs((int)pix.b - (int)wanted.b), 0, 255) };

            SDL_Color t;

            // broken
            if (x < w - 1)
            {
                t = (error * (7.0 / 16.0)) + getPixel(image, x + 1, y);
                pixels[x + 1 + y * w] = SDL_MapRGB(format, t.r, t.g, t.b);
            }
            if (y < image->h - 1)
            {
                if (x < w - 1)
                {
                    t = (error * (1.0 / 16.0)) + getPixel(image, x + 1, y + 1);
                    pixels[x + 1 + (y + 1) * w] = SDL_MapRGB(format, t.r, t.g, t.b);
                }
                t = (error * (5.0 / 16.0)) + getPixel(image, x, y + 1);
                pixels[x + (y + 1) * w] = SDL_MapRGB(format, t.r, t.g, t.b);

                t = (error * (3.0 / 16.0)) + getPixel(image, x - 1, y + 1);
                pixels[x - 1 + (y + 1) * w] = SDL_MapRGB(format, t.r, t.g, t.b);
            }
            // broken

        }
    }

    SDL_BlitSurface(image, NULL, orig, NULL);

    return colors;
}

/// <summary>
/// 
/// </summary>
/// <param name="img">Image to save</param>
/// <param name="path">Path to saved image</param>
void SaveToFile(SDL_Surface* orig, std::vector<SDL_Color> colors, std::string filename)
{
    SDL_Surface* img = SDL_CreateRGBSurface(0, orig->w, orig->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(orig, NULL, img, NULL);

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


    SDL_Color color = getPixel(img, 0, 0);  // write first pixel in memory
    unsigned char rowLength = 1;

    unsigned int x, y;
    unsigned char code;
    unsigned char maxrow = 254;

    int counter = 0;

    for (int n = 1; n < w * h; n++)
    {
        counter++;

        Uint8 r, g, b;
        SDL_GetRGB(((Uint32*)img->pixels)[n], img->format, &r, &g, &b);
        SDL_Color pixelColor = { r, g, b };

        if (pixelColor == color && rowLength <= maxrow)    // if current pixel color matches color of row     // 255 is reserved
            rowLength++;
        else                                           // if not, write current row length and color to file and start new row
        {
            code = 0;

            for (unsigned int i = 0; i < colors.size(); i++)   // search for matching color code
            {
                if (color == colors[i])
                {
                    code = i;
                    break;
                }
            }

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