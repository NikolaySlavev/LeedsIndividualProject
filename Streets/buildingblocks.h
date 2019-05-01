#ifndef BUILDINGBLOCKS_H
#define BUILDINGBLOCKS_H

#include <Windows.h>
#include "structures.h"
#include <vector>
#include <GL/glu.h>
#include "structures.h"
#include "drawstreet.h"
#include <algorithm>
#include "junction.h"


class BuildingBlocks {
    public:
        BuildingBlocks(std::map<int, node> *nodes, std::map<int, std::map<int, edge_axis>> *edges, std::vector<std::vector<int>> *objects, std::vector<std::vector<point>> *objects_p);
        void findBlocks();
        float findAngle(graphVector vec1, graphVector vec2);
        void drawBlocks();
        void addPair(edge_offset edge, int edge_id, float t);
        void removeLargest();
        void computeDrawableBlocks();
    private:
        void search(int v, std::map<int, std::map<int, bool>> visited_edges, std::vector<int> found);
        std::vector<int> stack;
        //Layout *layout;
        DrawStreet *street = new DrawStreet();
        //Junction *junction;
        std::vector<std::vector<bool>> visited_edges;
        std::vector<int> object_nodes = {};
        std::map<int, node> *nodes;
        std::map<int, std::map<int, edge_axis>> *edges;
        std::vector<std::vector<int>> *objects;
        std::vector<std::vector<point>> *objects_p;
        int stack_count = 0;
};



#endif // BUILDINGBLOCKS_H
