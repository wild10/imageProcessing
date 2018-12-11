#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w; // ventana principal
    w.show(); // iniciar ventana

    // ejecutar la clase principal
    return a.exec();
}
