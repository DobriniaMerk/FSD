#include "Utils.cpp"
#include <iostream>

#include <windows.h>
#include <string.h>

std::wstring getFile();
std::wstring getFolder();

int main()
{
    sf::String filename = getFile();
    

    sf::Image img;
    img.loadFromFile(filename.toAnsiString());

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
                sf::String savefolder = getFolder();
                ImageDithering::Utils::SaveToFile(img, colors, savefolder.toAnsiString());
                window.close();
            }

            //  DELETE THIS
            if (event.type == sf::Event::KeyPressed)
            {
                img.saveToFile("out.jpg");
                window.close();
            }
            //  DELETE THIS
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
        return ws;
    }
    return empty;
}
