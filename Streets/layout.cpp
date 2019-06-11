#include "layout.h"
#include <iostream>
#include <sstream>
#include <set>
#include <fstream>

using namespace std;

Layout::Layout() {
//    // random layout
//    gridCurved(-40, 3, 10); // creates grid layout
//    node node1 = {count_node_id++, -20, 0, -10}, node2 = {count_node_id++, 0, 0, -10}, node3 = {count_node_id++, 0, 0, 10};
//    vector<vector<node>> inputs = {{node1, node2},
//                                   {node2, node3},
//                                   {node1, node3},
//                                   {node1, nodes[8]},
//                                   {node1, nodes[5]},
//                                   {node2, nodes[6]}
//                                  };

//    inputLayout(inputs);

    // load the specified file as a layout
    fileLayout("example_borovets.csv");
    street = new DrawStreet();
    junction = new Junction(&nodes, &edges, &objects);
    blocks = new BuildingBlocks(&nodes, &edges, &objects, &objects_p);
    buildings = new Buildings(&objects_p);
    subdivision = new BlockSubdivision();
    blocks->findBlocks(); // finds graph loops
    junction->addPairs(); // adds the found counter-clockwise neighbours
    junction->findClosestIntersections(); //find the closest intersectinos between street offest lines
    junction->findJunctionObjects(); //find all junction objects
    blocks->removeLargest(); // removes the outer loop around the graph
    blocks->computeDrawableBlocks(); //finds the building areas considering all overlapping segments
    objects_p = subdivision->allSubdivision(objects_p); // subdivides the building areas
    buildings->computeBuildings(); // computes building structures
}

void Layout::updateWidthSize(float width) {
    // updates the width of the streets
    street_width = width;
    for (auto const& edge: edges) {
        for (auto const& e: edge.second) {
            if (!edges[edge.first][e.first].control.empty())
                edges[edge.first][e.first] = street->computeCurvedStreet(nodes[edge.first], nodes[e.first], edges[edge.first][e.first].control, width);
            else
                edges[edge.first][e.first] = street->computeStraightStreet(nodes[edge.first], nodes[e.first], width);
        }
    }
    updateScreen();
}

void Layout::updateMove(int move_node) {
    // updates a nodes location
    edge_axis changed_edge1, changed_edge2;
    for (auto const& edge: edges[move_node]) {
        if (!edge.second.control.empty()) {
            changed_edge1 = street->computeCurvedStreet(nodes[move_node], nodes[edge.first], edges[move_node][edge.first].control, street_width);
            changed_edge2 = street->computeCurvedStreet(nodes[edge.first], nodes[move_node], edges[edge.first][move_node].control, street_width);
        } else {
            changed_edge1 = street->computeStraightStreet(nodes[move_node], nodes[edge.first], street_width);
            changed_edge2 = street->computeStraightStreet(nodes[edge.first], nodes[move_node], street_width);
        }
        edges[move_node][edge.first] = changed_edge1;
        edges[edge.first][move_node] = changed_edge2;
    }
    updateScreen();
}

void Layout::updateScreen() {
    // updates the model when width is changed or a node is moved
    street = new DrawStreet();
    junction = new Junction(&nodes, &edges, &objects);
    blocks = new BuildingBlocks(&nodes, &edges, &objects, &objects_p);
    buildings = new Buildings(&objects_p);
    subdivision = new BlockSubdivision();

    objects = {};
    objects_p = {};
    blocks->findBlocks();
    junction->addPairs();
    junction->findClosestIntersections();
    junction->findJunctionObjects();
    blocks->removeLargest();
    blocks->computeDrawableBlocks();
    objects_p = subdivision->allSubdivision(objects_p);
    buildings->computeBuildings();
}

void Layout::inputLayout(vector<vector<node>> nodePairs) {
    // allows manual inputting
    bool exist = false;
    for (int i = 0; i < nodePairs.size(); i++) {
        for (int j=0; j < nodePairs[i].size(); j++) {
            for (auto const& n : nodes) {
                if (n.second.id == nodePairs[i][j].id)
                    exist = true;
            }
            if (!exist)
                addNode(nodePairs[i][j]);
            exist = false;
        }
    }
    edge_axis edge;
    for (int i = 0; i < nodePairs.size(); i++) {
        edge = street->computeStraightStreet(nodePairs[i][0], nodePairs[i][1]);
        addEdge(edge.start.id, edge.end.id, edge);
        edge = street->computeStraightStreet(nodePairs[i][1], nodePairs[i][0]);
        addEdge(edge.start.id, edge.end.id, edge);
    }
}

void Layout::fileLayout(string filename) {
    // opens a file to be used to model the street layout
    ifstream myfile("D:/Uni/Individual Project/SourceCode/LeedsIndividualProject/example_kocherinovo.csv");
    if (!myfile.is_open()) {
        cerr << "FILE CANNOT BE OPENED" << endl;
        return;
    }
    node n;
    string line, substr;
    vector<string> output;
    bool next = false;
    edge_axis edge;
    point c1, c2;
    while (myfile.good()) {
        output = {};
        getline(myfile, line, '\n');
        if (line == "") {
            next = true;
            continue;
        }
        stringstream s(line);
        while(s.good()) {
            getline(s, substr, ',');
            output.push_back(substr);
        }
        if (!next) {
            addNode({stoi(output[0]), stof(output[1]), stof(output[2]), stof(output[3])});
        } else {
            if (output.size() == 8) {
                c1 = {stof(output[2]), stof(output[3]), stof(output[4])};
                c2 = {stof(output[5]), stof(output[6]), stof(output[7])};
                edge = street->computeCurvedStreet(nodes[stoi(output[0])], nodes[stoi(output[1])], {c1, c2});
                addEdge(edge.start.id, edge.end.id, edge);
                edge = street->computeCurvedStreet(nodes[stoi(output[1])], nodes[stoi(output[0])], {c2, c1});
                addEdge(edge.start.id, edge.end.id, edge);
            } else {
                edge = street->computeStraightStreet(nodes[stoi(output[0])], nodes[stoi(output[1])]);
                addEdge(edge.start.id, edge.end.id, edge);
                edge = street->computeStraightStreet(nodes[stoi(output[1])], nodes[stoi(output[0])]);
                addEdge(edge.start.id, edge.end.id, edge);
            }
        }
    }
    myfile.close();
}


void Layout::gridCurved(int start, int size, int increment) {
    // creates a curved grid of streets
    int start_count = count_node_id;
    for (float i = start; i < (size*increment + start); i += increment) {
        for (float j = start; j < (size*increment + start); j += increment) {
            node new_node = {count_node_id, i, 0, j};
            addNode(new_node);
            count_node_id++;
        }
    }

    graphVector line_vector;
    float line_length;
    vector<graphVector> normals;
    edge_axis edge;
    for (int i = 0; i < nodes.size(); i++) {
        if ((i+1) % size != 0) {
            node start = nodes[i];
            node end = nodes[i+1];
            line_vector = street->findVector(toPoint(start), toPoint(end));
            line_length = street->findLength(line_vector);
            normals = street->findNormals(line_vector, line_length);
            point first = {(0.7*start.x + 0.3*end.x) + 3*normals[0].x, 0, (0.7*start.z + 0.3*end.z) + 5*normals[0].z};
            point second = {(0.3*start.x + 0.7*end.x) + 3*normals[0].x, 0, (0.3*start.z + 0.7*end.z) + 5*normals[0].z};

            edge = street->computeCurvedStreet(start, end, {first, second});
            addEdge(edge.start.id, edge.end.id, edge);
            edge = street->computeCurvedStreet(end, start, {second, first});
            addEdge(edge.start.id, edge.end.id, edge);
        }
        if (static_cast<int>((i+size) / size) != size) {
            edge = street->computeStraightStreet(nodes[i], nodes[i+size]);
            addEdge(edge.start.id, edge.end.id, edge);
            edge = street->computeStraightStreet(nodes[i+size], nodes[i]);
            addEdge(edge.start.id, edge.end.id, edge);
        }
    } 
}

void Layout::gridStraight(int start, int size, int increment) {
    // creates a straight grid of streets
    int start_count = count_node_id;

    for (float i = start; i < (size*(increment+6) + start); i += (increment+6)) {
        for (float j = start; j < (size*increment + start); j += increment) {
            node new_node = {count_node_id, i, 0, j};
            addNode(new_node);
            count_node_id++;
        }
    }

    vector<vector<float>> offset;
    edge_axis edge;
    for (int i = start_count; i < nodes.size(); i++) {
        if ((i+1) % size != 0) {
            edge = street->computeStraightStreet(nodes[i], nodes[i+1]);
            addEdge(edge.start.id, edge.end.id, edge);
            edge = street->computeStraightStreet(nodes[i+1], nodes[i]);
            addEdge(edge.start.id, edge.end.id, edge);
        }
        if (static_cast<int>((i+size) / size) != size) {
            edge = street->computeStraightStreet(nodes[i], nodes[i+size]);
            addEdge(edge.start.id, edge.end.id, edge);
            edge = street->computeStraightStreet(nodes[i+size], nodes[i]);
            addEdge(edge.start.id, edge.end.id, edge);
        }
    }
}

void Layout::addEdge(int node_s, int node_e, edge_axis edge) {
    // adds an edge to the graph
    edges[node_s][node_e] = edge;
}

void Layout::addNode(node new_node) {
    // adds a node to the graph
    nodes[new_node.id] = new_node;
}
