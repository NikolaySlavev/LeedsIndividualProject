#ifndef BUILDINGS_H
#define BUILDINGS_H

#include "buildingblocks.h"

class Buildings {
public:
    Buildings(Layout *layout);
    void drawBuildings();
    void computeBuildings();
private:
    Layout *layout;
    void polygon(point a, point b, point c, point d);
    void cube(std::vector<point> p);
    GLfloat* normal(point a, point b, point c);
    std::vector<std::vector<point>> building_points;
    void drawLine(point s, point e);
};

#endif // BUILDINGS_H
