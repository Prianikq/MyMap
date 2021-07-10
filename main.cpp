#include <QApplication>
#include "rsxffile.h"
#include "drawfunctions.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    const char* FNAME = "M3833.SXF";
    nSXFFile::rSXFFile file;
    file.Read(FNAME);
    file.Print();
    QApplication app(argc, argv);
    nDraw::Draw(file);
    std::cout << "Отрисовка карты завершилась." << std::endl;
    return app.exec();
}
