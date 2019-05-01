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

    // adds a vertical layout to the existing layout in order to define a better structure of the GUI
    windowLayout->addLayout(optionLayout);
    // creates a checkbox which would enable and disable light rotation around the world's sphere
    QCheckBox *checkBoxLight = new QCheckBox("Buildings", this);
    optionLayout->addWidget(checkBoxLight);
    connect (checkBoxLight, SIGNAL(released()), cubeWidget, SLOT(enableBuildings()));

    moveXSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(moveXSlider);
    moveXSlider->setMinimum(0);
    moveXSlider->setMaximum(50);
    connect(moveXSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(moveXNode(int)));

    moveZSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(moveZSlider);
    moveZSlider->setMinimum(0);
    moveZSlider->setMaximum(50);
    connect(moveZSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(moveZNode(int)));

    moveNode = new QLineEdit;
    windowLayout->addWidget(moveNode);
    moveNode->setPlaceholderText("<node_id> you want to move");
    connect(moveNode, SIGNAL(textChanged(QString)), cubeWidget, SLOT(changeMoveNode(QString)));

    changeWidth = new QLineEdit;
    windowLayout->addWidget(changeWidth);
    changeWidth->setPlaceholderText("set width of streets");
    connect(changeWidth, SIGNAL(textChanged(QString)), cubeWidget, SLOT(changeWidthSize(QString)));

    writeOBJ = new QPushButton("OBJ");
    windowLayout->addWidget(writeOBJ);
    connect(writeOBJ, SIGNAL(released()), cubeWidget, SLOT(writeOBJ()));
}

MainWindow::~MainWindow() {
}
