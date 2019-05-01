#include "mainwindow.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {

    // creates a box layout
    windowLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    // create the main scene
    cubeWidget = new MainWidget(this);
    windowLayout->addWidget(cubeWidget);

    // slider for rotating the camera up and down
    nHorSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(nHorSlider);
    nHorSlider->setMinimum(0);
    nHorSlider->setMaximum(360);
    connect(nHorSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(cameraRotateHor(int)));

    // slider for moving the camera up and down
    nVerSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(nVerSlider);
    nVerSlider->setMinimum(0);
    nVerSlider->setMaximum(360);
    connect(nVerSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(cameraRotateVer(int)));

    // adds a vertical layout to the existing layout in order to define a better structure of the GUI
    windowLayout->addLayout(optionLayout);
    // checkbox for toggling the buildings
    QCheckBox *checkBoxLight = new QCheckBox("Buildings", this);
    optionLayout->addWidget(checkBoxLight);
    connect (checkBoxLight, SIGNAL(released()), cubeWidget, SLOT(enableBuildings()));

    // slider for moving a node on the x axis
    moveXSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(moveXSlider);
    moveXSlider->setMinimum(0);
    moveXSlider->setMaximum(50);
    connect(moveXSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(moveXNode(int)));

    // slider for moving a node on the z axis
    moveZSlider = new QSlider(Qt::Horizontal);
    windowLayout->addWidget(moveZSlider);
    moveZSlider->setMinimum(0);
    moveZSlider->setMaximum(50);
    connect(moveZSlider, SIGNAL(valueChanged(int)), cubeWidget, SLOT(moveZNode(int)));

    // text box for defining the node to be moved
    moveNode = new QLineEdit;
    windowLayout->addWidget(moveNode);
    moveNode->setPlaceholderText("<node_id> you want to move");
    connect(moveNode, SIGNAL(textChanged(QString)), cubeWidget, SLOT(changeMoveNode(QString)));

    // text box to change the street's width
    changeWidth = new QLineEdit;
    windowLayout->addWidget(changeWidth);
    changeWidth->setPlaceholderText("set width of streets");
    connect(changeWidth, SIGNAL(textChanged(QString)), cubeWidget, SLOT(changeWidthSize(QString)));

    // button to export the scene as OBJ
    writeOBJ = new QPushButton("OBJ");
    windowLayout->addWidget(writeOBJ);
    connect(writeOBJ, SIGNAL(released()), cubeWidget, SLOT(writeOBJ()));
}

MainWindow::~MainWindow() {
}
