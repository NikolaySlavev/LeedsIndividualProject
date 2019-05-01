#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include "MainWidget.h"


class MainWindow : public QWidget {
    public:
        MainWindow(QWidget *parent);
        ~MainWindow();
        QBoxLayout *windowLayout;
        QHBoxLayout *optionLayout = new QHBoxLayout;
        MainWidget *cubeWidget;
        QSlider *nHorSlider;
        QSlider *nVerSlider;
        QSlider *moveXSlider;
        QSlider *moveZSlider;
        QLineEdit *moveNode;
        QLineEdit *changeWidth;
        QPushButton *writeOBJ;
};

#endif // MAINWINDOW_H
