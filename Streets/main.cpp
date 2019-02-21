#include <Windows.h>
#include "mainwindow.h"
#include <QApplication>
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow *window = new MainWindow(NULL);
    window->resize(812, 612);
    window->show();
    app.exec();
    delete window;
    return 0;
    } // main()
