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

struct point {
    float x;
    float y;
    float z;
};

struct osm_helper {
    int read_count;
    int stop_limit;
};

struct custom_node {
    long long id;
    double lat;
    double lon;
	point pixel;
};

struct custom_way {
    long long id;
    std::vector<long long> dots;
	std::vector<point> control = {};
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
    static std::vector<float> findCoordinates(double lat, double lon);
    static double largest_lat;
    static double largest_lon;
    static double smallest_lat;
    static double smallest_lon;
	static double largest_x;
    static double largest_z;
    static double smallest_x;
    static double smallest_z;
    static std::map<int, int> nodes_count;
    static std::map<int, std::vector<int>> nodes_links;
	static std::vector<point> approximateCurve(point start, point end, std::vector<point> control);
    static bool checkCurved(point a, point b, point c);
    static std::vector<point> findCurve(long long way_id);

};



#endif // PARSEINPUT_H
