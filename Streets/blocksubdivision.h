#ifndef BLOCKSUBDIVISION_H
#define BLOCKSUBDIVISION_H

#include <iostream>
#include <vector>
#include "structures.h"
#include "junction.h"

class BlockSubdivision {
public:
    BlockSubdivision();
    std::vector<point> convexHull(std::vector<point> points);
    rectangle smallestRectangle(std::vector<point> convex);
    void drawConvexHull();
    std::vector<std::vector<point>> convex_hulls;
    std::vector<rectangle> smallest_rect;
    void drawSmallestRectangle();
    void findSmallestRectangles();
    std::vector<std::vector<point>> divideRect(rectangle rect, std::vector<point> dots);
    std::vector<std::vector<point>> allSubdivision(std::vector<std::vector<point>> objects_p);
private:
    static point p0;
    static int compare(const void *vp1, const void *vp2);
    static int orientation(point p, point q, point r);
    static int distSq(point p1, point p2);
    static point nextToTop(std::vector<point> &S);
    static int swap(point &p1, point &p2);
    graphVector Perp(graphVector vec);
    graphVector Normalise(graphVector vec);
    int Dot(graphVector vec1, graphVector vec2);
    float pointDistance(graphVector n, point a, point p);
    std::vector<std::vector<point>> subdivision(std::vector<point> concave_p, bool first);
    float convexArea(std::vector<point> convex);
};

#endif // BLOCKSUBDIVISION_H
