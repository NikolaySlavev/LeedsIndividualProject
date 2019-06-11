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
    layout = new Layout();
}

void MainWidget::cameraRotateHor(int value) { horVal = value; update(); }

void MainWidget::cameraRotateVer(int value) { verVal = value; update(); }

void MainWidget::enableBuildings() { enable_buildings = !enable_buildings; update(); }

void MainWidget::changeMoveNode(QString value) {
    move_node = value.toInt();
    updateMove();
}

void MainWidget::changeWidthSize(QString value) {
    width = value.toFloat();
    layout->updateWidthSize(width);
    update();
}

void MainWidget::writeOBJ() {
    OBJ *obj = new OBJ(layout);
    obj->writeObj();
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
    layout->updateMove(move_node);
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
    //glOrtho(-300.0, 300.0, -300.0, 300.0, -300, 300.0);
    glFrustum(-0.1, 0.1, -0.2, 0.2, 0.1, 500);

}

void MainWidget::keyPressEvent(QKeyEvent *keyEvent) {
    float fraction = 5.0f;
    switch (keyEvent->key()) {
    case Qt::Key_Down:
        x -= lx * fraction;
        z -= lz * fraction;
        break;
    case Qt::Key_Up:
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

    gluLookAt(x, verVal, z, x+lx, verVal, z+lz, 0.0f, 1.0f, 0.0f);

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
                   layout->street->drawStraightStreet(edge.second, first_t, second_t);
               } else {
                   int i_start = 0;
                   int i_end = edge.second.offset_down.dots.size()-1;
                   vector<point> p_end = {};
                   vector<point> p_start = {};
                   if (!isnan(edge.second.offset_up.closest_t_intersection)) {
                        i_end = edge.second.offset_up.closest_i_intersection;
                        p_end = {edge.second.offset_up.closest_p_intersection, edge.second.offset_down.closest_p_intersection};
                   }
                    if (!isnan(rev_edge.offset_up.closest_t_intersection)) {
                       i_start = (rev_edge.offset_up.dots.size() - 1) - rev_edge.offset_up.closest_i_intersection;
                       p_start = {rev_edge.offset_up.closest_p_intersection, rev_edge.offset_down.closest_p_intersection};
                    }

                   layout->street->drawCurvedStreet(edge.second, p_start, p_end, i_start, i_end);
               }
           }
        }
    }

    glPushMatrix();
    for (auto const& edges : layout->edges) {
       for (auto const& edge : edges.second) {
           glColor3f(1,0,0);
           glPointSize(6);
           glBegin(GL_POINTS);
           if (!isnan(edge.second.offset_down.pair_t_intersection)) {
                point p = edge.second.offset_down.pair_p_intersection;
                //glVertex3f(p.x, 1, p.z);
           }
            point p2 = edge.second.offset_down.closest_p_intersection;
            point p3 = edge.second.offset_up.closest_p_intersection;
            //cout << "P.X" << p.x << " " << p.z << endl;

            //glColor3f(0,1,0);
            //glVertex3f(p2.x, 1, p2.z);
            //glVertex3f(p3.x, 1, p3.z);



//            float closest_t = 0.45;
//            point closest_p = {(1-closest_t)*test1.x + closest_t*test2.x,
//                               (1-closest_t)*test1.y + closest_t*test2.y,
//                               (1-closest_t)*test1.z + closest_t*test2.z};
//            glColor3f(0,0,0);
//            glVertex3f(test1.x, 1, test1.z);
//            glVertex3f(test2.x, 1, test2.z);
//            glVertex3f(closest_p.x, 1, closest_p.z);

//            for (auto test: layout->edges[1][0].offset_down.dots) {
//                glVertex3f(test.x, 1, test.z);
//            }
//            for (auto test: layout->edges[1][0].offset_up.dots) {
//                glVertex3f(test.x, 1, test.z);
//            }

           glEnd();
       }
    }
    glPopMatrix();
    glPushMatrix();
    layout->blocks->drawBlocks();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5,0.5,0.5);
    //glColor3f(1,1,1);
    layout->junction->drawJunctions();
    glPopMatrix();

    if (enable_buildings) {
        glPushMatrix();
        layout->buildings->drawBuildings();
        glPopMatrix();
    }
    glPushMatrix();
    //layout->subdivision->drawConvexHull();
    glPopMatrix();

    glPushMatrix();
    //layout->subdivision->drawSmallestRectangle();
    glPopMatrix();

    glFlush();
}
