#include "obj.h"
#include <iostream>
using namespace std;

OBJ::OBJ(Layout *layout) {
    this->layout = layout;
}

void OBJ::writeObj() {
    objfile.open("D:/Uni/Individual Project/SourceCode/LeedsIndividualProject/build-Streets-QT_VSdebug-Debug/testobj.obj", fstream::out);
    writeStreets();
    writeJunctions();
    writeBuildings();
    objfile.close();
}

void OBJ::writeStreets() {
    for (auto const& edges : layout->edges) {
       for (auto const& edge : edges.second) {
           if (edge.second.end.id > edge.second.start.id) {
               int start = edge.second.start.id;
               int end = edge.second.end.id;
               edge_axis rev_edge = layout->edges[end][start];
               if (edge.second.control.empty()) {
                   float first_t = 0, second_t = 1;
                   if (!isnan(rev_edge.offset_down.closest_t_intersection))
                       first_t = 1 - rev_edge.offset_down.closest_t_intersection;
                   if (!isnan(edge.second.offset_down.closest_t_intersection))
                       second_t = edge.second.offset_down.closest_t_intersection;
                   writeStraightStreet(edge.second, first_t, second_t);
               } else {
                   int i_start = 0;
                   int i_end = edge.second.offset_down.dots.size()-1;
                   vector<point> p_end = {};
                   vector<point> p_start = {};
                   if (!isnan(edge.second.offset_up.closest_t_intersection)) {
                        i_end = edge.second.offset_up.closest_i_intersection;
                        p_end = {edge.second.offset_up.closest_p_intersection, edge.second.offset_down.closest_p_intersection};
                   }
                    if (!isnan(rev_edge.offset_up.closest_t_intersection)) {
                       i_start = (rev_edge.offset_up.dots.size() - 1) - rev_edge.offset_up.closest_i_intersection;
                       p_start = {rev_edge.offset_up.closest_p_intersection, rev_edge.offset_down.closest_p_intersection};
                    }
                   writeCurvedStreet(edge.second, p_start, p_end, i_start, i_end);
               }
           }
        }
    }
}

void OBJ::writeBuildings() {
    point a, b, c, d;
    for (vector<point> dots: layout->buildings->building_points) {
        for (int i=0; i<dots.size(); i++) {
            objfile << "v " + to_string(dots[i].x) + " " + to_string(dots[i].y) + " " + to_string(dots[i].z) + "\n";
            count_obj++;

        }
        objfile << "f";
        for (int i=stopped_count_obj; i<count_obj; i++) {
            objfile << " " + to_string(i);
        }
        objfile << "\n";
        stopped_count_obj = count_obj;

        for (int i=0; i<dots.size(); i++) {
            a = dots[i];
            if (i+1 == dots.size())
                b = dots[0];
            else
                b = dots[i+1];
            c = b;
            c.y = 0;
            d = a;
            d.y = 0;
            objfile << "v " + to_string(a.x) + " " + to_string(a.y) + " " + to_string(a.z) + "\n";
            objfile << "v " + to_string(b.x) + " " + to_string(b.y) + " " + to_string(b.z) + "\n";
            objfile << "v " + to_string(c.x) + " " + to_string(c.y) + " " + to_string(c.z) + "\n";
            objfile << "v " + to_string(d.x) + " " + to_string(d.y) + " " + to_string(d.z) + "\n";
            count_obj+=4;
            objfile << "f";
            for (int i=stopped_count_obj; i<count_obj; i++) {
              objfile << " " + to_string(i);
            }
            objfile << "\n";
            stopped_count_obj = count_obj;
        }
    }
}

void OBJ::writeStraightStreet(edge_axis street, float t_start, float t_end) {
    edge_offset offset_up = street.offset_up;
    edge_offset offset_down = street.offset_down;
    point obj_up_start = {(1-t_start)*offset_up.start.x+t_start*offset_up.end.x, 0, (1-t_start)*offset_up.start.z+t_start*offset_up.end.z};
    point obj_up_end = {(1-t_end)*offset_up.start.x+t_end*offset_up.end.x, 0, (1-t_end)*offset_up.start.z+t_end*offset_up.end.z};
    point obj_down_start = {(1-t_start)*offset_down.start.x+t_start*offset_down.end.x, 0, (1-t_start)*offset_down.start.z+t_start*offset_down.end.z};
    point obj_down_end = {(1-t_end)*offset_down.start.x+t_end*offset_down.end.x, 0, (1-t_end)*offset_down.start.z+t_end*offset_down.end.z};
    objfile << "v " + to_string(obj_up_start.x) + " " + to_string(obj_up_start.y) + " " + to_string(obj_up_start.z) + "\n";
    objfile << "v " + to_string(obj_up_end.x) + " " + to_string(obj_up_end.y) + " " + to_string(obj_up_end.z) + "\n";
    objfile << "v " + to_string(obj_down_end.x) + " " + to_string(obj_down_end.y) + " " + to_string(obj_down_end.z) + "\n";
    objfile << "v " + to_string(obj_down_start.x) + " " + to_string(obj_down_start.y) + " " + to_string(obj_down_start.z) + "\n";

    count_obj+=4;
    objfile << "f";
    for (int i=stopped_count_obj; i<count_obj; i++) {
        objfile << " " + to_string(i);
    }
    objfile << "\n";
    stopped_count_obj = count_obj;
}

void OBJ::writeCurvedStreet(edge_axis street, vector<point> p_start, vector<point> p_end, int i_start, int i_end) {
    point up_f, down_f;
    if (!p_start.empty()) {
        objfile << "v " + to_string(p_start[0].x) + " " + to_string(p_start[0].y) + " " + to_string(p_start[0].z) + "\n";
        count_obj++;
    }
    for (int i = i_start; i <= i_end; i++) {
        up_f = street.offset_up.dots[i];
        objfile << "v " + to_string(up_f.x) + " " + to_string(up_f.y) + " " + to_string(up_f.z) + "\n";
        count_obj++;
    }
    if (!p_end.empty()) {
        objfile << "v " + to_string(p_end[0].x) + " " + to_string(p_end[0].y) + " " + to_string(p_end[0].z) + "\n";
        count_obj++;
    }

    if (!p_end.empty()) {
        objfile << "v " + to_string(p_end[1].x) + " " + to_string(p_end[1].y) + " " + to_string(p_end[1].z) + "\n";
        count_obj++;
    }
    for (int i = i_end; i >= i_start; i--) {
        down_f = street.offset_down.dots[i];
        objfile << "v " + to_string(down_f.x) + " " + to_string(down_f.y) + " " + to_string(down_f.z) + "\n";
        count_obj++;
    }

    if (!p_start.empty()) {
        objfile << "v " + to_string(p_start[1].x) + " " + to_string(p_start[1].y) + " " + to_string(p_start[1].z) + "\n";
        count_obj++;
    }

    objfile << "f";
    for (int i=stopped_count_obj; i<count_obj; i++)
        objfile << " " + to_string(i);

    objfile << "\n";
    stopped_count_obj = count_obj;
}

void OBJ::writeJunctions() {
    for (vector<point> dots: layout->junction->all_dots) {
        for (int i=0; i<dots.size(); i++) {
            objfile << "v " + to_string(dots[i].x) + " " + to_string(dots[i].y) + " " + to_string(dots[i].z) + "\n";
            count_obj++;
        }
        objfile << "f";
        for (int i=stopped_count_obj; i<count_obj; i++) {
            objfile << " " + to_string(i);
        }
        objfile << "\n";
        stopped_count_obj = count_obj;
    }
}
