#include "buildingblocks.h"
#include <iostream>

using namespace std;

BuildingBlocks::BuildingBlocks(Layout *layout, DrawStreet *street, Junction *junction) {
    this->layout = layout;
    this->street = street;
    this->junction = junction;
}

void BuildingBlocks::removeLargest() {
    int largest = 0;
    int index = 0;
    for (int i = 0; i < layout->objects.size(); i++) {
        if (layout->objects[i].size() > largest) {
            largest = layout->objects[i].size();
            index = i;
        }
    }
    layout->objects.erase(layout->objects.begin() + index);
}

void BuildingBlocks::computeDrawableBlocks() {
    vector<point> dots;
    vector<point> output;
    vector<point> edge_nodes;
    vector<vector<point>> all_dots;
    int s, m, e;
    for (vector<int> object : layout->objects) {
        for (int i=0; i < object.size(); i++) {
            if (i == object.size() - 1) {
                s = object[i];
                e = object[0];
            } else {
                s = object[i];
                e = object[i+1];
            }
            point start = layout->edges[s][e].offset_up.closest_p_intersection;
            point control = layout->edges[s][e].offset_up.pair_p_intersection;
            vector<int> p = layout->edges[s][e].offset_up.pair_id;
            point end = layout->edges[p[1]][p[0]].offset_down.closest_p_intersection;

            edge_nodes = layout->edges[s][e].offset_up.dots;

            if (!edge_nodes.empty()) {
                int end_dot = layout->edges[s][e].offset_up.closest_i_intersection;
                int start_dot = (edge_nodes.size()-2) - layout->edges[e][s].offset_down.closest_i_intersection;
                dots.insert( dots.end(), edge_nodes.begin()+start_dot+1, edge_nodes.begin()+end_dot+1);
            }

            if (isnan(control.x))
                output = {start, end};
            else {
                curved_edge_axis line = {toNode(start), {control}, toNode(end)};
                output = street->drawQuadCurvedLine(toEdgeAxis(line));
            }
            dots.insert( dots.end(), output.begin(), output.end() );
        }
        layout->objects_p.push_back(dots);
        dots = {};
    }
}

void BuildingBlocks::drawBlocks() {
    GLUtesselator *tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    for (vector<point> dots: layout->objects_p) {

//        for (int i=0; i<dots.size(); i++) {
//            glPointSize(4);
//            glBegin(GL_POINTS);
//                 glVertex3f(dots[i].x, 3, dots[i].z);
//            glEnd();
//        }

        int vec_size = dots.size();
        GLdouble (*quad)[3] = new GLdouble[vec_size][3];
        float red = (rand() % 100 + 50) / 256.0;
        float green = (rand() % 100 + 50) / 256.0;
        float blue = (rand() % 100 + 50) / 256.0;
        glColor3f(red,green,blue);
        //glColor3f(1,0,0);
        glNormal3f(0,1,0);
        gluTessBeginPolygon(tess, nullptr);
             gluTessBeginContour(tess);
                for (int i=0; i<dots.size(); i++) {
                    quad[i][0] = (double) dots[i].x;
                    quad[i][1] = 0.5;
                    quad[i][2] = (double) dots[i].z;
                    gluTessVertex(tess, quad[i], quad[i]);
                 }
             gluTessEndContour(tess);
        gluTessEndPolygon(tess);
    }
}

float BuildingBlocks::findAngle(graphVector vec1, graphVector vec2) {
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
    int v = 0;
    map<int, map<int, bool>> visited_edges;

    bool set_first = false;
    int search_from;
    for (int i = 0; i < layout->edges.size(); i++) {
        for (auto const& edge : layout->edges[i]) {
            visited_edges[edge.second.start.id][edge.second.end.id] = false;
            if (!set_first) {
                visited_edges[edge.second.start.id][edge.second.end.id] = true;
                set_first = true;
                search_from = edge.second.end.id;
            }
        }
    }

    stack = {0};
    vector<int> found = {-1, 0};

    search(search_from, visited_edges, found);
}

void BuildingBlocks::search(int v, map<int, map<int, bool>> visited_edges, vector<int> found) {
    int check;
    graphVector vec, adj_vec;
    float angle, smallest_angle;
    int smallest = -1;
    vec = street->findVector(toPoint(layout->nodes[v]), toPoint(layout->nodes[stack[stack.size()-1]]));
    smallest_angle = 361;
    for (auto const& edge : layout->edges[v]) {
        if (!visited_edges[v][edge.first]) {
            adj_vec = street->findVector(toPoint(layout->edges[v][edge.first].end), toPoint(layout->edges[v][edge.first].start));
            angle = findAngle(vec, adj_vec);
            if (angle < smallest_angle) {
                smallest_angle = angle;
                smallest = edge.first;
            }
        }
    }
    if (smallest >= 0) {
        check = layout->edges[v][smallest].end.id;
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
            for (int j=0; j < object_nodes.size(); j++)
                cout << " OBJECT_NODES " << object_nodes[j];
            layout->objects.push_back(object_nodes);
        }
        search(check, visited_edges, found);
    } else {
        int previous = -1;
        if (stack.size() == 0)
            return;
        if (stack.size() == 1) {
            for (auto const& edge : layout->edges[stack[0]]) {
                if (!visited_edges[stack[0]][edge.first]) {
                    previous = layout->edges[stack[0]][edge.first].end.id;
                }
            }
            if (previous == -1)
                return;
        } else {
            previous = stack[stack.size()-1];
        }
        found[1] = previous;
        stack.pop_back();
        search(previous, visited_edges, found);
    }
}
