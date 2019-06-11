#ifndef BUILDINGS_H
#define BUILDINGS_H

#include "buildingblocks.h"

class Buildings {
public:
    Buildings(std::vector<std::vector<point>> *objects_p);
    void drawBuildings();
    void computeBuildings();
    std::vector<std::vector<point>> building_points;

private:
    void polygon(point a, point b, point c, point d);
    void cube(std::vector<point> p);
    GLfloat* normal(point a, point b, point c);
    void drawLine(point s, point e);
    void drawConcavePolygon(std::vector<point> dots, float height, GLUtesselator *tess, GLdouble (*quad)[3]);
    std::vector<std::vector<point>> *objects_p;
};

#endif // BUILDINGS_H
