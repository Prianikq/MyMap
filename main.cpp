#include "rsxffile.h"
#include "drawfunctions.hpp"
#include <iostream>

int main() {
    setlocale(LC_ALL, "Russian");
    const char* FNAME = "M3833.SXF";
    nSXFFile::rSXFFile file;
    file.Read(FNAME);
    file.Print();
    nDraw::Draw(file);
    return 0;
}
