#include <Windows.h>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // creates the window
    MainWindow *window = new MainWindow(nullptr);
    // with the following size
    window->resize(812, 612);
    window->show();
    app.exec();
    delete window;
    return 0;
    } // main()
