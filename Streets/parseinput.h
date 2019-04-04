#ifndef PARSEINPUT_H
#define PARSEINPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "readosm.h"
#include <fstream>
#include <iostream>
#include <map>


struct osm_helper {
    int read_count;
    int stop_limit;
};

struct custom_node {
    long long id;
    double lat;
    double lon;
};

struct custom_way {
    long long id;
    std::vector<long long> dots;
};

class ParseInput {
public:
    ParseInput(char* file_name);
    static std::map<int, custom_node> nodes;
    static std::map<int, custom_way> ways;
private:
    static int id_count;
    static std::ofstream myfile;
    std::string file_name;
    static int parseNode (const void *user_data, const readosm_node * node);
    static int parseWay (const void *user_data, const readosm_way * way);
    static int parseRelation (const void *user_data, const readosm_relation * relation);
    static int evalAbort (struct osm_helper *helper);
    static void divideWay(int way_id, int node_id);
    static void populateFile();
    static std::vector<double> findCoordinates(double lat, double lon);
    static double largest_lat;
    static double largest_lon;
    static double smallest_lat;
    static double smallest_lon;
    static std::map<int, int> nodes_count;
    static std::map<int, std::vector<int>> nodes_links;

};



#endif // PARSEINPUT_H
