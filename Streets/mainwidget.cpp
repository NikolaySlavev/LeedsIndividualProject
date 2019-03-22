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
    glClearColor(0.3, 0.3, 0.3, 0.0);
    layout->gridCurved(-40, 3, 10);
    node node1 = {layout->count_node_id++, -20, 0, -10}, node2 = {layout->count_node_id++, 0, 0, -10}, node3 = {layout->count_node_id++, 0, 0, 10};
    vector<vector<node>> inputs = {{node1, node2},
                                   {node2, node3},
                                   {node1, node3},
                                   {node1, layout->nodes[8]},
                                   {node1, layout->nodes[5]},
                                   {node2, layout->nodes[6]}
                                  };
    layout->inputLayout(inputs);
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
    buildings->computeBuildings();
    cout << "AA: " << layout->objects_p[0].size() << endl;
    for (int i=0; i<layout->objects_p.size(); i++) {
        subdivision->convexHull(layout->objects_p[i]);
    }
    subdivision->findSmallestRectangles();
    layout->objects_p = subdivision->allSubdivision(layout->objects_p);
}

void MainWidget::cameraRotateHor(int value) { horVal = value; update(); }

void MainWidget::cameraRotateVer(int value) { verVal = value; update(); }

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
    glOrtho(-50.0, 50.0, -50.0, 50.0, -50.0, 50.0);
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

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

//    glPushMatrix();
//    buildings->drawBuildings();
//    glPopMatrix();

    glPushMatrix();
    subdivision->drawConvexHull();
    glPopMatrix();

    glPushMatrix();
    subdivision->drawSmallestRectangle();
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(horVal,1,0,0);
    glRotatef(verVal,0,1,0);

    gluLookAt(0,0,1, 0.0,0.0,0.0, 0.0,1.0,0.0);
    glFlush();
}
