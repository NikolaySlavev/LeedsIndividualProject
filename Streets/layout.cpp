#include "layout.h"
#include <iostream>
#include <sstream>
#include <set>
#include <fstream>

using namespace std;

Layout::Layout(DrawStreet *street) {
    this->street = street;
}

void Layout::inputLayout(vector<vector<node>> nodePairs) {
    // very dodgy code!!! with node and edge ids
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
    cout << "!!!!!!!" << endl;
    ifstream myfile("D:/Uni/Individual Project/SourceCode/LeedsIndividualProject/example_old.csv");
    if (!myfile.is_open()) {
        cout << "FILE CANNOT BE OPENED" << endl;
        return;
    }
    node n;
    string line, substr;
    vector<string> output;
    bool next = false;
    edge_axis edge;
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
            cout << line << endl;
            addNode({stoi(output[0]), stof(output[1]), stof(output[2]), stof(output[3])});
        } else {
            cout << "OK2" << endl;
            edge = street->computeStraightStreet(nodes[stoi(output[0])], nodes[stoi(output[1])]);
            addEdge(edge.start.id, edge.end.id, edge);
            edge = street->computeStraightStreet(nodes[stoi(output[1])], nodes[stoi(output[0])]);
            addEdge(edge.start.id, edge.end.id, edge);
        }
    }
    myfile.close();
    cout << "DONE" << endl;
}

//void random() {}

//void gridRandom() {}

void Layout::gridCurved(int start, int size, int increment) {
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
    int start_count = count_node_id;
    for (float i = start; i < (size*increment + start); i += increment) {
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
    // FIXXXXXX for curves
    edges[node_s][node_e] = edge;
}

void Layout::addNode(node new_node) {
    nodes[new_node.id] = new_node;
}
