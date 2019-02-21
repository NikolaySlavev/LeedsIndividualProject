#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QBoxLayout>
#include <QSlider>
#include "MainWidget.h"


class MainWindow : public QWidget {
    public:
        MainWindow(QWidget *parent);
        ~MainWindow();
        QBoxLayout *windowLayout;
        MainWidget *cubeWidget;
        QSlider *nHorSlider;
        QSlider *nVerSlider;



};

#endif // MAINWINDOW_H
