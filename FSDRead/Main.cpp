#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <windows.h>
#include <string.h>

// zpaq
#include "libzpaq.h"
#include <stdio.h>
#include <stdlib.h>
// zpaq

#define _CRT_SECURE_NO_WARNINGS

std::ifstream filein;
std::ofstream fileout;

void libzpaq::error(const char* msg)  // print message and exit
{
    fprintf(stderr, "Oops: %s\n", msg);
    exit(1);
}

class In : public libzpaq::Reader
{
public:
    int offset = 0;
    int get() {
        unsigned char t;
        if (!filein.eof())
        {
            filein.seekg(offset++);
            filein.read((char*)&t, 1);
            std::cout << filein.tellg() << "\n";
            std::cout << "Read: " << (int)t << "\n";
            return t;
        }

        return -1;
    }  // returns byte 0..255 or -1 at EOF
} in;

class Out : public libzpaq::Writer
{
public:
    void put(int c) {
        unsigned char t = c;
        fileout.write((char*)&t, 1);
        std::cout << "Written: " << (int)t << "\n";
    }  // writes 1 byte 0..255
} out;


sf::Image ReadFile(std::string);
std::wstring getFile();

int main(int argc, char** argv)
{
    sf::String filename;

    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i];
    }

    if (argc > 1)
    {
        std::string f = argv[1];
        filename = f;
    }
    else
        filename = getFile();
    

    sf::Image img = ReadFile(filename.toAnsiString());

    sf::RenderWindow window(sf::VideoMode(img.getSize().x, img.getSize().y), "FSD Reader");

    sf::Texture t;
    t.loadFromImage(img);
    sf::Sprite s;
    s.setTexture(t);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(s);
        window.display();
    }

    return 0;
}

sf::Image ReadFile(std::string filename = "out.fsd")
{
    sf::Image img;
    std::ifstream file(filename, std::ios::out | std::ios::binary);
    unsigned int x, y;
    unsigned char colornum;


    file.read((char*)&x, sizeof(unsigned int));
    file.read((char*)&y, sizeof(unsigned int));        // get image x and y size
    file.read((char*)&colornum, sizeof(char));                // number of colors
    img.create(x, y, sf::Color(255, 0, 0));  // filled with red for debugging

    std::cout << "-------------------" << std::endl;
    std::cout << "X: " << x << " Y: " << y << std::endl << "Number of colors: " << (int)colornum << std::endl;
    std::cout << "-------------------" << std::endl;


    std::vector<sf::Color> colors;

    for (int i = 0; i < colornum; i++)
    {
        unsigned char r, g, b;
        file.read((char*)&r, sizeof(char));
        file.read((char*)&g, sizeof(char));
        file.read((char*)&b, sizeof(char));
        std::cout << (int)r << ", " << (int)g << ", " << (int)b << std::endl;
        colors.push_back(sf::Color(r, g, b));
    }

    std::cout << "-------------------" << std::endl;

    unsigned int n = 0;
    sf::Color writeColor;

    while (n < y * x)
    {
        if (file.eof())
            break;

        unsigned char num = 0, code = 0;
        file.read((char*)&num, sizeof(char));
        file.read((char*)&code, sizeof(char));
        writeColor = colors[code];

        for (int i = 0; i < num; i++)
        {
            img.setPixel(n % x, n / x, writeColor);
            n++;
        }

    }

    std::cout << n <<' ' << x * y;

    file.close();

    return img;
}

std::wstring getFile()
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"FSD Image\0 *.FSD\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    std::string s = "";
    std::wstring empty(s.begin(), s.end());

    if (GetOpenFileName(&ofn) == TRUE)
    {
        std::wstring ws(ofn.lpstrFile);
        std::wcout << ws << std::endl;
        return ws;
    }
    return empty;
}
