//#include "parseinput.h"
//#include <iostream>
//#include <fstream>
//#include <string.h>
//#include <iterator>
//#include <algorithm>
//#define _USE_MATH_DEFINES
//#include <math.h>


//using namespace std;

//ofstream ParseInput::myfile;
//ParseInput::ParseInput(char* file_name) {
//    myfile.open ("example.csv");
//    this->file_name = "map.osm";
//    this->file_name = file_name;
//    const void *osm_handle;
//    int ret;
//    struct osm_helper helper;
//    helper.read_count = 0;
//    helper.stop_limit = 100;

//    ret = readosm_open(file_name, &osm_handle);
//    if (ret != READOSM_OK) {
//        cout << "NOT OK1" << endl;
//        readosm_close(osm_handle);
//        return;
//    }
//    ret = readosm_parse(osm_handle, &helper, parseNode, parseWay, parseRelation);
//    if (ret != READOSM_OK) {
//        cout << "NOT OK2" << endl;
//        readosm_close (osm_handle);
//        return;
//    }
//    readosm_close (osm_handle);
//    populateFile();
//    myfile.close();
//}

//map<int, custom_node> ParseInput::nodes = {};
//int ParseInput::parseNode (const void *user_data, const readosm_node * node) {
//    struct osm_helper *helper = (struct osm_helper *) user_data;
//    nodes[node->id] = {node->id, node->latitude, node->longitude};
//    nodes_count[node->id] = 0;

//    if (node->latitude != READOSM_UNDEFINED) {
//        if (node->latitude > largest_lat)
//            largest_lat = node->latitude;
//        if (node->latitude < smallest_lat)
//             smallest_lat = node->latitude;
//    }
//    if (node->longitude != READOSM_UNDEFINED) {
//        if (node->longitude > largest_lon)
//            largest_lon = node->longitude;
//        if (node->longitude < smallest_lon)
//            smallest_lon = node->longitude;
//    }

//    if (evalAbort (helper))
//        return READOSM_ABORT;
//    return READOSM_OK;
//}

//map<int, custom_way> ParseInput::ways = {};
//double ParseInput::largest_lat = -181;
//double ParseInput::largest_lon = -181;
//double ParseInput::smallest_lat = 181;
//double ParseInput::smallest_lon = 181;
//map<int, int> ParseInput::nodes_count = {};
//map<int, vector<int>> ParseInput::nodes_links = {};
//int ParseInput::id_count = 0;

//int ParseInput::parseWay (const void *user_data, const readosm_way * way) {
//    struct osm_helper *helper = (struct osm_helper *) user_data;
//    custom_way c_way;
//    const readosm_tag* tag;
//    const long long* ref;
//    bool found = false;
//    int way_link_id;
//    for (int i=0; i < way->tag_count; i++) {
//        tag = way->tags + i;
//        if (strcmp(tag->key, "highway") == 0)
//            found = true;
//    }
//    if (found == true) {
//        c_way.id = way->id;
//        ways[c_way.id] = c_way;
//        for (int i=0; i < way->node_ref_count; i++) {
//            ref = way->node_refs + i;
//            c_way.dots.push_back(*ref);
//            nodes_count[*ref]++;
//            nodes_links[*ref].push_back(c_way.id);
//            ways[c_way.id].dots.push_back(*ref);
//            if (nodes_count[*ref] > 1) {
//                cout << "\nOK " << *ref << " " << nodes_count[*ref] << " " << c_way.id << " " << i << endl;
//                if (i != 0 && i != way->node_ref_count-1)
//                    divideWay(c_way.id, *ref);
//                cout << "OK1" << endl;
//                for (int j=0; j < nodes_links[*ref].size()-1; j++) {
//                    way_link_id = nodes_links[*ref][j];
//                    cout << "\nOK2 " << *ref << " " << nodes_count[*ref] << " " << way_link_id << " " << i << endl;
//                    if (ways[way_link_id].dots[0] != *ref && ways[way_link_id].dots[ways[way_link_id].dots.size()-1] != *ref)
//                        divideWay(way_link_id, *ref);
//                }
//            }
//        }
//        if (ways[c_way.id].dots.empty())
//            ways.erase(c_way.id);
//    }
//    if (evalAbort (helper))
//        return READOSM_ABORT;
//    return READOSM_OK;
//}

//void ParseInput::divideWay(int way_id, int node_id) {
//    vector<long long> dots(ways[way_id].dots);
//    custom_way new_way;
//    bool found = false;
//    new_way.id = id_count++;
//    for (int i=0; i < dots.size(); i++) {
//        cout << "LOL " << dots[i] << endl;
//        if (dots[i] != node_id) {
//            cout << "NOT FOUND" << endl;
//            new_way.dots.push_back(dots[i]);
//            nodes_links[dots[i]].push_back(new_way.id);
//            nodes_links[dots[i]].erase(remove(nodes_links[dots[i]].begin(), nodes_links[dots[i]].end(), way_id), nodes_links[dots[i]].end());
//            ways[way_id].dots.erase(ways[way_id].dots.begin()+i);
//        }
//        if (dots[i] == node_id) {
//            cout << "FOUND" << endl;
//            new_way.dots.push_back(dots[i]);
//            nodes_links[dots[i]].push_back(new_way.id);
//            break;
//        }
//    }
//    if (!new_way.dots.empty())
//        ways[new_way.id] = new_way;
//}

//// left bound to 0
//// divide by the resulting right bound
//// multiply by the size (100)
//// substract by 50
//vector<double> ParseInput::findCoordinates(double lat, double lon) {
//    int pixel_width = 100;
//    int pixel_height = 100;
//    double map_lon_delta = largest_lon - smallest_lon;
//    double map_lat_bottom_degree = smallest_lat * M_PI / 180;


//    float x = (lon - smallest_lon) * (pixel_width / map_lon_delta);
//    lat = lat * M_PI /180;
//    double world_map_width = ((pixel_width / map_lon_delta) * 360) / (2 * M_PI);
//    double map_offset_Y = (world_map_width / 2 * log((1 + sin(map_lat_bottom_degree)) / (1 - sin(map_lat_bottom_degree))));
//    float y = pixel_height - ((world_map_width / 2 * log((1 + sin(lat)) / (1 - sin(lat)))) - map_offset_Y);

//    cout << "X " << x << "Y " << y << endl;

//    lat -= smallest_lat;
//    lat /= (largest_lat - smallest_lat);
//    lat *= 100;
//    lat -= 50;

//    lon -= smallest_lon;
//    lon /= (largest_lon - smallest_lon);
//    lon *= 100;
//    lon -= 50;

//    return {lat, 0, lon};
//}


//int ParseInput::parseRelation(const void *user_data, const readosm_relation * relation) {
//    struct osm_helper *helper = (struct osm_helper *) user_data;
//    if (evalAbort (helper))
//        return READOSM_ABORT;
//    return READOSM_OK;
//}

//int ParseInput::evalAbort (struct osm_helper *helper) {
//    if (helper->read_count > helper->stop_limit)
//        return 1;
//    return 0;
//}

//bool ParseInput::checkCurved(point a, point b, point c) {
//    vector<float> vec1 = {a.x- b.x, a.y - b.y, a.z - b.z};
//    vector<float> vec2 = {b.x- c.x, b.y - c.y, b.z - c.z};

//    point cross = {vec1[1]*vec2[2] - vec1[2]*vec2[1], vec1[2]*vec2[0] - vec1[0]*vec2[2], vec1[0]*vec2[1] - vec1[1]*vec2[0]};
//    float length = sqrt(pow(cross.x,2)+ pow(cross.y,2) + pow(cross.z,2));
//    cout << "LENGTH: " << length << endl;
//    if (length == 0)
//        return false;
//    return true;
//}

//static vector<float> solvexy(double a, double b, double c, double d, double e, double f) {
//    float j = (c - a / d * f) / (b - a * e / d);
//    float i = (c - (b * j)) / a;
//    return {i, j};
//}

//static double b0(double t) { return pow(1 - t, 3); }
//static double b1(double t) { return t * (1 - t) * (1 - t) * 3; }
//static double b2(double t) { return (1 - t) * t * t * 3; }
//static double b3(double t) { return pow(t, 3); }

//vector<point> ParseInput::approximateCurve(point start, point end, vector<point> control) {
//    //if (control.size()==1)
//    double c1 = sqrt(pow((control[0].x - start.x),2) + pow((control[0].z - start.z),2));
//    double c2 = sqrt(pow((control[1].x - control[0].x),2) + pow((control[1].z - control[0].z),2));
//    double c3 = sqrt(pow((end.x - control[1].x),2) + pow((end.z - control[1].z),2));

//    double t1 = c1 / (c1 + c2 + c3);
//    double t2 = (c1 + c2) / (c1 + c2 + c3);

//    vector<float> out1 = solvexy(b1(t1), b2(t1), control[0].x - (start.x * b0(t1)) - (end.x * b3(t1)), b1(t2), b2(t2), control[1].x - (start.x * b0(t2)) - (end.x * b3(t2)));
//    vector<float> out2 = solvexy(b1(t1), b2(t1), control[0].z - (start.z * b0(t1)) - (end.z * b3(t1)), b1(t2), b2(t2), control[1].z - (start.z * b0(t2)) - (end.z * b3(t2)));

//    point con1 = {out1[0], 0, out2[0]};
//    point con2 = {out1[1], 0, out2[1]};
//    return {con1, con2};
//}

//vector<point> ParseInput::findCurve(vector<long long> dots) {
//    if (dots.size() < 4)
//        return {};

//    bool curved = false;
//    for (int i=0; i<dots.size()-2; i+=3) {
//        if (i+3 >= dots.size())
//            break;
//        if (!checkCurved(nodes[dots[i]].pixel, nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel)) {
//            curved = true;
//            break;
//        }
//    }
//    if (!curved)
//        return {};

//    vector<point> support;
//    vector<point> support_dots = {};
//    // dots are ids
//    support_dots.push_back(nodes[dots[0]].pixel);
//    for (int i=0; i<dots.size()-2; i+=3) {
//        if (i == dots.size()-3) {
//            int num = dots.size() - i;
//            if (num == 1)
//                support = {nodes[dots[i]].pixel, nodes[dots[i]].pixel};
//            else if(num == 2)
//                support = {nodes[dots[i+1]].pixel, nodes[dots[i+1]].pixel};
//                //support = approximateCurve(nodes[dots[i]].pixel, nodes[dots[i+2]].pixel, {nodes[dots[i+1]].pixel});
//            else
//                support = approximateCurve(nodes[dots[i]].pixel, nodes[dots[i+3]].pixel, {nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel});
//        } else
//            support = approximateCurve(nodes[dots[i]].pixel, nodes[dots[i+3]].pixel, {nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel});
//        support_dots.push_back(support[0]);
//        support_dots.push_back(support[1]);
//    }
//    support_dots.push_back(nodes[dots[dots.size()-1]].pixel);
//    return support_dots;
//}


//void ParseInput::populateFile() {
//    vector<double> coord;
//    custom_node  n;
//    int id = 0;
//    map<int, int> id_link;
//    for (auto way: ways) {
//        n = nodes[way.second.dots[0]];
//        if (id_link.find(n.id) != id_link.end()) {
//            id_link[n.id] = id++;
//            coord = findCoordinates(n.lat, n.lon);
//            nodes[way.second.dots[0]].pixel = {(float) coord[0], (float) coord[1], (float) coord[2]};
//            myfile << to_string(id_link[n.id]);
//            myfile << "," + to_string(coord[0]) + "," + to_string(coord[1]) + "," + to_string(coord[2]) + "\n";
//        }
//    }
//    myfile << "\n";
//    vector<point> control;
//    for (auto way: ways) {
//        myfile << to_string(id_link[way.second.dots[0]]) + "," + to_string(id_link[way.second.dots[way.second.dots.size()-1]]) + "\n";
//        control = findCurve(way.second.dots);
//        cout << "SIZE: " << control.size() << endl;
//        //cout << "CONTROL: " << control[0] << " " << control[1] << endl;
//    }
//}

//int main (int argc, char *argv[]) {
//    if (argc != 3)
//      return -1;
//    ParseInput* parse = new ParseInput(argv[1]);
//    return 0;
//}
