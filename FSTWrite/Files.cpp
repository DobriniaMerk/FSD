#include "libzpaq.h"
#include "tinyfiledialogs.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>


void libzpaq::error(const char *msg) // print error message and exit
{
  fprintf(stderr, "Oops: %s\n", msg);
  exit(1);
}

class In : public libzpaq::Reader
{
public:
    std::ifstream* readfile;

    In(std::ifstream* f)
    {
        readfile = f;
    }

    int offset = 0;
    int get()
    {
        unsigned char t;
        if (!readfile->eof())
        {
            readfile->seekg(offset++);
            readfile->read((char*)&t, 1);
            return t;
        }

        return -1;
    }  // returns byte 0..255 or -1 at EOF
};

class Out : public libzpaq::Writer
{
public:
    std::ofstream* writefile;

    Out(std::ofstream* f)
    {
        writefile = f;
    }

    void put(int c)
    {
        unsigned char t = c;
        writefile->write((char*)&t, 1);
    }  // writes 1 byte 0..255
};

void compress(std::string source, std::string dest)
{
    std::ifstream filein = std::ifstream(source, std::ios::in | std::ios::binary);
    std::ofstream fileout = std::ofstream(dest, std::ios::out | std::ios::binary | std::ios::trunc);
    In in(&filein);
    Out out(&fileout);
    libzpaq::compress(&in, &out, "53,180,0");  // "0".."5" = faster..better;  // after half a year of not touching this, "53,180,0" look like some magic numbers
}

std::string getImageFile()
{
    char const* title = "Pick an image";
    char const* defaultPath = NULL;
    int patternNum = 3;
    char const* filterPatterns[3] = {"*.png", "*.jpg", "*.jpeg"};
    char const* filterDescr = "Image files";
    int allowMultiple = 0;

    char* result = tinyfd_openFileDialog(title, defaultPath, patternNum,
                                         filterPatterns, filterDescr, allowMultiple);

    if (result == NULL) {
        // user canceled
        return "";
    }
    return result;
}

char const* saveFile(char const* defaultpath = nullptr, char const** filterpatterns = nullptr, char const* filterdescr = nullptr) {
    char const* title = "Save file";
    int patternNum = 1;
    char const* defaultPath = defaultpath ? defaultpath : "";
    char const* t = "*.fsd";
    char const** filterPatterns = filterpatterns ? filterpatterns : &t;
    char const* filterDescr = filterdescr ? filterdescr : "FSD images";

    char *result = tinyfd_saveFileDialog(title, defaultPath, patternNum,
                                         filterPatterns, filterDescr);
    if (result == NULL) {
        // user canceled
        return "";
    }
    return result;
}
