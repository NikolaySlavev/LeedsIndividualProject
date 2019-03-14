#include "structures.h"

point toPoint(node input) { point output = {input.x, input.y, input.z}; return output; }

node toNode(point input) { node output = {0, input.x, input.y, input.z}; return output; }

edge_axis toEdgeAxis(curved_edge_axis input) {
    edge_axis output;
    output.start = input.start;
    output.end = input.end;
    output.control = input.control;
    return output;
}
