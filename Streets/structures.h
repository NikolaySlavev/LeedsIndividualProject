#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <Windows.h>
#include <vector>
#include <limits>
#include <array>
#include <GL/glu.h>

struct node {
    int id;
    float x;
    float y;
    float z;
};

struct point {
    float x;
    float y;
    float z;
};

struct graphVector {
    float x;
    float y;
    float z;
};

struct edge_offset {
    std::vector<int> id = {0,0,0};
    point start;
    point end;
    std::vector<int> pair_id = {0,0,0};
    float pair_t_intersection = std::numeric_limits<double>::quiet_NaN();
    point pair_p_intersection;
    int pair_t_index = 0;
    point closest_p_intersection;
    float closest_t_intersection = std::numeric_limits<double>::quiet_NaN();
    int closest_i_intersection = 0;
    std::vector<point> dots;
};

struct edge_axis {
    node start;
    node end;
    edge_offset offset_up;
    edge_offset offset_down;
    float step_size;
    std::vector<point> control;
};

struct curved_edge_axis {
    node start;
    std::vector<point> control;
    node end;
};

struct rectangle {
    graphVector U[2];           // unit vectors in both directions
    point origin;               // origin point of the vectors
    std::vector<int> index;
    float leftmost, rightmost, upmost;
    std::array<point, 4> nodes;   // nodes of the rectangle
    float area;
};

point toPoint(node input);
node toNode(point input);
edge_axis toEdgeAxis(curved_edge_axis input);
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid *data);
void CALLBACK tessVertexCB2(const GLvoid *data);

#endif // STRUCTURES_H
