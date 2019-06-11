#include "junction.h"
#include <iostream>

using namespace std;

Junction::Junction() {}

Junction::Junction(map<int, node> *nodes, map<int, map<int, edge_axis>> *edges, vector<vector<int>> *objects) {
    this->nodes = nodes;
    this->edges = edges;
    this->objects = objects;
}

void Junction::drawJunctions() {
    // draw the ready jucntions
    DrawStreet *street = new DrawStreet();
    GLUtesselator *tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    vector<point> dots;
    vector<point> output;

    all_dots = {};
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
    GLdouble (*quad)[3] = nullptr;
    int vec_size = 0;
    for (vector<point> dots: all_dots) {
        vec_size = dots.size();
        quad = new GLdouble[vec_size][3];

        //glColor3f(1,1,1);
        gluTessBeginPolygon(tess, nullptr);
             gluTessBeginContour(tess);
                for (int i=0; i<dots.size(); i++) {
                    quad[i][0] = dots[i].x;
                    quad[i][1] = 0;//dots[i].y;
                    quad[i][2] = dots[i].z;
                    gluTessVertex(tess, quad[i], quad[i]);
                 }
             gluTessEndContour(tess);
        gluTessEndPolygon(tess);
        free(quad);
    }

}

void Junction::addJunctionLine(int start, int end, vector<int> target_id) {
    // connects the different turning radiuses into a single object
    curved_edge_axis straight;
    if (isnan((*edges)[start][end].offset_down.closest_t_intersection))
        return;

    straight.start = toNode((*edges)[start][end].offset_down.closest_p_intersection);
    straight.end = toNode((*edges)[start][end].offset_up.closest_p_intersection);
    junction_objects[end].push_back(straight);

    curved_edge_axis curve;
    curve.start = toNode((*edges)[start][end].offset_up.closest_p_intersection);
    if (!isnan((*edges)[start][end].offset_up.pair_t_intersection))
        curve.control.push_back((*edges)[start][end].offset_up.pair_p_intersection);
    if (isnan((*edges)[start][end].offset_up.pair_t_intersection))
        return;
    vector<int> pair_id = (*edges)[start][end].offset_up.pair_id;
    int end_pair_id = pair_id[0];
    int start_pair_id =  pair_id[1];
    edge_axis new_edge = (*edges)[start_pair_id][end_pair_id];
    curve.end = toNode(new_edge.offset_down.closest_p_intersection);
    junction_objects[end].push_back(curve);

    if (start_pair_id == target_id[0] && end_pair_id == target_id[1])
        return;

    addJunctionLine(start_pair_id, end_pair_id, target_id);
}

void Junction::findJunctionObjects() {
    // iterates over all nodes which will be used to model junctions
    for (auto const& node : *nodes) {
        int start = (*edges)[node.first].begin()->first;
        int end = node.first;
        addJunctionLine(start, end, {start, end});
    }
}

void Junction::addClosestIntersection(float closest_t, edge_offset *offset) {
    // assigns the closest intersectino to the relevant offset lines
    point start = offset->start;
    point end = offset->end;
    if (!offset->dots.empty()) {
        int closest_i = (int) closest_t;
        if (closest_i >= offset->dots.size())
            return;
        start = offset->dots[closest_i];
        end = offset->dots[closest_i+1];
        closest_t -= closest_i;
        offset->closest_i_intersection = closest_i;
    }

    point closest_p = {(1-closest_t)*start.x + closest_t*end.x,
                       (1-closest_t)*start.y + closest_t*end.y,
                       (1-closest_t)*start.z + closest_t*end.z};
    offset->closest_p_intersection = closest_p;
    offset->closest_t_intersection = closest_t;
}

void Junction::findClosestIntersections() {
    // finds the closest intersecection for each street line
    float closest_t = NAN;
    int count = 0;
    for (auto const& new_edges : (*edges)) {
        count++;
        for (auto const& edge : new_edges.second) {
            edge_offset offset_up = edge.second.offset_up;
            edge_offset offset_down = edge.second.offset_down;

            if (isnan(offset_down.pair_t_intersection) && isnan(offset_up.pair_t_intersection))
                continue;
            else if (isnan(offset_down.pair_t_intersection) ||
                    (!isnan(offset_up.pair_t_intersection) && offset_up.pair_t_index < offset_down.pair_t_index) ||
                    (!isnan(offset_up.pair_t_intersection) && offset_down.pair_t_index == offset_up.pair_t_index && offset_up.pair_t_intersection < offset_down.pair_t_intersection)) {
                closest_t = (offset_up.pair_t_index + offset_up.pair_t_intersection) * 0.9;
                if (offset_up.pair_t_intersection < 0) {
                    closest_t = (offset_up.dots.size() - offset_up.pair_t_intersection) * 0.1;
                }
            } else if (isnan(offset_up.pair_t_intersection) ||
                      (!isnan(offset_down.pair_t_intersection) && offset_down.pair_t_index < offset_up.pair_t_index) ||
                      (!isnan(offset_down.pair_t_intersection) && offset_down.pair_t_index == offset_up.pair_t_index && offset_down.pair_t_intersection <= offset_up.pair_t_intersection)) {
                closest_t = (offset_down.pair_t_index + offset_down.pair_t_intersection) * 0.9;
                if (offset_down.pair_t_intersection < 0) {
                    closest_t = (offset_up.dots.size() - offset_up.pair_t_intersection) * 0.1;
                }
            }
            addClosestIntersection(closest_t, &(*this->edges)[edge.second.start.id][edge.second.end.id].offset_up);
            addClosestIntersection(closest_t, &(*this->edges)[edge.second.start.id][edge.second.end.id].offset_down);
        }
    }
}

void Junction::addPairs() {
    // find the pairs
    int s, m, e;
    for (vector<int> object: *objects) {
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
    // adds pairs to the graph
    edge_offset edge1 = (*edges)[start][mid].offset_up;
    edge_offset edge2 = (*edges)[mid][end].offset_up;

    vector<float> intersection = findIntersection(edge1, edge2);
    float t = intersection[0];
    float u = intersection[1];
    int index_t = (int) intersection[2];
    int index_u = (int) intersection[3];
    if (edge2.dots.size() != 0)
        index_u = (edge2.dots.size()-2) - index_u;

    edge_offset *offset_up = &(*edges)[start][mid].offset_up;
    edge_offset *offset_down = &(*edges)[end][mid].offset_down;
    offset_up->pair_id = edge2.id;
    offset_down->pair_id = edge1.id;

    if (!isinf(t) && !isnan(t)) {
        point p;
        if (index_t == 0)
            p = {(1 - t) * edge1.start.x + t * edge1.end.x, 0 , (1 - t) * edge1.start.z + t * edge1.end.z};
        else
            p = {(1-t) * edge1.dots[index_t].x + t * edge1.dots[index_t+1].x, 0, (1-t) * edge1.dots[index_t].z + t * edge1.dots[index_t+1].z};
        assignPair(offset_up, t, p, index_t);
        assignPair(offset_down, 1-u, p, index_u);
    }
}

vector<float> Junction::findIntersection(edge_offset e1, edge_offset e2) {
    // finds intersecion between two edges
    vector<float> t_u;
    if (e1.dots.empty() && e2.dots.empty()) {
         t_u = algorithmIntersection(e1.start, e1.end, e2.start, e2.end);
         t_u.push_back(0);
         t_u.push_back(0);
    } else {
        vector<point> e1_dots = e1.dots;
        vector<point> e2_dots = e2.dots;
        if (e1.dots.empty())
            e1_dots = {e1.start, e1.end};
        if (e2.dots.empty())
            e2_dots = {e2.start, e2.end};
        t_u = findCurvedIntersection(e1_dots, e2_dots);
    }
    return t_u;
}

vector<float> Junction::findCurvedIntersection(vector<point> e1, vector<point> e2) {
    // finds a curved intersection
    vector<float> best = {numeric_limits<float>::infinity(), numeric_limits<float>::infinity()};
    vector<int> best_index = {-1, -1};
    vector<float> t_u;

    for (int i=0; i < e1.size()-1; i++) {
        for (int j=0; j < e2.size()-1; j++) {
            t_u = algorithmIntersection(e1[i], e1[i+1], e2[j], e2[j+1]);
            if (t_u[0] >= 0 && t_u[0] <= 1 && t_u[1] >=0 && t_u[1] <= 1) {
                best = {t_u[0], t_u[1]};
                best_index = {i,j};
                return {best[0], best[1], static_cast<float>(best_index[0]), static_cast<float>(best_index[1])};
            }
        }
    }

    t_u = algorithmIntersection(e1[e1.size()-2], e1[e1.size()-1], e2[0], e2[1]);
    best = {t_u[0], t_u[1]};
    best_index = {static_cast<int>(e1.size()-2), 0};
    return {best[0], best[1], static_cast<float>(best_index[0]), static_cast<float>(best_index[1])};
}

vector<float> Junction::algorithmIntersection(point e1_start, point e1_end, point e2_start, point e2_end) {
    // the actual algorithm that finds the intersection
    float t = ((e1_start.x - e2_start.x) * (e2_start.z - e2_end.z) - (e1_start.z - e2_start.z) * (e2_start.x - e2_end.x)) / ((e1_start.x - e1_end.x) * (e2_start.z - e2_end.z) - (e1_start.z - e1_end.z)*(e2_start.x - e2_end.x));
    float u = -(((e1_start.x - e1_end.x) * (e1_start.z - e2_start.z) - (e1_start.z - e1_end.z) * (e1_start.x - e2_start.x)) / ((e1_start.x - e1_end.x) * (e2_start.z - e2_end.z) - (e1_start.z - e1_end.z)*(e2_start.x - e2_end.x)));
    //inf
    if (t > 100 || u > 100) {
        t = NAN; u = NAN;
    }
    return {t, u};
}

void Junction::assignPair(edge_offset *offset, float t, point p, int index) {
    offset->pair_t_intersection = t;
    offset->pair_p_intersection = p;
    offset->pair_t_index = index;
}
