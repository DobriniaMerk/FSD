#include "Utils.cpp"
#include <iostream>
#include <windows.h>
#include <string.h>

// zpaq
#include "libzpaq.h"
#include <stdio.h>
#include <stdlib.h>
// zpaq

std::wstring getFile();
std::wstring getFolder();


std::ifstream filein;
std::ofstream fileout;
sf::String savefolder;  // folder to save final image in; only ASCII symbols
char tempfile[100];


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
    }  // writes 1 byte 0..255
} out;


int main()
{
    sf::String filename = getFile();   // file to process; only ASCII symbols in path
    tmpnam_s(tempfile, 100);  // create temp filename for intermediate result

    sf::Image img;
    img.loadFromFile(filename.toAnsiString()); // load it


    int colornum;
    std::cout << "Input number of colors (must be a power of 2 and no more than 256): ";
    std::cin >> colornum;

    sf::RenderWindow window(sf::VideoMode(img.getSize().x, img.getSize().y), "Final Image");
    std::vector<sf::Color> colors = ImageDithering::Utils::Dither(img, colornum);
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

            if (event.type == sf::Event::MouseButtonPressed)
            {
                savefolder = getFolder();
                std::cout << "temporary file name: " << tempfile << "\n";

                std::cout << "savefolder: " << savefolder.toAnsiString() << "\n";

                ImageDithering::Utils::SaveToFile(img, colors, tempfile);

                fileout = std::ofstream(savefolder.toAnsiString(), std::ios::out | std::ios::binary | std::ios::trunc);
                filein = std::ifstream(tempfile, std::ios::in | std::ios::binary);

                libzpaq::compress(&in, &out, "53,180,0");  // "0".."5" = faster..better

                window.close();
            }
        }

        window.clear();
        window.draw(s);
        window.display();
    }

    return 0;
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
    ofn.lpstrFilter = L"Image\0*.JPEG;*.JPG;*.PNG;*.BMP;*.TGA;*.PSD;*.HDR;*.PIC\0\0";
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

std::wstring getFolder()
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"FSD Image\0*.fsd\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    std::string s = "";
    std::wstring empty(s.begin(), s.end());

    if (GetSaveFileName(&ofn) == TRUE)
    {
        std::wstring ws(ofn.lpstrFile);
        std::wcout << ws << std::endl;

        if (ws[ws.size() - 4] != '.')  // if saving is broken, it is because of this block
        {
            ws.push_back('.');
            ws.push_back('f');
            ws.push_back('s');
            ws.push_back('d');
        }

        return ws;
    }
    return empty;
}
