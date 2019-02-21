#include "mainwindow.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {

    // create the window layout
    windowLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    // create main widget
    cubeWidget = new MainWidget(this);
    windowLayout->addWidget(cubeWidget);

    // creates a horizontal slider used for world's rotations on the Y axis
    nHorSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(nHorSlider);
    nHorSlider->setMinimum(0);
    nHorSlider->setMaximum(360);
    connect(nHorSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(cameraRotateHor(int)));

    nVerSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(nVerSlider);
    nVerSlider->setMinimum(0);
    nVerSlider->setMaximum(360);
    connect(nVerSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(cameraRotateVer(int)));

}

MainWindow::~MainWindow() {
}
