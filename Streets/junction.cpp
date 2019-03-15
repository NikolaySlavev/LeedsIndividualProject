#include "junction.h"
#include <iostream>

using namespace std;

Junction::Junction(Layout *layout, DrawStreet *street) {
    this->layout = layout;
    this->street = street;
}

void Junction::drawJunctions() {
    GLUtesselator *tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    vector<point> dots;
    vector<point> output;
    vector<vector<point>> all_dots;
    for (auto const& junction : junction_objects) {
        for (curved_edge_axis line: junction.second) {
            if (line.control.empty()) {
                street->drawStraightLine(toEdgeAxis(line));
                dots.push_back(toPoint(line.start));
                dots.push_back(toPoint(line.end));
            } else {
                output = street->drawQuadCurvedLine(toEdgeAxis(line));
                dots.insert( dots.end(), output.begin(), output.end() );
            }
        }
        all_dots.push_back(dots);
        dots = {};
    }

    for (vector<point> dots: all_dots) {
        int vec_size = dots.size();
        GLdouble (*quad3)[3] = new GLdouble[vec_size][3];
        glColor3f(1,1,1);
        gluTessBeginPolygon(tess, nullptr);
             gluTessBeginContour(tess);
                for (int i=0; i<dots.size(); i++) {
                    quad3[i][0] = dots[i].x;
                    quad3[i][1] = 0.5;//dots[i].y;
                    quad3[i][2] = dots[i].z;
                    gluTessVertex(tess, quad3[i], quad3[i]);
                 }
             gluTessEndContour(tess);
        gluTessEndPolygon(tess);
    }
}

void Junction::addJunctionLine(int start, int end, vector<int> target_id) {
    curved_edge_axis straight;
    straight.start = toNode(layout->edges[start][end].offset_down.closest_p_intersection);
    straight.end = toNode(layout->edges[start][end].offset_up.closest_p_intersection);
    junction_objects[end].push_back(straight);

    curved_edge_axis curve;
    curve.start = toNode(layout->edges[start][end].offset_up.closest_p_intersection);
    if (!isnan(layout->edges[start][end].offset_up.pair_t_intersection))
        curve.control.push_back(layout->edges[start][end].offset_up.pair_p_intersection);
    vector<int> pair_id = layout->edges[start][end].offset_up.pair_id;
    int end_pair_id = pair_id[0]; //(int) pair_id / 100;
    int start_pair_id =  pair_id[1]; //(int) (pair_id % 100 / 10);
    edge_axis new_edge = layout->edges[start_pair_id][end_pair_id];
    curve.end = toNode(new_edge.offset_down.closest_p_intersection);
    junction_objects[end].push_back(curve);

    if (start_pair_id == target_id[0] && end_pair_id == target_id[1])
        return;

    addJunctionLine(start_pair_id, end_pair_id, target_id);
}

void Junction::findJunctionObjects() {
    for (auto const& node : layout->nodes) {
        int start = layout->edges[node.first].begin()->first;
        int end = node.first;
        cout << "END: " << start << " " << end << endl;
        addJunctionLine(start, end, {start, end});
    }
}

void Junction::addClosestIntersection(float closest_t, edge_offset *offset) {
    point closest_p = {(1-closest_t)*offset->start.x + closest_t*offset->end.x,
                       (1-closest_t)*offset->start.y + closest_t*offset->end.y,
                       (1-closest_t)*offset->start.z + closest_t*offset->end.z};
    offset->closest_p_intersection = closest_p;
    offset->closest_t_intersection = closest_t;
}

void Junction::findClosestIntersections() {
    float closest_t;
    for (auto const& edges : layout->edges) {
        for (auto const& edge : edges.second) {
            edge_offset offset_up = edge.second.offset_up;
            edge_offset offset_down = edge.second.offset_down;
            if (isnan(offset_down.pair_t_intersection) && isnan(offset_up.pair_t_intersection))
                cout << "LOL" << endl;
            if (isnan(offset_down.pair_t_intersection))
                closest_t = offset_up.pair_t_intersection - 0.2f;
            else if (isnan(offset_up.pair_t_intersection))
                closest_t = offset_down.pair_t_intersection - 0.2f;
            else if (offset_down.pair_t_intersection < offset_up.pair_t_intersection)
                closest_t = offset_down.pair_t_intersection - 0.2f;
            else if (offset_up.pair_t_intersection <= offset_down.pair_t_intersection)
                closest_t = offset_up.pair_t_intersection - 0.2f;
            addClosestIntersection(closest_t, &layout->edges[edge.second.start.id][edge.second.end.id].offset_up);
            addClosestIntersection(closest_t, &layout->edges[edge.second.start.id][edge.second.end.id].offset_down);
        }
    }
}

void Junction::addPairs() {
    int s, m, e;
    for (vector<int> object: layout->objects) {
        for (int i=0; i < object.size(); i++) {
            m = object[i];
            if (i == object.size() - 1) {
                s = object[i-1];
                e = object[0];
            } else if(i == 0) {
                s = object[object.size()-1];
                e = object[i+1];
            } else {
                s = object[i-1];
                e = object[i+1];
            }
            addPair(s, m, e);
        }
    }
}

void Junction::addPair(int start, int mid,  int end) {
    edge_offset edge1 = layout->edges[start][mid].offset_up;
    edge_offset edge2 = layout->edges[mid][end].offset_up;

    vector<float> intersection = findIntersection(edge1, edge2);
    float t = intersection[0];
    float u = intersection[1];
    edge_offset *offset_up = &layout->edges[start][mid].offset_up;
    edge_offset *offset_down = &layout->edges[end][mid].offset_down;
    offset_up->pair_id = edge2.id;
    offset_down->pair_id = edge1.id;

    if (!isinf(t) && !isnan(t)) {
        point p = {(1 - t) * edge1.start.x + t * edge1.end.x, 0 , (1 - t) * edge1.start.z + t * edge1.end.z};
        assignPair(offset_up, t, p);
        assignPair(offset_down, 1-u, p);
    }
}

vector<float> Junction::findIntersection(edge_offset e1, edge_offset e2) {
    float t = ((e1.start.x - e2.start.x) * (e2.start.z - e2.end.z) - (e1.start.z - e2.start.z) * (e2.start.x - e2.end.x)) / ((e1.start.x - e1.end.x) * (e2.start.z - e2.end.z) - (e1.start.z - e1.end.z)*(e2.start.x - e2.end.x));
    float u = -(((e1.start.x - e1.end.x) * (e1.start.z - e2.start.z) - (e1.start.z - e1.end.z) * (e1.start.x - e2.start.x)) / ((e1.start.x - e1.end.x) * (e2.start.z - e2.end.z) - (e1.start.z - e1.end.z)*(e2.start.x - e2.end.x)));
    return {t, u};
}

void Junction::assignPair(edge_offset *offset, float t, point p) {
    offset->pair_t_intersection = t;
    offset->pair_p_intersection = p;
}
