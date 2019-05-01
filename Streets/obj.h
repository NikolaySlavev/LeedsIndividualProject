#ifndef OBJ_H
#define OBJ_H

#include <Windows.h>
#include <vector>
#include <fstream>
#include "layout.h"
//#include "structures.h"
//#include "junction.h"
//#include "buildings.h"


class OBJ {
public:
    OBJ(Layout *layout);
    int count_obj = 1;
    int stopped_count_obj = 1;
    std::fstream objfile;
    void writeObj();
private:
    void writeBuildings();
    void writeStraightStreet(edge_axis street, float t_start, float t_end);
    void writeCurvedStreet(edge_axis street, std::vector<point> p_start, std::vector<point> p_end, int i_start, int i_end);
    void writeJunctions();
    void writeStreets();
    Layout *layout;
};

#endif // OBJ_H
