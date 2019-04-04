#include "buildings.h"
#include <iostream>

using namespace std;

Buildings::Buildings(Layout *layout) {
    this->layout = layout;
}

//void Buildings::computeBuildings() {
//    vector<int> object;
//    vector<point> points;
//    int end;
//    point dot;
//    for (int i=0; i < layout->objects.size(); i++) {
//        object = layout->objects[i];
//        for (int j=0; j < object.size(); j++) {
//            end = j+1;
//            if (j == object.size() - 1)
//                end = 0;
//            dot = layout->edges[object[j]][object[end]].offset_up.pair_p_intersection;
//            points.push_back(dot);
//        }
//        float height = (rand() % 20) + 10;
//        for (int j=0; j < object.size(); j++){
//            point new_p = {points[j].x, points[j].y+height, points[j].z};
//            points.push_back(new_p);
//        }
//        building_points.push_back(points);
//        points = {};
//    }
//}

void Buildings::computeBuildings() {
    float height;
    vector<point> new_object;
    for (vector<point> object: layout->objects_p) {
        new_object = {};
        height = (rand() % 30) + 10;
        for (int i=0; i<object.size(); i++) {
            new_object.push_back({object[i].x, height, object[i].z});
        }
        building_points.push_back(new_object);
    }
}

void Buildings::drawBuildings() {
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
//        OBJ::objfile << "v " + to_string(dots[i].x) + " " + to_string(dots[i].y) + " " + to_string(dots[i].z) + "\n";
//        OBJ::count_obj++;
    }
//    OBJ::objfile << "f";
//    for (int i=OBJ::stopped_count_obj; i<OBJ::count_obj; i++) {
//        OBJ::objfile << " " + to_string(i);
//    }
//    OBJ::objfile << "\n";
//    OBJ::stopped_count_obj = OBJ::count_obj;
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
}

void Buildings::polygon(point a, point b, point c, point d) {
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

//  OBJ::objfile << "v " + to_string(a.x) + " " + to_string(a.y) + " " + to_string(a.z) + "\n";
//  OBJ::objfile << "v " + to_string(b.x) + " " + to_string(b.y) + " " + to_string(b.z) + "\n";
//  OBJ::objfile << "v " + to_string(c.x) + " " + to_string(c.y) + " " + to_string(c.z) + "\n";
//  OBJ::objfile << "v " + to_string(d.x) + " " + to_string(d.y) + " " + to_string(d.z) + "\n";
//  OBJ::count_obj+=4;
//  OBJ::objfile << "f";
//  for (int i=OBJ::stopped_count_obj; i<OBJ::count_obj; i++) {
//      OBJ::objfile << " " + to_string(i);
//  }
//  OBJ::objfile << "\n";
//  OBJ::stopped_count_obj = OBJ::count_obj;
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
    glLineWidth(2);
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
