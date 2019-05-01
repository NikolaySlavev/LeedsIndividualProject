#include "buildingblocks.h"
#include <iostream>

using namespace std;

BuildingBlocks::BuildingBlocks(map<int, node> *nodes, map<int, map<int, edge_axis>> *edges, vector<vector<int>> *objects, vector<vector<point>> *objects_p) {
    this->nodes = nodes;
    this->edges = edges;
    this->objects = objects;
    this->objects_p = objects_p;
}

void BuildingBlocks::removeLargest() {
    // removes the loop that goes around the boundary of the graph
    int largest = 0;
    int index = 0;
    for (int i = 0; i < objects->size(); i++) {
        if ((*objects)[i].size() > largest) {
            largest = (*objects)[i].size();
            index = i;
        }
    }

    objects->erase(objects->begin() + index);
    // at least three unique
    int count = 0;
    int node_count = 0;
    vector<int> obj;
    for (int i=0; i<objects->size(); i++) {
        obj = (*objects)[i];
        sort(obj.begin(), obj.end());
        for (int j=0; j<obj.size()-1; j++) {

            if (j+1 == obj.size()-1) {
                if (obj[j] != obj[j+1])
                    node_count++;
            }

            if (obj[j] == obj[j+1])
                count++;
            if (obj[j] != obj[j+1]) {
                if (count == 0)
                    node_count++;
                count = 0;
            }
        }
        if (node_count < 3) {
            objects->erase(objects->begin() + i);
            i--;
        }
        node_count = 0;
    }
}

void BuildingBlocks::computeDrawableBlocks() {
    // computes the blocks that will not overlap with any other segments
    vector<point> dots;
    vector<point> output;
    vector<point> edge_nodes;
    vector<vector<point>> all_dots;
    int s, m, e;
    for (vector<int> object : *objects) {
        for (int i=0; i < object.size(); i++) {
            if (i == object.size() - 1) {
                s = object[i];
                e = object[0];
            } else {
                s = object[i];
                e = object[i+1];
            }
            point start = (*edges)[s][e].offset_up.closest_p_intersection;
            if (isnan((*edges)[s][e].offset_up.closest_t_intersection))
                start = (*edges)[s][e].offset_up.end;
            point control = (*edges)[s][e].offset_up.pair_p_intersection;
            vector<int> p = (*edges)[s][e].offset_up.pair_id;
            point end = (*edges)[p[1]][p[0]].offset_down.closest_p_intersection;
            if (isnan((*edges)[p[1]][p[0]].offset_down.closest_t_intersection))
                end = (*edges)[p[1]][p[0]].offset_down.end;

            edge_nodes = (*edges)[s][e].offset_up.dots;

            if (!edge_nodes.empty()) {
                int end_dot = (*edges)[s][e].offset_up.closest_i_intersection;
                if (isnan((*edges)[s][e].offset_up.closest_t_intersection))
                    end_dot = edge_nodes.size()-1;
                int start_dot = (edge_nodes.size()-2) - (*edges)[e][s].offset_down.closest_i_intersection;
                if (isnan((*edges)[e][s].offset_down.closest_t_intersection))
                    start_dot = 0;
                if (start_dot < end_dot) {
                    dots.insert( dots.end(), edge_nodes.begin()+start_dot+1, edge_nodes.begin()+end_dot+1);
                }
            }

            if (isnan(control.x)) {
                output = {start, end};
            } else {
                curved_edge_axis line = {toNode(start), {control}, toNode(end)};
                output = street->drawQuadCurvedLine(toEdgeAxis(line));
            }
            dots.insert( dots.end(), output.begin(), output.end() );
        }

        bool correct_order = true;
        point p, q, r, temp;
        int val;
        for (int i=0; i<dots.size()-2; i++) {
           if (isnan(dots[i].x) || isnan(dots[i].z)) {
               correct_order = false;
           }
           p = dots[i];
           q = dots[i+1];
           r = dots[i+2];
           val = (q.z - p.z) * (r.x - q.x) - (q.x - p.x) * (r.z - q.z);
        }

        if (correct_order)
            objects_p->push_back(dots);
        dots = {};
    }
}

void BuildingBlocks::drawBlocks() {
    // draws the blocks which can be concave
    GLUtesselator *tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    for (vector<point> dots: *objects_p) {
        int vec_size = dots.size();
        GLdouble (*quad)[3] = new GLdouble[vec_size][3];
        float red = (rand() % 100 + 50) / 256.0;
        float green = (rand() % 100 + 50) / 256.0;
        float blue = (rand() % 100 + 50) / 256.0;
        glColor3f(red,green,blue);
        //glColor3f(1,0,0);

        gluTessBeginPolygon(tess, nullptr);
             gluTessBeginContour(tess);
                for (int i=0; i<dots.size(); i++) {
                    quad[i][0] = (double) dots[i].x;
                    quad[i][1] = 0;
                    quad[i][2] = (double) dots[i].z;
                    gluTessVertex(tess, quad[i], quad[i]);
                 }
             gluTessEndContour(tess);
        gluTessEndPolygon(tess);
    }
}

float BuildingBlocks::findAngle(graphVector vec1, graphVector vec2) {
    // finds the angle between two vectors
    float dot_product = vec1.x*vec2.x + vec1.z*vec2.z;
    float determinant = vec1.x*vec2.z - vec1.z*vec2.x;
    float angle = atan2(determinant, dot_product);

    angle = angle*180/3.145;
    if ((int) angle == 180 || (int) angle == 179)
        angle = -180;
    angle = 180 - angle;
    return angle;
}

void BuildingBlocks::findBlocks() {
    // the initial step of finding enclosed loops
    int v = 0;
    map<int, map<int, bool>> visited_edges;

    bool set_first = false;
    int search_from;
    for (int i = 0; i < (*edges).size(); i++) {
        for (auto const& edge : (*edges)[i]) {
            visited_edges[edge.second.start.id][edge.second.end.id] = false;
        }
    }

    float smallest_x = numeric_limits<float>::infinity(), smallest_z = numeric_limits<float>::infinity();
    int smallest_id = 0;
    for (int i=0; i < (*nodes).size(); i++) {
        if ((*nodes)[i].x < smallest_x) {
            smallest_x = (*nodes)[i].x;
            if ((*nodes)[i].z < smallest_z) {
                smallest_z = (*nodes)[i].z;
                smallest_id = (*nodes)[i].id;
            }
        }
    }
    graphVector vec, adj_vec;
    float angle, smallest_angle = 361;
    for (auto const& edge : (*edges)[smallest_id]) {
        adj_vec = street->findVector(toPoint((*edges)[smallest_id][edge.first].end), toPoint((*edges)[smallest_id][edge.first].start));
        angle = findAngle(vec, adj_vec);
        if (angle < smallest_angle) {
            smallest_angle = angle;
            search_from = edge.first;
        }
    }
    visited_edges[smallest_id][search_from] = true;
    stack = {smallest_id};
    vector<int> found = {-1, smallest_id};
    search(search_from, visited_edges, found);
}

void BuildingBlocks::search(int v, map<int, map<int, bool>> visited_edges, vector<int> found) {
    // the recursive algorithm that finds enclosed loops
    if (stack_count >= 1200) {
        return;
    }
    int check;
    graphVector vec, adj_vec;
    float angle, smallest_angle;
    int smallest = -1;
    vec = street->findVector(toPoint((*nodes)[v]), toPoint((*nodes)[stack[stack.size()-1]]));
    smallest_angle = 361;
    for (auto const& edge : (*edges)[v]) {
        if (!visited_edges[v][edge.first]) {
            adj_vec = street->findVector(toPoint((*edges)[v][edge.first].end), toPoint((*edges)[v][edge.first].start));
            angle = findAngle(vec, adj_vec);
            if (angle < smallest_angle) {
                smallest_angle = angle;
                smallest = edge.first;
            }
        }
    }

    if (smallest >= 0) {
        check = (*edges)[v][smallest].end.id;
        visited_edges[v][smallest] = true;

        stack.push_back(v);
        if (check == found[1]) {
            object_nodes = {};
            for (int j=stack.size()-1; j>=0; j--) {
                object_nodes.push_back(stack[j]);
                if (stack[j] == found[1]) {
                    break;
                }
            }
            objects->push_back(object_nodes);
        }
        stack_count++;
        search(check, visited_edges, found);
    } else {
        int previous = -1;
        if (stack.size() == 0)
            return;
        if (stack.size() == 1) {
            for (auto const& edge : (*edges)[stack[0]]) {
                if (!visited_edges[stack[0]][edge.first]) {
                    previous = (*edges)[stack[0]][edge.first].end.id;
                }
            }
            if (previous == -1)
                return;
        } else {
            previous = stack[stack.size()-1];
        }
        found[1] = previous;
        stack.pop_back();
        stack_count++;
        search(previous, visited_edges, found);
    }
}
