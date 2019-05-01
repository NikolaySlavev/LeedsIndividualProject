#include "buildings.h"
#include <iostream>

using namespace std;

Buildings::Buildings(std::vector<std::vector<point>> *objects_p) {
    this->objects_p = objects_p;

}

void Buildings::computeBuildings() {
    // computes a randomly height buildings using the subdivided parcels
    float height;
    vector<point> new_object;
    for (vector<point> object: *objects_p) {
        new_object = {};
        height = (rand() % 30) + 10;
        for (int i=0; i<object.size(); i++) {
            new_object.push_back({object[i].x, height, object[i].z});
        }
        building_points.push_back(new_object);
    }
}

void Buildings::drawBuildings() {
    //draws the buildings
    float height;
    point p1, p2, p3, p4;
    for (vector<point> object: building_points) {
        height = object[0].y;
        drawConcavePolygon(object, height);
        for (int i=0; i<object.size(); i++) {
            p1 = object[i];
            if (i+1 == object.size())
                p2 = object[0];
            else
                p2 = object[i+1];
            p3 = p2;
            p3.y = 0;
            p4 = p1;
            p4.y = 0;
            polygon(p1,p2,p3,p4);
        }
    }
}

void Buildings::drawConcavePolygon(vector<point> dots, float height) {
    // draws concave polygons used for the floor and roof of the buildings
    GLUtesselator *tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    int vec_size = dots.size();
    GLdouble (*quad)[3] = new GLdouble[vec_size][3];
    glColor3f(1,0,0);
    glNormal3f(0,1,0);
    gluTessBeginPolygon(tess, nullptr);
    gluTessBeginContour(tess);
    for (int i=0; i<dots.size(); i++) {
        quad[i][0] = (double) dots[i].x;
        quad[i][1] = height;
        quad[i][2] = (double) dots[i].z;
        gluTessVertex(tess, quad[i], quad[i]);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
}

void Buildings::polygon(point a, point b, point c, point d) {
  // a polygon for the walls
  glColor3f(1,0,0);
  glNormal3fv(normal(a,b,c));
  glBegin(GL_POLYGON);
    glVertex3f( a.x, a.y, a.z);
    glVertex3f( b.x, b.y, b.z);
    glVertex3f( c.x, c.y, c.z);
    glVertex3f( d.x, d.y, d.z);
  glEnd();

  glColor3f(0, 0, 0);
  drawLine(a, b);
  drawLine(b, c);
  drawLine(c, d);
  drawLine(d, a);
}

// needs 8 points
void Buildings::cube(vector<point> p) {
  // debugging
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
    // draws a line a each edge of the buildings
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(s.x, s.y, s.z);
        glVertex3f(e.x, e.y, e.z);
    glEnd();
}

GLfloat* Buildings::normal(point a, point b, point c) {
  // finds the normal vector
  graphVector vX = {b.x-a.x, b.y-a.y, b.z-a.z};
  graphVector vY = {c.x-a.x, c.y-a.y, c.z-a.z};
  GLfloat n[3] = {vX.y*vY.z - vX.z*vY.y, vX.z*vY.x - vX.x*vY.z, vX.x*vY.y - vX.y*vY.x};

  float length = sqrt(pow(n[0],2) + pow(n[1],2) + pow(n[2],2));
  for (int i=0; i<3; i++)
    n[i] /= length;
  return n;
}
