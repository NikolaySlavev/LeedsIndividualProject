#include "buildings.h"
#include <iostream>

using namespace std;

Buildings::Buildings(Layout *layout) {
    this->layout = layout;
}

void Buildings::computeBuildings() {
    vector<int> object;
    vector<point> points;
    int end;
    point dot;
    for (int i=0; i < layout->objects.size(); i++) {
        object = layout->objects[i];
        for (int j=0; j < object.size(); j++) {
            end = j+1;
            if (j == object.size() - 1)
                end = 0;
            dot = layout->edges[object[j]][object[end]].offset_up.pair_p_intersection;
            points.push_back(dot);
        }
        float height = (rand() % 20) + 10;
        for (int j=0; j < object.size(); j++){
            point new_p = {points[j].x, points[j].y+height, points[j].z};
            points.push_back(new_p);
        }
        building_points.push_back(points);
        points = {};
    }

}

void Buildings::drawBuildings() {
    for (vector<point> points: building_points){
        cube(points);
    }
}

void Buildings::polygon(point a, point b, point c, point d) {
  glNormal3fv(normal(a,b,c));
  glBegin(GL_POLYGON);
    glVertex3f( a.x, a.y, a.z);
    glVertex3f( b.x, b.y, b.z);
    glVertex3f( c.x, c.y, c.z);
    glVertex3f( d.x, d.y, d.z);
  glEnd();
}

// needs 8 points
void Buildings::cube(vector<point> p) {
  glColor3f(1, 0.5, 0.5);
  polygon(p[3], p[2], p[1], p[0]);
  polygon(p[1], p[2], p[6], p[5]);
  polygon(p[2], p[3], p[7], p[6]);
  polygon(p[0], p[4], p[7], p[3]);
  polygon(p[0], p[1], p[5], p[4]);
  polygon(p[4], p[5], p[6], p[7]);

  glColor3f(0, 0, 0);
  drawLine(p[3], p[2]);
  drawLine(p[2], p[1]);
  drawLine(p[1], p[0]);
  drawLine(p[2], p[6]);

  drawLine(p[6], p[5]);
  drawLine(p[3], p[7]);
  drawLine(p[7], p[6]);
  drawLine(p[0], p[4]);

  drawLine(p[4], p[7]);
  drawLine(p[1], p[5]);
  drawLine(p[5], p[4]);
  drawLine(p[0], p[3]);

}

void Buildings::drawLine(point s, point e) {
    glBegin(GL_LINES);
        glVertex3f(s.x, s.y, s.z);
        glVertex3f(e.x, e.y, e.z);
    glEnd();
}

GLfloat* Buildings::normal(point a, point b, point c) {
  graphVector vX = {b.x-a.x, b.y-a.y, b.z-a.z};
  graphVector vY = {c.x-a.x, c.y-a.y, c.z-a.z};
  GLfloat n[3] = {vX.y*vY.z - vX.z*vY.y, vX.z*vY.x - vX.x*vY.z, vX.x*vY.y - vX.y*vY.x};

  float length = sqrt(pow(n[0],2) + pow(n[1],2) + pow(n[2],2));
  for (int i=0; i<3; i++)
    n[i] /= length;
  return n;
}
