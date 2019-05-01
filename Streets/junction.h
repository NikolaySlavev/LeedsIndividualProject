#ifndef JUNCTION_H
#define JUNCTION_H

#include <Windows.h>
#include "structures.h"
#include <GL/glu.h>
#include <math.h>
#include <string>
#include<map>
#include "drawstreet.h"


class Junction {
    public:
        Junction();
        Junction(std::map<int, node> *nodes, std::map<int, std::map<int, edge_axis>> *edges, std::vector<std::vector<int>> *objects);
        std::vector<float> findIntersection(edge_offset e1, edge_offset e2);
        void assignPair(edge_offset *offset, float t, point p, int index);
        void addPair(int start, int mid, int end);
        void findClosestIntersections();
        void findJunctionObjects();
        void addPairs();
        void drawJunctions();
        std::vector<std::vector<point>> all_dots = {};


    private:
        void addClosestIntersection(float closest_t, edge_offset *offset);
        void addJunctionLine(int start, int end, std::vector<int> target_id);
        std::vector<float> algorithmIntersection(point e1_start, point e1_end, point e2_start, point e2_end);
        std::vector<float> findCurvedIntersection(std::vector<point> e1, std::vector<point> e2);
        std::map<int, std::vector<curved_edge_axis>> junction_objects;
        std::map<int, node> *nodes;
        std::map<int, std::map<int, edge_axis>> *edges;
        std::vector<std::vector<int>> *objects;
};

#endif // JUNCTION_H
