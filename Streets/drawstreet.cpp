#include "drawstreet.h"
#include <algorithm>
#include <iostream>

using namespace std;

DrawStreet::DrawStreet() {}

vector<point> DrawStreet::drawQuadCurvedLine(edge_axis street) {
    point dot;
    float t = 0;

    vector<point> dots;

    glBegin(GL_POINTS);
    glPointSize(1);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        dot.x = pow(1-t, 2)*street.start.x + 2*(1-t)*t*street.control[0].x + t*t*street.end.x;
        dot.y = 0.5;
        dot.z = pow(1-t, 2)*street.start.z + 2*(1-t)*t*street.control[0].z + t*t*street.end.z;
        dots.push_back(dot);
        //t += street.step_size;
        t += 0.01;
        glVertex3f(dot.x, dot.y, dot.z);
    }
    glEnd();

    return dots;
}

void DrawStreet::drawStraightLine(edge_axis street) {
    float t = 0;
    point dot;

    glBegin(GL_POINTS);
    glPointSize(1);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        dot.x = (1-t)*street.start.x + t*street.end.x;
        dot.y = 1;
        dot.z = (1-t)*street.start.z + t*street.end.z;
        //t += street.step_size;
        t += 0.01;
        glVertex3f(dot.x, dot.y, dot.z);
    }
    glEnd();
}

void DrawStreet::drawCurvedStreet(edge_axis street) {
    float t = 0, line_length;
    point a, b, c, d, e, f, up_f, down_f;
    graphVector line_vector;
    vector<graphVector> normals;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        a = Lerp(toPoint(street.start), street.control[0], t);
        b = Lerp(street.control[0], street.control[1], t);
        c = Lerp(street.control[1], toPoint(street.end), t);
        d = Lerp(a,b,t);
        e = Lerp(b,c,t);
        f = Lerp(d,e,t);

        line_vector = findVector(e, d);
        line_length = findLength(line_vector);
        normals = findNormals(line_vector, line_length);

        up_f = {f.x + normals[0].x, f.y + normals[0].y, f.z + normals[0].z};
        down_f = {f.x + normals[1].x, f.y + normals[1].y, f.z + normals[1].z};;

        t += 0.01;
        glVertex3f(up_f.x, up_f.y, up_f.z);
        glVertex3f(down_f.x, down_f.y, down_f.z);
    }
    glEnd();
}


point DrawStreet::Lerp(point p1, point p2, float t) {
    point move = {(1-t)*p1.x + t*p2.x, (1-t)*p1.y + t*p2.y, (1-t)*p1.z + t*p2.z};
    return move;
}

edge_axis DrawStreet::computeStraightStreet(node start, node end) {
    edge_axis street;
    graphVector line_vector = findVector(toPoint(start), toPoint(end));
    float line_length =  findLength(line_vector);
    vector<graphVector> normals = findNormals(line_vector, line_length);
    street.step_size = 1 / (line_length * 10);

    street.start = start;
    street.end = end;
    edge_offset offset_up, offset_down;
    offset_up.id = {start.id, end.id, 0};
    offset_down.id = {start.id, end.id, 1};
    offset_up.start = {start.x + normals[0].x, start.y + normals[0].y, start.z + normals[0].z};
    offset_up.end = {end.x + normals[0].x, end.y + normals[0].y, end.z + normals[0].z};
    offset_down.start = {start.x + normals[1].x, start.y + normals[1].y, start.z + normals[1].z};
    offset_down.end = {end.x + normals[1].x, end.y + normals[1].y, end.z + normals[1].z};
    offset_up.pair_p_intersection = {NAN,NAN,NAN};
    street.offset_up = offset_up;
    street.offset_down = offset_down;

    return street;
}

void DrawStreet::drawStraightStreet(edge_axis street) {
    float t = 0;
    edge_offset offset_up = street.offset_up;
    edge_offset offset_down = street.offset_down;
    point up, down;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        up = Lerp(offset_up.start, offset_up.end, t);
        down = Lerp(offset_down.start, offset_down.end, t);
        t += street.step_size;
        glVertex3f(up.x, up.y, up.z);
        glVertex3f(down.x, down.y, down.z);
    }
    glEnd();
}

graphVector DrawStreet::findVector(point s, point e) { graphVector v; v = {s.x - e.x, s.y - e.y, s.z - e.z}; return v;}

float DrawStreet::findLength(graphVector line_vector) { return sqrt(pow(line_vector.x,2) + pow(line_vector.y,2) + pow(line_vector.z,2)); }

vector<graphVector> DrawStreet::findNormals(graphVector line_vector, float line_length) {
    vector<graphVector> normals;
    normals.push_back({-line_vector.z/line_length, line_vector.y/line_length, line_vector.x/line_length});
    normals.push_back({line_vector.z/line_length, line_vector.y/line_length, -line_vector.x/line_length});
    return normals;
}

