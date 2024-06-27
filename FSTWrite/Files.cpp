#include "Files.h"

void libzpaq::error(const char* msg)  // print error message and exit
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
    int get() {
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

    void put(int c) {
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

std::string getFile()
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
    ofn.FlagsEx = OFN_EX_NOPLACESBAR; // makes things work but look XP

    std::string s = "";

    if (GetOpenFileName(&ofn) == TRUE)
    {   
         s = (std::string)CW2A(ofn.lpstrFile);
    }
    return s;
}

std::string getNewFile(std::string extention)
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    // black magic begin
    std::string filter = "\0*" + extention + "\0\0";
    ofn.lpstrFilter = std::wstring(filter.begin(), filter.end()).c_str();
    // black magic end

    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.FlagsEx = OFN_EX_NOPLACESBAR; // makes things work but look XP

    std::string s = "";

    if (GetSaveFileName(&ofn) == TRUE)
    {
        s = (std::string)CW2A(ofn.lpstrFile);
        //if (s[s.size() - 4] != '.')  // old version
        if (!std::equal(extention.rbegin(), extention.rend(), s.rbegin()))
            s += extention;
    }
    return s;
}
