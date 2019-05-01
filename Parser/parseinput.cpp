#include "parseinput.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <iterator>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>


using namespace std;

ofstream ParseInput::myfile;
ParseInput::ParseInput(char* file_name) {
    myfile.open ("example.csv");
    this->file_name = "map2.osm";
    const void *osm_handle;
    int ret;
    struct osm_helper helper;
    helper.read_count = 0;
    helper.stop_limit = 100;

    ret = readosm_open(file_name, &osm_handle);
    if (ret != READOSM_OK) {
        cout << "NOT OK1" << endl;
        readosm_close(osm_handle);
        return;
    }
    ret = readosm_parse(osm_handle, &helper, parseNode, parseWay, parseRelation);
    if (ret != READOSM_OK) {
        cout << "NOT OK2" << endl;
        readosm_close (osm_handle);
        return;
    }
    readosm_close (osm_handle);
    populateFile();
    myfile.close();
}

map<int, custom_node> ParseInput::nodes = {};
int ParseInput::parseNode (const void *user_data, const readosm_node * node) {
    struct osm_helper *helper = (struct osm_helper *) user_data;
    nodes[node->id] = {node->id, node->latitude, node->longitude};
    nodes_count[node->id] = 0;

    if (node->latitude != READOSM_UNDEFINED) {
        if (node->latitude > largest_lat)
            largest_lat = node->latitude;
        if (node->latitude < smallest_lat)
             smallest_lat = node->latitude;
    }
    if (node->longitude != READOSM_UNDEFINED) {
        if (node->longitude > largest_lon)
            largest_lon = node->longitude;
        if (node->longitude < smallest_lon)
            smallest_lon = node->longitude;
    }

    if (evalAbort (helper))
        return READOSM_ABORT;
    return READOSM_OK;
}

map<int, custom_way> ParseInput::ways = {};
double ParseInput::largest_lat = -181;
double ParseInput::largest_lon = -181;
double ParseInput::smallest_lat = 181;
double ParseInput::smallest_lon = 181;
double ParseInput::largest_x = - numeric_limits<double>::infinity();
double ParseInput::largest_z = - numeric_limits<double>::infinity();
double ParseInput::smallest_x = numeric_limits<double>::infinity();
double ParseInput::smallest_z = numeric_limits<double>::infinity();
map<int, int> ParseInput::nodes_count = {};
map<int, vector<int>> ParseInput::nodes_links = {};
int ParseInput::id_count = 0;

int ParseInput::parseWay (const void *user_data, const readosm_way * way) {
    struct osm_helper *helper = (struct osm_helper *) user_data;
    custom_way c_way;
    const readosm_tag* tag;
    const long long* ref;
    bool found = false;
    int way_link_id;
    for (int i=0; i < way->tag_count; i++) {
        tag = way->tags + i;
        if (strcmp(tag->key, "highway") == 0 && 
		((strcmp(tag->value, "residential") == 0 || strcmp(tag->value, "primary") == 0) || strcmp(tag->value, "secondary") == 0 || strcmp(tag->value, "unclassified") == 0 || strcmp(tag->value, "tertiary") == 0))
            found = true;
    }
    if (found == true) {
        c_way.id = way->id;
        ways[c_way.id] = c_way;
        for (int i=0; i < way->node_ref_count; i++) {
            ref = way->node_refs + i;
            c_way.dots.push_back(*ref);
            nodes_count[*ref]++;
            nodes_links[*ref].push_back(c_way.id);
            ways[c_way.id].dots.push_back(*ref);
            if (nodes_count[*ref] > 1) {
                if (i != 0 && i != way->node_ref_count-1)
                    divideWay(c_way.id, *ref);
                for (int j=0; j < nodes_links[*ref].size()-1; j++) {
                    way_link_id = nodes_links[*ref][j];
                    if (ways[way_link_id].dots[0] != *ref && ways[way_link_id].dots[ways[way_link_id].dots.size()-1] != *ref)
                        divideWay(way_link_id, *ref);
                }
            }
        }
        if (ways[c_way.id].dots.empty())
            ways.erase(c_way.id);
    }
    if (evalAbort (helper))
        return READOSM_ABORT;
    return READOSM_OK;
}

void ParseInput::divideWay(int way_id, int node_id) {
    vector<long long> dots(ways[way_id].dots);
    custom_way new_way;
    bool found = false;
    new_way.id = id_count++;
	cout << "IDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD " << new_way.id << endl;
	if (new_way.id == 5) {
		cout << "??????????????????????????????????????????? " << way_id << " " << node_id << endl;
	}
    for (int i=0; i < dots.size(); i++) {
        cout << "LOL " << dots[i] << endl;
        if ((int)dots[i] != (int)node_id) {
            cout << "NOT FOUND" << endl;
            new_way.dots.push_back(dots[i]);
            nodes_links[dots[i]].push_back(new_way.id);
            nodes_links[dots[i]].erase(remove(nodes_links[dots[i]].begin(), nodes_links[dots[i]].end(), way_id), nodes_links[dots[i]].end());
			cout << "LOL3 " << ways[way_id].dots[i] << " " << i << endl;
			ways[way_id].dots.erase(remove(ways[way_id].dots.begin(), ways[way_id].dots.end(), dots[i]), ways[way_id].dots.end());
            //ways[way_id].dots.erase(ways[way_id].dots.begin()+i);
			cout << "LOL4" << endl;
        }
        if ((int)dots[i] == (int)node_id) {
            cout << "FOUND" << endl;
            new_way.dots.push_back(dots[i]);
            nodes_links[dots[i]].push_back(new_way.id);
            break;
        }
    }
	cout << "LOL5" << endl;
    if (!new_way.dots.empty())
        ways[new_way.id] = new_way;
}

// left bound to 0
// divide by the resulting right bound
// multiply by the size (100)
// substract by 50
vector<float> ParseInput::findCoordinates(double lat, double lon) {    
    double map_lon_delta = largest_lon - smallest_lon;
    double map_lat_bottom_degree = smallest_lat * M_PI / 180;
	cout << "DELTA: " << map_lon_delta << endl;
	int pixel_width = 20000 * map_lon_delta;
    int pixel_height = 20000 * map_lon_delta;
	
    float x = (lon - smallest_lon) * (pixel_width / map_lon_delta);
    float test_lat = lat * M_PI /180;
    double world_map_width = ((pixel_width / map_lon_delta) * 360) / (2 * M_PI);
    double map_offset_Z = (world_map_width / 2 * log((1 + sin(map_lat_bottom_degree)) / (1 - sin(map_lat_bottom_degree))));
    float z = pixel_height - ((world_map_width / 2 * log((1 + sin(test_lat)) / (1 - sin(test_lat)))) - map_offset_Z);
    cout << "X " << x << " Z " << z << " " << " LAT " << lat << " LON " << lon << endl;

	
	if (x < smallest_x)
		smallest_x = x;
	else if (x > largest_x)
		largest_x = x;
	if (z < smallest_z)
		smallest_z = z;
	else if (z > largest_z)
		largest_z = z;
	
	return {x, 0, z};
}


int ParseInput::parseRelation(const void *user_data, const readosm_relation * relation) {
    struct osm_helper *helper = (struct osm_helper *) user_data;
    if (evalAbort (helper))
        return READOSM_ABORT;
    return READOSM_OK;
}

int ParseInput::evalAbort (struct osm_helper *helper) {
    if (helper->read_count > helper->stop_limit)
        return 1;
    return 0;
}

void ParseInput::populateFile() {
    vector<float> coord;
    custom_node  n;
    int id = 0;
    map<int, int> id_link;
	
	vector<float> output;	
	for (auto way: ways) {
		for (long long c_n: way.second.dots) {
			output = findCoordinates(nodes[c_n].lat, nodes[c_n].lon);
			nodes[c_n].pixel = {output[0], output[1], output[2]};
			cout << "COOOOOORDDDDDDDDDDDDD: " << output[0] << " " << output[1] << " " << output[2] << " " << nodes[c_n].lat << " " << nodes[c_n].lon << " " << way.second.id << " " << c_n << endl;
		}
		findCurve(way.second.id);
	}
		
	float fix_x = largest_x/2 + smallest_x/2;
	float fix_z = largest_z/2 + smallest_z/2;
    for (auto way: ways) {
		n = nodes[way.second.dots[0]];
		if (id_link.find(n.id) == id_link.end()) {
			id_link[n.id] = id++;
			myfile << to_string(id_link[n.id]);
			n.pixel.x -= fix_x;
			n.pixel.z -= fix_z;
			myfile << "," + to_string(n.pixel.x) + "," + to_string(n.pixel.y) + "," + to_string(n.pixel.z) + "\n";
		}
		n = nodes[way.second.dots[way.second.dots.size()-1]];
		if (id_link.find(n.id) == id_link.end()) {
			id_link[n.id] = id++;
			myfile << to_string(id_link[n.id]);
			n.pixel.x -= fix_x;
			n.pixel.z -= fix_z;
			myfile << "," + to_string(n.pixel.x) + "," + to_string(n.pixel.y) + "," + to_string(n.pixel.z) + "\n";
		}
    }
    myfile << "\n";
	
    for (auto way: ways) {
		myfile << to_string(id_link[way.second.dots[0]]) + "," + to_string(id_link[way.second.dots[way.second.dots.size()-1]]);
		if (!way.second.control.empty()) {
			way.second.control[0].x -= fix_x;
			way.second.control[0].z -= fix_z;
			way.second.control[1].x -= fix_x;
			way.second.control[1].z -= fix_z;
			myfile << "," + to_string(way.second.control[0].x) + "," + to_string(way.second.control[0].y) + "," + to_string(way.second.control[0].z);
			myfile << "," + to_string(way.second.control[1].x) + "," + to_string(way.second.control[1].y) + "," + to_string(way.second.control[1].z);
		}
		myfile << "\n";
    }
}

bool ParseInput::checkCurved(point a, point b, point c) {
	point vec1 = {a.x-b.x, a.y-b.y, a.z-b.z};
    point vec2 = {b.x-c.x, b.y-c.y, b.z-c.z};
	float length1 = sqrt(pow(vec1.x,2) + pow(vec1.z,2));
	float length2 = sqrt(pow(vec2.x,2) + pow(vec2.z,2));
    vec1 = {(a.x- b.x)/length1, (a.y - b.y)/length1, (a.z - b.z)/length1};
    vec2 = {(b.x- c.x)/length1, (b.y - c.y)/length1, (b.z - c.z)/length1};

    point cross = {vec1.y*vec2.z - vec1.z*vec2.y, vec1.z*vec2.x - vec1.x*vec2.z, vec1.x*vec2.y - vec1.y*vec2.x};
    float length = sqrt(pow(cross.x,2)+ pow(cross.y,2) + pow(cross.z,2));
    cout << "LENGTH: " << length << endl;
    if (length < 0.2 && length > -0.2)
        return false;
	cout << "ASD" << endl;
    return true;
}

static vector<float> solvexy(double a, double b, double c, double d, double e, double f) {
    float j = (c - a / d * f) / (b - a * e / d);
    float i = (c - (b * j)) / a;
    return {i, j};
}

static double b0(double t) { return pow(1 - t, 3); }
static double b1(double t) { return t * (1 - t) * (1 - t) * 3; }
static double b2(double t) { return (1 - t) * t * t * 3; }
static double b3(double t) { return pow(t, 3); }

vector<point> ParseInput::approximateCurve(point start, point end, vector<point> control) {
    //if (control.size()==1)
    double c1 = sqrt(pow((control[0].x - start.x),2) + pow((control[0].z - start.z),2));
    double c2 = sqrt(pow((control[1].x - control[0].x),2) + pow((control[1].z - control[0].z),2));
    double c3 = sqrt(pow((end.x - control[1].x),2) + pow((end.z - control[1].z),2));

    double t1 = c1 / (c1 + c2 + c3);
    double t2 = (c1 + c2) / (c1 + c2 + c3);
	
	cout << "start " << start.x << " " << start.z << endl;
	cout << "end " << end.x << " " << end.z << endl;
	cout << "control0 " << control[0].x << " " << control[0].z << endl;
	cout << "control1 " << control[1].x << " " << control[1].z << endl;
	cout << "CCCCCc " << c1 << " " << c2 << " " << c3 << endl;
	cout << "TTTTTTT " << t1 << " " << t2 << endl;
    vector<float> out1 = solvexy(b1(t1), b2(t1), control[0].x - (start.x * b0(t1)) - (end.x * b3(t1)), b1(t2), b2(t2), control[1].x - (start.x * b0(t2)) - (end.x * b3(t2)));
    vector<float> out2 = solvexy(b1(t1), b2(t1), control[0].z - (start.z * b0(t1)) - (end.z * b3(t1)), b1(t2), b2(t2), control[1].z - (start.z * b0(t2)) - (end.z * b3(t2)));

	cout << "OUT1 " << out1[0] << " " << out1[1] << endl;
    point con1 = {out1[0], 0, out2[0]};
    point con2 = {out1[1], 0, out2[1]};
    return {start, con1, con2, end};
}

vector<point> ParseInput::findCurve(long long way_id) {
	vector<long long> dots = ways[way_id].dots;
	cout << "START " << dots.size() << endl;
    if (dots.size() < 3)
        return {};

	cout << "OK1" << endl;
    bool curved = false;
	vector<point> support;
	custom_way new_way;
	
	for (int i=0; i<dots.size()-1; i+=2) {
		if (checkCurved(nodes[dots[i]].pixel, nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel)) {
			if (i != 0) {
				cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDD" << endl;
				divideWay(way_id, dots[i]);
				dots = ways[way_id].dots;
				i = 0;
			}
			cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDD2" << endl;
			if (i == dots.size()-4) {
				support = approximateCurve(nodes[dots[i]].pixel, nodes[dots[i+3]].pixel, {nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel});
				cout << "111111111111111111111111111111111111111111111111111111 " << support[1].x << " " << support[1].z << " " << support[2].x << " " << support[2].z << endl;
				ways[way_id].control = {support[1], support[2]};
			} else if (i == dots.size()-3) {
				point support1 = {(1-0.3)*nodes[dots[i]].pixel.x+0.3*nodes[dots[i+1]].pixel.x, 0, (1-0.3)*nodes[dots[i]].pixel.z+0.3*nodes[dots[i+1]].pixel.z};
				point support2 = {(1-0.7)*nodes[dots[i]].pixel.x+0.7*nodes[dots[i+1]].pixel.x, 0, (1-0.7)*nodes[dots[i]].pixel.z+0.7*nodes[dots[i+1]].pixel.z};
				cout << "333333333333333333333333333333333333333333333333333333333 " << support1.x << " " << support1.z << " " << support2.x << " " << support2.z << endl;
				ways[way_id].control = {support1, support2};
			} else if (i == dots.size()-2) {
				point support1 = {(1-0.3)*nodes[dots[i]].pixel.x+0.3*nodes[dots[i+1]].pixel.x, 0, (1-0.3)*nodes[dots[i]].pixel.z+0.3*nodes[dots[i+1]].pixel.z};
				point support2 = {(1-0.7)*nodes[dots[i]].pixel.x+0.7*nodes[dots[i+1]].pixel.x, 0, (1-0.7)*nodes[dots[i]].pixel.z+0.7*nodes[dots[i+1]].pixel.z};
				cout << "22222222222222222222222222222222222222222222222222222222 " << support1.x << " " << support1.z << endl;
				ways[way_id].control = {support1, support2};
			} else {
				support = approximateCurve(nodes[dots[i]].pixel, nodes[dots[i+3]].pixel, {nodes[dots[i+1]].pixel, nodes[dots[i+2]].pixel});
				cout << "44444444444444444444444444444444444444444 " << support[1].x << " " << support[1].z << " " << support[2].x << " " << support[2].z << endl;
				new_way.id = id_count++;
				cout << "NNEWWWWWWWWWWW_IDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDd " << new_way.id << " " << dots[i] << " " << dots[i+3] << endl;
				new_way.dots = {dots[i], dots[i+3]};
				cout << "SUPORTTT " << support[1].x << endl;
				new_way.control = {support[1], support[2]};
				ways[new_way.id] = new_way;
				cout << "IIIIIIIIII " << i << endl;
				ways[way_id].dots.erase(remove(ways[way_id].dots.begin(), ways[way_id].dots.end(), dots[i]), ways[way_id].dots.end());
				ways[way_id].dots.erase(remove(ways[way_id].dots.begin(), ways[way_id].dots.end(), dots[i+1]), ways[way_id].dots.end());
				ways[way_id].dots.erase(remove(ways[way_id].dots.begin(), ways[way_id].dots.end(), dots[i+2]), ways[way_id].dots.end());
				dots = ways[way_id].dots;
				i-=2;
			}
		}
	}
}


int main (int argc, char *argv[]) {
    if (argc != 3)
      return -1;
    ParseInput* parse = new ParseInput(argv[1]);
    return 0;
}
