#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#define bp char BREAKPOINT = '1'

namespace ImageDithering
{
    static class Utils
    {
        static sf::Color Divide(sf::Color self, sf::Uint8 n)
        {
            return sf::Color((self.r / n), (self.g / n), (self.b / n));
        }


        static sf::Color Multiply(sf::Color self, sf::Uint8 n)
        {
            return sf::Color((self.r * n), (self.g * n), (self.b * n));
        }

        static sf::Color Add(sf::Color self, sf::Color other)
        {
            return sf::Color(clamp(self.r + other.r, 0, 255), clamp(self.g + other.g, 0, 255), clamp(self.b + other.b, 0, 255));
        }

        static int clamp(int val, int min, int max)
        {
            return std::max(min, std::min(max, val));
        }

        /// <summary>
        /// Quatization by median cut
        /// </summary>
        /// <param name="img">Source image</param>
        /// <param name="colorNum">Number of colors to return; Must be a power of two</param>
        /// <returns>Array of Color[colorNum]</returns>
        static std::vector <sf::Color> QuantizeMedian(sf::Image img, int colorNum)
        {
            auto s = img.getSize();

            int skip = 10;
            int filledRows = 1;

            std::vector< std::vector<sf::Color> > oldColors(colorNum);
            std::vector< std::vector<sf::Color> > newColors(colorNum);
            std::vector< std::vector<sf::Color> > t;

            for (int i = 0; i < s.x; i += skip)  // set first array of oldColors to img pixels, with interval of skip
                for(int j = 0; j < s.y; j += skip)
                    oldColors[0].push_back(img.getPixel(i, j));


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

            std::vector<sf::Color> ret(colorNum);  // colors to return
            sf::Vector3f sum = sf::Vector3f(0, 0, 0);
            sf::Color c;
            float n;

            for (int i = 0; i < colorNum; i++)  // calculate mean color of each array and return them
            {
                n = 0;
                sum.x = 0;
                sum.y = 0;
                sum.z = 0;

                for(int j = 0; j < oldColors[i].size(); j++)
                {
                    c = oldColors[i][j];
                    sum.x += c.r;
                    sum.y += c.g;
                    sum.z += c.b;
                    n++;
                }

                sum = sum / n;
                ret[i] = sf::Color(sum.x, sum.y, sum.z);
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
        static std::vector<std::vector<sf::Color> > QuantizeMedianSplit(std::vector<sf::Color> _colors)
        {
            std::vector<std::vector<sf::Color> > ret(2);
            std::vector<sf::Color> colors = _colors;
            sf::Color c;
            int r = 0, g = 0, b = 0;

            for (int i = 0; i < colors.size(); i++)
            {
                c = colors[i];
                r += c.r;
                g += c.g;
                b += c.b;
            }

            if (r > g && r > b)
                std::sort(colors.begin(), colors.end(), [](sf::Color x, sf::Color y) { return x.r < y.r; });
            else if (g > r && g > b)
                std::sort(colors.begin(), colors.end(), [](sf::Color x, sf::Color y) { return x.g < y.g; });
            else if (b > r && b > g)
                std::sort(colors.begin(), colors.end(), [](sf::Color x, sf::Color y) { return x.b < y.b; });


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
        static std::vector<sf::Color> Quantize(sf::Image img, int colorNum)
        {
            std::vector<sf::Color> means(colorNum);
            sf::Color color;
            sf::Vector3f sum = sf::Vector3f(0, 0, 0);
            int n, num;

            std::srand(std::time(nullptr));

            means = QuantizeMedian(img, colorNum);

            for (int i = 0; i < 100; i++)
            {
                num = 0;

                //foreach(sf::Color mean in means)
                for(int j = 0; j < colorNum; j++)
                {
                    sum = sum * 0.0f;
                    n = 0;
                    auto s = img.getSize();
                    int imgSize = s.x * s.y;
                    
                    for (int k = 1; k < imgSize; k += 30)
                    {
                        color = sf::Color(img.getPixel(k%s.x, k/s.x));
                        if (GetNearest(color, means, 250) == means[j])
                        {
                            sum.x += color.r;
                            sum.y += color.g;
                            sum.z += color.b;
                            n++;
                        }
                    }

                    if (n != 0)
                    {
                        sum /= (float)n;
                        means[num] = sf::Color(sum.x, sum.y, sum.z);
                    }
                    num++;
                }
            }

            std::cout << "----------------------" << std::endl;
            for (int i = 0; i < means.size(); i++)
                std::cout << (int)means[i].r << ", " << (int)means[i].g << ", " << (int)means[i].b << std::endl;


            return means;
        }


        static float DistanceTo(sf::Color self, sf::Color other)  // to get proper distance you need sqare root of result; not using for optimisation
        {
            return (self.r - other.r) * (self.r - other.r) + (self.g - other.g) * (self.g - other.g) + (self.b - other.b) * (self.b - other.b);
        }


        /// <summary>
        /// Searchs nearest but not farther than maxDist color to color in search array
        /// </summary>
        /// <param name="color">Base color</param>
        /// <param name="search">Array for searching in</param>
        /// <param name="maxDist">Maximum distance of nearest color</param>
        /// <returns>Color</returns>
        static sf::Color GetNearest(sf::Color color, std::vector<sf::Color> search, int maxDist)
        {
            float dist = -1, tDist = 0;
            sf::Color ret = color;
            sf::Color c;

            //foreach (sf::Color c in search)
            for(int i = 0; i < search.size(); i++)
            {
                c = search[i];
                tDist = DistanceTo(color, c);

                if (tDist < maxDist && (dist == -1 || tDist < dist))
                {
                    dist = tDist;
                    ret = c;
                }
            }

            return ret;
        }

        static sf::Image AddDebug(sf::Image &image, std::vector<sf::Color> colors)
        {
            sf::Image img;
            img.create(image.getSize().x, image.getSize().y*1.2, sf::Color(255, 0, 0));

            int blocksize = img.getSize().x / colors.size();

            for (int i = 0; i < img.getSize().x * img.getSize().y; i++)
            {
                int x = i % img.getSize().x;
                int y = i / img.getSize().x;

                if (y < image.getSize().y)
                    img.setPixel(x, y, image.getPixel(x, y));
                else
                    img.setPixel(x, y, colors[(x / blocksize + (y - image.getSize().y) / blocksize) % colors.size()]);
            }
            return img;
        }

        public:
            static std::vector<sf::Color> Dither(sf::Image &image, int colorDepth)
            {
                std::vector<sf::Color> colors;
                colors = Quantize(image, colorDepth);

                for (int x = 0; x < image.getSize().x; x++)
                {
                    for (int y = 0; y < image.getSize().y; y++)
                    {
                        sf::Color pix = image.getPixel(x, y);

                        sf::Color wanted = GetNearest(pix, colors, 100000000);
                        

                        image.setPixel(x, y, wanted);

                        sf::Color error = sf::Color(clamp(pix.r - wanted.r, 0, 255), clamp(pix.g - wanted.g, 0, 255), clamp(pix.b - wanted.b, 0, 255));
                       
                        if (x < image.getSize().x - 1)  //  error distribution;  if's are almost black magic so do not touch without need
                            image.setPixel(x + 1, y, Add(Multiply(error, 1 / 7), image.getPixel(x + 1, y)));
                        if (y < image.getSize().y - 1)
                        {
                            if (x < image.getSize().x - 1)
                                image.setPixel(x + 1, y + 1, Add(Multiply(error, 1 / 1), image.getPixel(x + 1, y + 1)));
                            image.setPixel(x, y + 1, Add(Multiply(error, 1 / 5), image.getPixel(x, y + 1)));
                            image.setPixel(x - 1, y + 1, Add(Multiply(error, 1 / 3), image.getPixel(x - 1, y + 1)));
                        }
                    }
                }

                image = AddDebug(image, colors);

                return colors;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="img">Image to save</param>
            /// <param name="path">Path to saved image</param>
            static void SaveToFile(sf::Image img, std::vector<sf::Color> colors, std::string filename)
            {
                std::ofstream filestream(filename, std::ios::in|std::ios::binary|std::ios::trunc);  // std::ios::trunc is for writing file over instead of appending
                sf::Vector2u size = img.getSize();
                filestream.write((char*)&size.x, sizeof(unsigned int));  // first 4 bytes is x of image
                filestream.write((char*)&size.y, sizeof(unsigned int));  // second 4 bytes is y

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


                sf::Color pixelColor, color = img.getPixel(0, 0);  // write first pixel in memory
                unsigned char rowLength = 1;

                unsigned int x, y;
                unsigned char code;
                unsigned char maxrow = 254;

                int counter = 0;

                for (int n = 1; n < size.x * size.y; n++)
                {
                    counter++;
                    
                    x = n % size.x;
                    y = n / size.x;

                    pixelColor = img.getPixel(x, y);
                    if (pixelColor == color && rowLength < maxrow)    // if current pixel color matches color of row     // 255 is reserved
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
    };
}
