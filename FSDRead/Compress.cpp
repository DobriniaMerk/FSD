#include "Compress.h"

class Compress
{
    static std::string Decompress(std::string path)
    {
        char tempfile[100];

    /* image decompression
        *  handle later
        sf::String filename;
        tmpnam_s(tempfile, 100);

        for (int i = 0; i < argc; i++)
            std::cout << argv[i];

        if (argc > 1)
        {
            std::string f = argv[1];
            filename = f;
        }
        else
            filename = getFile();

        filein = std::ifstream(filename.toAnsiString(), std::ios::in | std::ios::binary);
        fileout = std::ofstream(tempfile, std::ios::out | std::ios::binary | std::ios::trunc);

        libzpaq::decompress(&in, &out);

        sf::Image img = ReadFile(tempfile);

        sf::Image out_img;
        out_img.create(img.getSize().x, img.getSize().y / 1.2, sf::Color::Red);
        for (int i = 0; i < out_img.getSize().x * out_img.getSize().y; i++)
        {
            int x = i % out_img.getSize().x;
            int y = i / out_img.getSize().x;

            out_img.setPixel(x, y, img.getPixel(x, y));
        }
        */
    }
    
};