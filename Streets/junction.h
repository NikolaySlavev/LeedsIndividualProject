#ifndef JUNCTION_H
#define JUNCTION_H

#include <Windows.h>
#include "structures.h"
#include <GL/glu.h>
#include "layout.h"
#include <math.h>


class Junction {
    public:
        Junction(Layout *layout, DrawStreet *street);
        std::vector<float> findIntersection(edge_offset e1, edge_offset e2);
        void assignPair(edge_offset *offset, float t, point p);
        void addPair(int start, int mid, int end);
        void findClosestIntersections();
        void findJunctionObjects();
        void addPairs();
        void drawJunctions();

    private:
        void addClosestIntersection(float closest_t, edge_offset *offset);
        void addJunctionLine(int start, int end, std::vector<int> target_id);
        Layout *layout;
        DrawStreet *street;
        std::map<int, std::vector<curved_edge_axis>> junction_objects;
};

#endif // JUNCTION_H
