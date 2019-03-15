#ifndef DRAWSTREET_H
#define DRAWSTREET_H

#include <Windows.h>
#include "structures.h"
#include <GL/glu.h>


class DrawStreet {
public:
    DrawStreet();
    void drawStraightStreet(edge_axis street, float t_start = 0, float t_end = 1);
    edge_axis computeStraightStreet(node start, node end);
    graphVector findVector(point s, point e);
    float findLength(graphVector line_vector);
    std::vector<graphVector> findNormals(graphVector line_vector, float line_length);
    point Lerp(point p1, point p2, float t);
    void drawCurvedStreet(edge_axis street);
    void drawStraightLine(edge_axis street);
    std::vector<point> drawQuadCurvedLine(edge_axis street);

private:
    int count_offset_edges_id = 0;
};

#endif // DRAWSTREET_H
