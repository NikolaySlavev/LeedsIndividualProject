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
        bool search(int v, std::map<int, std::map<int, bool>> visited_edges, std::vector<int> found);
        std::vector<int> stack;
        DrawStreet *street = new DrawStreet();
        std::vector<std::vector<bool>> visited_edges;
        std::vector<int> object_nodes = {};
        std::map<int, node> *nodes;
        std::map<int, std::map<int, edge_axis>> *edges;
        std::vector<std::vector<int>> *objects;
        std::vector<std::vector<point>> *objects_p;
        int stack_count = 0;
        int left_search_from = 0;
        std::map<int, std::map<int, bool>> left_visited = {};
        std::vector<int> left_found = {};
};



#endif // BUILDINGBLOCKS_H
