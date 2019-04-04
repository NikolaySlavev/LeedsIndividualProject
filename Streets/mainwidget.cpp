#include <Windows.h>
#include <iostream>
#include <QtGui>
#include "MainWidget.h"
#include <algorithm>

using namespace std;

MainWidget::MainWidget(QWidget *parent):QGLWidget(parent) {
}

// called when OpenGL context is set up
void MainWidget::initializeGL() {
    setFocusPolicy(Qt::StrongFocus);
    glClearColor(0.3, 0.3, 0.3, 0.0);
    layout->gridStraight(-40, 3, 10);
    node node1 = {layout->count_node_id++, -20, 0, -10}, node2 = {layout->count_node_id++, 0, 0, -10}, node3 = {layout->count_node_id++, 0, 0, 10};
    vector<vector<node>> inputs = {{node1, node2},
                                   {node2, node3},
                                   {node1, node3},
                                   {node1, layout->nodes[8]},
                                   {node1, layout->nodes[5]},
                                   {node2, layout->nodes[6]}
                                  };

//    node node0 = {layout->count_node_id++, -8.537976, 0, -8.540727};
//    node node1 = {layout->count_node_id++, -50, 0, -50};
//    node node2 = {layout->count_node_id++, -7.590774, 0, -7.590873};
//    node node3 = {layout->count_node_id++, -38.532714, 0, 24.308835};
//    vector<vector<node>> inputs = {{node0, node2},
//                                   {node1, node0},
//                                   {node2, node1},
//                                   {node3, node2},
//                                   {node0, node},
//                                  };

    layout->inputLayout(inputs);
//    OBJ::objfile.open("D:/Uni/Individual Project/SourceCode/LeedsIndividualProject/build-Streets-QT_VSdebug-Debug/testobj.obj", fstream::out);
//    fstream new_obj;
//    new_obj.open("aaa.txt", fstream::out);
//    new_obj << "test\n";
//    new_obj.close();
//    layout->fileLayout("example.csv");
    street = new DrawStreet();
    junction = new Junction(layout, street);
    blocks = new BuildingBlocks(layout, street, junction);
    buildings = new Buildings(layout);
    subdivision = new BlockSubdivision(junction);
    blocks->findBlocks();
    junction->addPairs();
    junction->findClosestIntersections();
    junction->findJunctionObjects();
    blocks->removeLargest();
    blocks->computeDrawableBlocks();
    for (int i=0; i<layout->objects_p.size(); i++) {
        subdivision->convexHull(layout->objects_p[i]);
    }
    subdivision->findSmallestRectangles();
    layout->objects_p = subdivision->allSubdivision(layout->objects_p);
    buildings->computeBuildings();
}

void MainWidget::cameraRotateHor(int value) { horVal = value; update(); }

void MainWidget::cameraRotateVer(int value) { verVal = value; update(); }

void MainWidget::enableBuildings() { enable_buildings = !enable_buildings; update(); }

void MainWidget::changeMoveNode(QString value) {
    move_node = value.toInt();
}

void MainWidget::moveZNode(int value) {
    if (value > prev_Zvalue)
        layout->nodes[move_node].z--;
    else
        layout->nodes[move_node].z++;
    prev_Zvalue = value;
    updateMove();
}
void MainWidget::moveXNode(int value) {
    if (value > prev_Xvalue)
        layout->nodes[move_node].x--;
    else
        layout->nodes[move_node].x++;
    prev_Xvalue = value;
    updateMove();
}

void MainWidget::updateMove() {

    edge_axis changed_edge;
    for (auto const& edge: layout->edges[move_node]) {
        changed_edge = street->computeStraightStreet(layout->nodes[move_node], layout->nodes[edge.first]);
        layout->edges[move_node][edge.first] = changed_edge;
        changed_edge = street->computeStraightStreet(layout->nodes[edge.first], layout->nodes[move_node]);
        layout->edges[edge.first][move_node] = changed_edge;
    }

    street = new DrawStreet();
    junction = new Junction(layout, street);
    blocks = new BuildingBlocks(layout, street, junction);
    buildings = new Buildings(layout);
    subdivision = new BlockSubdivision(junction);

    layout->objects = {};
    layout->objects_p = {};
    blocks->findBlocks();
    junction->addPairs();
    junction->findClosestIntersections();
    junction->findJunctionObjects();
    blocks->removeLargest();
    blocks->computeDrawableBlocks();
    for (int i=0; i<layout->objects_p.size(); i++) {
        subdivision->convexHull(layout->objects_p[i]);
    }
    subdivision->findSmallestRectangles();
    layout->objects_p = subdivision->allSubdivision(layout->objects_p);
    buildings->computeBuildings();
    update();
}

void MainWidget::resizeGL(int w, int h) { // resizeGL()
    int offset;
    if (w > h) {
        offset = (w - h)/2;
        glViewport(offset, 0, h, h);
    } else {
        offset = (h-w)/2;
        glViewport(0, offset, w, w);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-200.0, 200.0, -50.0, 200.0, -200, 200.0);
    glFrustum(-0.1, 0.1, -0.2, 0.2, 0.1, 500);

}

void MainWidget::keyPressEvent(QKeyEvent *keyEvent) {
    cout << "OKOKO" << endl;
    float fraction = 5.0f;

    switch (keyEvent->key()) {
    case Qt::Key_Down:
        cout << "DOWN" << endl;
        x -= lx * fraction;
        z -= lz * fraction;
        break;
    case Qt::Key_Up:
        cout << "UP" << endl;
        x += lx * fraction;
        z += lz * fraction;
        break;
    case Qt::Key_Left:
        angle -= 0.1f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    case Qt::Key_Right:
        angle += 0.1f;
        lx = sin(angle);
        lz = -cos(angle);
        break;
    }
    update();
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(horVal,1,0,0);
//    glRotatef(verVal,0,1,0);

    cout << x << " " <<  z << " " << lx << " " << lz << endl;
    gluLookAt(	x, verVal, z,
                x+lx, verVal,  z+lz,
                0.0f, 1.0f,  0.0f);

//    gluLookAt(0,0,1, 0.0,0.0,0.0, 0.0,1.0,0.0);

    glPushMatrix();
    glColor3f(0.5,0.5,0.5);

    for (auto const& edges : layout->edges) {
       for (auto const& edge : edges.second) {
           if (edge.second.end.id > edge.second.start.id) {
               int start = edge.second.start.id;
               int end = edge.second.end.id;
               edge_axis rev_edge = layout->edges[end][start];
               if (edge.second.control.empty()) {
                   float first_t = 0, second_t = 1;
                   if (!isnan(rev_edge.offset_down.closest_t_intersection))
                       first_t = 1 - rev_edge.offset_down.closest_t_intersection;
                   if (!isnan(edge.second.offset_down.closest_t_intersection))
                       second_t = edge.second.offset_down.closest_t_intersection;
                   street->drawStraightStreet(edge.second, first_t, second_t);
               } else {
                   int i_end = edge.second.offset_up.closest_i_intersection;
                   vector<point> p_end = {edge.second.offset_up.closest_p_intersection, edge.second.offset_down.closest_p_intersection};
                   vector<point> p_start = {rev_edge.offset_up.closest_p_intersection, rev_edge.offset_down.closest_p_intersection};
                   int i_start = (rev_edge.offset_up.dots.size() - 1) - rev_edge.offset_up.closest_i_intersection;
                   street->drawCurvedStreet(edge.second, p_start, p_end, i_start, i_end);
               }
           }
        }
    }

    glPushMatrix();
    for (auto const& edges : layout->edges) {
       for (auto const& edge : edges.second) {
           glColor3f(1,0,0);
           glPointSize(4);
           glBegin(GL_POINTS);
           if (!isnan(edge.second.offset_down.pair_t_intersection)) {
                point p = edge.second.offset_down.pair_p_intersection;
                glVertex3f(p.x, 2, p.z);
           }
            point p2 = edge.second.offset_down.closest_p_intersection;
            point p3 = edge.second.offset_up.closest_p_intersection;
            //cout << "P.X" << p.x << " " << p.z << endl;

            glColor3f(0,1,0);
            //glVertex3f(p2.x, 2, p2.z);
            //glVertex3f(p3.x, 2, p3.z);
           glEnd();
       }
    }
    glPopMatrix();
    glPushMatrix();
    blocks->drawBlocks();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0,0,1);
    junction->drawJunctions();
    glPopMatrix();

    cout << "OK1" << endl;

    if (enable_buildings) {
        glPushMatrix();
        buildings->drawBuildings();
        glPopMatrix();
    }
    cout << "OK2" << endl;
    glPushMatrix();
    subdivision->drawConvexHull();
    glPopMatrix();

    glPushMatrix();
    subdivision->drawSmallestRectangle();
    glPopMatrix();
//    OBJ::objfile.close();

    glFlush();
}
