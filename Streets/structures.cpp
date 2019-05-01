#include "structures.h"
#include <iostream>
using namespace std;

point toPoint(node input) { point output = {input.x, input.y, input.z}; return output; }

node toNode(point input) { node output = {0, input.x, input.y, input.z}; return output; }

edge_axis toEdgeAxis(curved_edge_axis input) {
    edge_axis output;
    output.start = input.start;
    output.end = input.end;
    output.control = input.control;
    return output;
}

void CALLBACK tessBeginCB(GLenum which) { glBegin(which); }

void CALLBACK tessEndCB() { glEnd(); }

void CALLBACK tessVertexCB(const GLvoid *data) {
    const GLdouble *ptr = (const GLdouble*)data;
    glVertex3dv(ptr);
 }

void CALLBACK tessVertexCB2(const GLvoid *data) {
    const GLdouble *ptr = (const GLdouble*)data;
    glColor3dv(ptr+3);
    glVertex3dv(ptr);
}

void CALLBACK tessErrorCB(GLenum errorCode) {
    const GLubyte *errorStr;
    errorStr = gluErrorString(errorCode);
    //cerr << "[ERROR]: " << errorStr << endl;
}
