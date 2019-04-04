#include "blocksubdivision.h"

using namespace std;


BlockSubdivision::BlockSubdivision(Junction* junction) {
    this->junction = junction;
}

int BlockSubdivision::orientation(point p, point q, point r) {
    int val = (q.z - p.z) * (r.x - q.x) - (q.x - p.x) * (r.z - q.z);
    if (val == 0) return 0;
    return (val > 0)? 1: 2;
}

int BlockSubdivision::distSq(point p1, point p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.z - p2.z)*(p1.z - p2.z);
}

point BlockSubdivision::nextToTop(vector<point> &S) {
    point p = S[S.size()-1];
    S.pop_back();
    point res = S[S.size()-1];
    S.push_back(p);
    return res;
}

int BlockSubdivision::swap(point &p1, point &p2) {
    point temp = p1;
    p1 = p2;
    p2 = temp;
    return 1;
}

graphVector BlockSubdivision::Normalise(graphVector vec) {
    float length = sqrt(pow(vec.x,2) + pow(vec.y,2) + pow(vec.z,2));
    return {vec.x/length, vec.y/length, vec.z/length};
}

graphVector BlockSubdivision::Perp(graphVector vec) {
    return {-vec.z, vec.y, vec.x};
}

int BlockSubdivision::Dot(graphVector vec1, graphVector vec2) {
    return vec1.x*vec2.x + vec1.y+vec2.y + vec1.z+vec2.z;
}

void BlockSubdivision::drawSmallestRectangle() {
    point c0,c1,c2,c3,c4;
    glPointSize(6);
    glBegin(GL_POINTS);
    for (int i=0; i<smallest_rect.size(); i++) {
        for (int j=0; j<smallest_rect[i].index.size(); j++) {
            c0 = convex_hulls[i][smallest_rect[i].index[0]];
            c1 = convex_hulls[i][smallest_rect[i].index[1]];
            c2 = convex_hulls[i][smallest_rect[i].index[j]];
            glColor3f(0,0,0);
            glVertex3f(c0.x, 6, c0.z);
            glVertex3f(c1.x, 6, c1.z);
            //cout <<"OPOPOPOP " << c0.x << " " << c0.z << " " << c1.x << " " << c1.z << endl;
            glColor3f(1,0,1);
            glVertex3f(c2.x, 6, c2.z);
        }
    }
    glEnd();

    glColor3f(1,0,1);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    for (int i=0; i<smallest_rect.size(); i++) {
            point n1 = smallest_rect[i].nodes[0];
            point n2 = smallest_rect[i].nodes[1];
            point n3 = smallest_rect[i].nodes[2];
            point n4 = smallest_rect[i].nodes[3];
            glVertex3f(n1.x, 6, n1.z);
            glVertex3f(n2.x, 6, n2.z);
            glVertex3f(n3.x, 6, n3.z);
            glVertex3f(n4.x, 6, n4.z);

            glVertex3f(n1.x, 6, n1.z);
            glVertex3f(n4.x, 6, n4.z);
            glVertex3f(n2.x, 6, n2.z);
            glVertex3f(n3.x, 6, n3.z);
    }
    glEnd();
}

vector<vector<point>> BlockSubdivision::allSubdivision(vector<vector<point>> objects_p) {
    vector<vector<point>> output = {}, divided = {};
    for (int i=0; i<objects_p.size(); i++) {
    //for (vector<point> object_p: objects_p) {
        divided = subdivision(objects_p[i]);
        output.insert(output.end(), divided.begin(), divided.end());
    }
    //return divided;
    return output;
}

vector<vector<point>> BlockSubdivision::subdivision(vector<point> concave_p) {
    vector<point> convex_p = convexHull(concave_p);
    rectangle bounding_box = smallestRectangle(convex_p);
    if (bounding_box.area < 30 || isinf(bounding_box.area)){
        return {concave_p};
    }
    vector<vector<point>> all_objects, objects, output;
    vector<vector<point>> divided_rects = divideRect(bounding_box, concave_p);
    if (divided_rects.empty())
        return {concave_p};

    objects = subdivision(divided_rects[0]);
    all_objects.insert(all_objects.end(), objects.begin(), objects.end());
    objects = subdivision(divided_rects[1]);
    all_objects.insert(all_objects.end(), objects.begin(), objects.end());
    return all_objects;
}

// requires OBB and convex hull
vector<vector<point>> BlockSubdivision::divideRect(rectangle rect, vector<point> dots) {
    float largest_dist;
    int next = 0, count;
    vector<vector<point>> divided_rects = {{},{}};
    array<point,2> line;
    edge_offset e1, e2;
    vector<float> t_u;
    point p;
    vector<int> inter_i = {};
    vector<point> inter_p = {};

    if (-rect.leftmost + rect.rightmost >= rect.upmost) {
        largest_dist = (rect.leftmost + rect.rightmost)/2;
        line[0] = {rect.origin.x + rect.U[0].x*largest_dist, 0, rect.origin.z + rect.U[0].z*largest_dist};
        line[1] = {line[0].x + rect.U[1].x*(rect.upmost)*2, 0, line[0].z + rect.U[1].z*(rect.upmost)*2};
        line[0] = {line[0].x + Perp(Perp(rect.U[1])).x*100, 0, line[0].z + Perp(Perp(rect.U[1])).z*100};
    } else {
        largest_dist = rect.upmost/2;
        line[0] = {rect.nodes[0].x + rect.U[1].x*largest_dist, 0, rect.nodes[0].z + rect.U[1].z*largest_dist};
        line[0] = {line[0].x + Perp(Perp(rect.U[0])).x*100, 0, line[0].z + Perp(Perp(rect.U[0])).z*100};
        line[1] = {rect.nodes[1].x + rect.U[1].x*largest_dist, 0, rect.nodes[1].z + rect.U[1].z*largest_dist};
        line[1] = {line[1].x + rect.U[0].x*100, 0, line[1].z + rect.U[0].z*100};
    }

    e1.dots = {line[0], line[1]};
    for (int i=0; i<2; i++) {
        e2.dots = {};
        e2.dots.insert(e2.dots.end(), dots.begin()+next, dots.end());
        if (next!=0)
            e2.dots.push_back(dots[0]);
//        if (i == 2) {
//            if (e2.dots.size() == 1)
//                break;
//            t_u = junction->findIntersection(e1, e2);
//            if (t_u[1] > 0 && t_u[1] < 1)
//                inter_i = {};
//            break;
//        }
        t_u = junction->findIntersection(e1, e2);
        if (isnan(t_u[0]) || isnan(t_u[1])) {
            cerr << "[Error] Cannot find subdivision intersection" << endl;
            return {};
        }
        p = {(1-t_u[0])*e1.dots[0].x + t_u[0]*e1.dots[1].x, 0, (1-t_u[0])*e1.dots[0].z + t_u[0]*e1.dots[1].z};
        inter_p.push_back(p);
        inter_i.push_back(t_u[3]+next);
        next += t_u[3]+1;
    }

    divided_rects[0].push_back(inter_p[0]);
    for (int i=inter_i[0]+1; i<=inter_i[1]; i++)
        divided_rects[0].push_back(dots[i]);
    divided_rects[0].push_back(inter_p[1]);

    divided_rects[1].push_back(inter_p[1]);
    count = inter_i[1]+1;
    while (true) {
        if (count == dots.size())
            count = 0;
        divided_rects[1].push_back(dots[count]);
        if (count == inter_i[0])
            break;
        count++;
    }
    divided_rects[1].push_back(inter_p[0]);

    return divided_rects;
}

float BlockSubdivision::pointDistance(graphVector n, point a, point p) {
    float c = -n.x*a.x -n.z*a.z;
    float result = (n.x*p.x + n.z*p.z + c);
    float length = sqrt(pow(n.x,2) + pow(n.z,2));
    result /= length;
    return result;
}

void BlockSubdivision::findSmallestRectangles() {
    int end;
    rectangle best_rect;
    for (vector<point> convex: convex_hulls) {
        best_rect = smallestRectangle(convex);
        smallest_rect.push_back(best_rect);
    }
}

float BlockSubdivision::convexArea(vector<point> convex) {
    float area = 0;
    int j = convex.size() - 1;
    for (int i = 0; i < convex.size(); i++) {
        area += (convex[j].x + convex[i].x) * (convex[j].z - convex[i].z);
        j = i;
    }
    return fabs(area/2);
}

rectangle BlockSubdivision::smallestRectangle(vector<point> convex) {
    rectangle rect, best_rect;
    int v1, v2;
    float distance, rev_distance, largest_dist, largest_rev_dist, smallest_rev_dist;
    best_rect.area = numeric_limits<float>::infinity();
    float convex_area = convexArea(convex);
    for (int i=0; i<convex.size(); i++) {
        v2 = i+1;
        v1 = i;
        if (v2 == convex.size())
            v2 = 0;
        rect.U[0] = {convex[v2].x - convex[v1].x, convex[v2].y - convex[v1].y, convex[v2].z - convex[v1].z};
        rect.U[0] = Normalise(rect.U[0]);
        rect.U[1] = Perp(rect.U[0]);
        rect.index = {v1,v2,v2,v2,v2};
        rect.origin = convex[v1];
        largest_dist = 0;
        largest_rev_dist = 0;
        smallest_rev_dist = numeric_limits<float>::infinity();
        for (int i=0; i<convex.size(); ++i) {
            distance = pointDistance(rect.U[1], rect.origin, convex[i]);
            rev_distance = pointDistance(rect.U[0], rect.origin, convex[i]);

            if (distance > largest_dist) {
                largest_dist = distance;
                rect.index[2] = i;
            }
            if (rev_distance > largest_rev_dist) {
                largest_rev_dist = rev_distance;
                rect.index[3] = i;
            }
            if (rev_distance < smallest_rev_dist) {
                smallest_rev_dist = rev_distance;
                rect.index[4] = i;
            }
        }
        rect.area = (largest_rev_dist - smallest_rev_dist) * largest_dist;


        if (rect.area < best_rect.area && rect.area >= convex_area) {
            best_rect= rect;
            best_rect.leftmost = smallest_rev_dist;
            best_rect.rightmost = largest_rev_dist;
            best_rect.upmost = largest_dist;
            best_rect.nodes[0] = {best_rect.origin.x + best_rect.U[0].x*best_rect.leftmost, 0, best_rect.origin.z + best_rect.U[0].z*best_rect.leftmost};
            best_rect.nodes[1] = {best_rect.origin.x + best_rect.U[0].x*best_rect.rightmost, 0, best_rect.origin.z + best_rect.U[0].z*best_rect.rightmost};
            best_rect.nodes[2] = {best_rect.nodes[1].x + best_rect.U[1].x*best_rect.upmost, 0, best_rect.nodes[1].z + best_rect.U[1].z*best_rect.upmost};
            best_rect.nodes[3] = {best_rect.nodes[0].x + best_rect.U[1].x*best_rect.upmost, 0, best_rect.nodes[0].z + best_rect.U[1].z*best_rect.upmost};
        }
    }
    return best_rect;
}

point BlockSubdivision::p0;

void BlockSubdivision::drawConvexHull() {
    glPointSize(4);
    glColor3f(0,1,0);
    glBegin(GL_POINTS);
    for (vector<point> dots: convex_hulls) {
        for (int i=0; i<dots.size(); i++) {
            glVertex3f(dots[i].x, 5, dots[i].z);
        }
    }
    glEnd();
}

vector<point> BlockSubdivision::convexHull(vector<point> points) {
   int zmin = points[0].z, min = 0;
   int n = points.size();
   for (int i = 1; i < n; i++) {
     int z = points[i].z;
     if ((z < zmin) || (zmin == z && points[i].x < points[min].x)) {
        zmin = points[i].z;
        min = i;
     }
   }

   swap(points[0], points[min]);
   p0 = points[0];
   qsort(&points[1], n-1, sizeof(point), compare);
   int m = 1;
   for (int i=1; i<n; i++) {
       while (i < n-1 && orientation(p0, points[i], points[i+1]) == 0)
          i++;
       points[m] = points[i];
       m++;
   }
   if (m < 3) return {};

   vector<point> S;
   S.push_back(points[0]);
   S.push_back(points[1]);
   S.push_back(points[2]);

   for (int i = 3; i < m; i++) {
      while (orientation(nextToTop(S), S[S.size()-1], points[i]) != 2) {
         S.pop_back();
         if (S.size() == 1)
             return S;
      }
      S.push_back(points[i]);
   }
   convex_hulls.push_back(S);
   return S;
}

int BlockSubdivision::compare(const void *vp1, const void *vp2) {
   point *p1 = (point *)vp1;
   point *p2 = (point *)vp2;

   // Find orientation
   int o = orientation(p0, *p1, *p2);
   if (o == 0)
     return (distSq(p0, *p2) >= distSq(p0, *p1))? -1 : 1;

   return (o == 2)? -1: 1;
}
