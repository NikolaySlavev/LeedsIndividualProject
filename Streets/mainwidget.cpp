#include <Windows.h>
#include <iostream>
#include <QtGui>
#include "MainWidget.h"
#include <algorithm>
#include "Database.h"

using namespace std;

MainWidget::MainWidget(QWidget *parent):QGLWidget(parent) {
}

// called when OpenGL context is set up
void MainWidget::initializeGL() {
    glClearColor(0.3, 0.3, 0.3, 0.0);

    Database *test = new Database();
    test->CreateTables();
//    gridStreets(-40, 10, 10);
//    cout << 11111 << endl;
//    //findBlocks_test();
//    cout << "SIZEEAFAFAF " << objects.size() << endl;
//    for (int i = 0; i < objects.size(); i++) {
//        for (int j = 0; j < objects[i].size(); j++) {
//        cout << objects[i][j] << " ";
//        }
//        cout << endl;
//    }
//    //removeBlocks();

    cout << "SIZEEAFAFAF " << objects.size() << endl;
    //initialise_adj
    //initialise_offset
    nodes = {{-20,0,-10}, {0,0, -10}, {0, 0, 10}};
    initAdj();
    addEdge(0, {1});
    addEdge(1, {2});
    initOffset();
    addOffset({0,1}, calcOffsetLine(nodes[0], nodes[1]));
    addOffset({1,2}, calcOffsetLine(nodes[1], nodes[2]));
    //FindIntersection(offsetLines[0][1][0], offsetLines[0][1][1], offsetLines[2][1][2], offsetLines[2][1][3]);
    cout << "Find junction" << endl;
    findJunction(1); //node index

//    for (int i = 0; i < offsetLines.size(); i++) {
//        for (int j = 0; j < offsetLines[i].size(); j++) {
//            cout << offsetLines[i][j][0][0] << " ";
//        }
//        cout << endl;
//    }


}

void MainWidget::findJunction(int node_index) {
    vector<vector<float>> line1, line2;
    float intr1, intr2, smallest;
    for (int i=0; i<adj[node_index].size()-1; i++) {
        smallest = 20;
        for (int j=i+1; j<adj[node_index].size(); j++) {
            line1 = offsetLines[adj[node_index][i][0]][node_index];
            line2 = offsetLines[adj[node_index][j][0]][node_index];

            smallest = compareIntersection(line1[0], line1[1], line2[0], line2[1], smallest);
            smallest = compareIntersection(line1[0], line1[1], line2[2], line2[3], smallest);

            intr1 = findIntersection(line2[2], line2[3], line1[0], line1[1]);
            intr2 = findIntersection(line2[2], line2[3], line1[2], line1[3]);

            intr1 = findIntersection(line1[0], line1[1], line2[0], line2[1]);
            intr2 = findIntersection(line1[2], line1[3], line2[0], line2[1]);



            findIntersection(line1[0], line1[3], line2[2], line2[3]);
            //3(line1[2], line1[3], line2[2], line2[3]);
        }
        // when smallest found -> do the test on the smallest -> if they match ok
        //smallest_inter[node_index][adj[node_index][i][0][0]][0] = smallest;
    }
}

float MainWidget::compareIntersection(vector<float> line1_s, vector<float> line1_e, vector<float> line2_s, vector<float> line2_e, float smallest) {
    float intr = findIntersection(line2_s, line2_e, line1_s, line1_e);
    if (intr < smallest)
        return intr;
    return smallest;
}

vector<vector<float>> MainWidget::calcOffsetLine(vector<float> s, vector<float> e) {
    float line_length;
    vector<float> line_vector, up_s, down_s, up_e, down_e;

    line_vector = {abs(s[0] - e[0]), abs(s[1] - e[1]), abs(s[2] - e[2])};
    line_length = sqrt(pow(line_vector[0],2) + pow(line_vector[1],2) + pow(line_vector[2],2));

    vector<float> normal = {-line_vector[2]/line_length, line_vector[1]/line_length, line_vector[0]/line_length};
    vector<float> rev_normal = {line_vector[2]/line_length, line_vector[1]/line_length, -line_vector[0]/line_length};

    up_s = {s[0] + normal[0], s[1] + normal[1], s[2] + normal[2]};
    down_s = {s[0] + rev_normal[0], s[1] + rev_normal[1], s[2] + rev_normal[2]};

    up_e = {e[0] + normal[0], e[1] + normal[1], e[2] + normal[2]};
    down_e = {e[0] + rev_normal[0], e[1] + rev_normal[1], e[2] + rev_normal[2]};

    cout << up_s[0] << " " << up_s[1] << " " << up_s[2] << " " << endl;

    return {up_s, up_e, down_s, down_e};
}

float MainWidget::findIntersection(vector<float> s1, vector<float> e1, vector<float> s2, vector<float> e2) {
    //normal vector of line 1
    vector<float> line_vector = {abs(s1[0] - e1[0]), abs(s1[1] - e1[1]), abs(s1[2] - e1[2])};
    vector<float> normal = {-line_vector[2], line_vector[1], line_vector[0]};

    // loop over the dots in line 2
    float dot_product, t = -0.5;
    vector<float> move, vec;

    while ((1.5 - t) > -std::numeric_limits<double>::epsilon()) {
        move = {(1-t)*s2[0] + t*e2[0], (1-t)*s2[1] + t*e2[1], (1-t)*s2[2] + t*e2[2]};
        vec = {move[0] - e1[0], move[1] - e1[1], move[2]- e1[2]};
        dot_product = normal[0]*vec[0] + normal[1]*vec[1] + normal[2]*vec[2];
        dot_product = roundf(dot_product * 100) / 100;
        t += 0.001;
        //cout << "DOT PRODUCT: " << dot_product << endl;
        if (dot_product == 0) {
            cout << t << endl;
            cout << "RETURNNN " << move[0] << " " << move[1] << " " << move[2] << endl;
            return t;  // also t?
        }
    }
    cout << "NO RETURN" << endl;
    return 0;
    // use gradient descent?


    // link the offsetted lines to its axis
    // pass the offsetted lines for interception
    // pick the interseption point with larger t?
    // decrease the length of both street segments by 1? (or 10%)
    // use the three points and create a curve
}


void MainWidget::removeBlocks() {
    int count = 0;
    vector<int> erase_objects = {};
    for (int i=0; i < objects.size()-1; i++) {
        for (int j=i+1; j < objects.size(); j++) {
                for (int k=0; k < objects[j].size(); k++) {
                     if (std::find(begin(objects[i]), end(objects[i]), objects[j][k]) != objects[i].end()) {
                        count++;
                     }
                 }
                 if (count > 2) {
                     int larger;
                     if (objects[j].size() < objects[i].size())
                        larger = i;
                     else
                        larger = j;
                     if (std::find(begin(erase_objects), end(erase_objects), larger) == erase_objects.end()) {
                         erase_objects.push_back(larger);
                     }
                 }
                 count = 0;
        }
    }

    int temp;
    for (int i = 0; i < erase_objects.size()-1; i++) {
      for (int j = 0; j < erase_objects.size()-i-1; j++) {
            if (erase_objects[j] > erase_objects[j+1]) {
                temp = erase_objects[j];
                erase_objects[j] = erase_objects[j+1];
                erase_objects[j+1] = temp;
            }
      }
    }

    cout <<" SIZEEE " << erase_objects.size() << endl;
    for (int i=erase_objects.size()-1; i>=0; i--) {
        objects.erase(objects.begin() + erase_objects[i]);
    }
}

void MainWidget::drawBlocks() {
    for (int i=0; i < objects.size(); i++) {
        float red = (rand() % 100 + 50) / 256.0;
        float green = (rand() % 100 + 50) / 256.0;
        float blue = (rand() % 100 + 50) / 256.0;
        cout << " " << red << " " << green << " " << blue << endl;
        glColor3f(red,green,blue);
        glNormal3f(0,1,0);
        glBegin(GL_POLYGON);
            for (int j=0; j < objects[i].size(); j++) {
                //cout << objects[i][j] << " " << nodes[objects[i][j]][0] << " " << nodes[objects[i][j]][2] << endl;

                glVertex3f( nodes[objects[i][j]][0], 0, nodes[objects[i][j]][2]);
            }
        glEnd();
        //cout << endl;
    }
}

void MainWidget::findBlocks_test() {
    // Mark all the vertices as not visited
    int v = 0;
    vector<vector<bool>> visited_edges(adj.size());

    for (int i=0; i<adj.size(); i++) {
        for (int j=0; j<adj[i].size(); j++) {
            visited_edges[i].push_back(false);
        }
    }
    //HARD CODED
    stack = {0};
    visited_edges[0][0] = true;
    vector<int> found = {-1, 0};
    DFS_test(1, visited_edges, found);
}

void MainWidget::DFS_test(int v, vector<vector<bool>> visited_edges, vector<int> found) {
    int check;
    vector<float> vec, adj_vec;
    float dot_product, length_vec, length_adj_vec, angle, smallest_angle, determinant;
    int smallest = -1;
    vec = {nodes[v][0] - nodes[stack[stack.size()-1]][0], 0, nodes[v][2] - nodes[stack[stack.size()-1]][2]};
    smallest_angle = 361;

    for (int i = 0; i != adj[v].size(); ++i) {
        if (!visited_edges[v][i]) {
            adj_vec = {nodes[adj[v][i][0]][0] - nodes[v][0], 0, nodes[adj[v][i][0]][2] - nodes[v][2]};
            dot_product = vec[0]*adj_vec[0] + vec[2]*adj_vec[2];
            determinant = vec[0]*adj_vec[2] - vec[2]*adj_vec[0];
            angle = atan2(determinant, dot_product);

            angle = angle*180/3.145;
            cout << "ANGLE " << angle << endl;
            if ((int) angle == 180 || (int) angle == 179)
                angle = -180;
            angle = 180 - angle;
            cout << "ANGLE: " << angle << " " << stack[stack.size()-1] << " " << v << " " << adj[v][i][0] << endl;
            cout << "2nd line " << nodes[stack[stack.size()-1]][2] << " " << nodes[v][2] << " " << nodes[adj[v][i][0]][2] << endl;

            if (angle < smallest_angle) {
                smallest_angle = angle;
                smallest = i;
            }
        }
    }
    if (smallest >= 0) {
        cout << "Smallest: " << adj[v][smallest][0] << endl;
        check = adj[v][smallest][0];
        visited_edges[v][smallest] = true;
        if (check == found[1]) {
            cout << "FOUND" << endl;
            object_nodes={v};
            for (int j=stack.size()-1; j>=0; j--) {
                object_nodes.push_back(stack[j]);
                if (stack[j] == found[1])
                    break;
            }
            for (int j=0; j!=object_nodes.size(); j++)
                cout << "OBJECT_NODES " << object_nodes[j];

            objects.push_back(object_nodes);
        }
        stack.push_back(v);
        DFS_test(check, visited_edges, found);

    } else {
        cout << stack.size() << endl;
        int previous = -1;
        if (stack.size() == 0)
            return;
        if (stack.size() == 1) {
            for (int i = 0; i != adj[stack[0]].size(); ++i) {
                if (!visited_edges[stack[0]][i]) {
                    previous = adj[stack[0]][i][0];

                }

            }
            if (previous == -1)
                return;

        } else {
            previous = stack[stack.size()-1];
            cout << "OK " << previous << endl;
            cout << "CHANGE FOUND" << endl;
        }
        found[1] = previous;
        stack.pop_back();
        DFS_test(previous, visited_edges, found);
    }
}

void MainWidget::radialStreets() {
    int size = 3;
    int start = -40;
    float increment = 10;

    for (float i = start; i < (size*increment + start); i += increment) {
        for (float j = start; j < (size*increment + start); j += increment) {
            cout << j << endl;
            nodes.push_back({i,0,j});
        }
    }

    vector<vector<vector<int>>> test(nodes.size());
    adj = test;
    int adj_size = adj.size();
    cout << "% " << 99%10 << endl;
    for (int i = 0; i < adj_size; i++) {
        //cout << "Done1" << endl;
        //cout << i << endl;
        if ((i+1) % size != 0) {
            vector<float> x = nodes[i];
            vector<float> y= nodes[i+1];

            vector<float> line = {nodes[i+1][0] - nodes[i][0], nodes[i+1][2] - nodes[i][2]};
            float length = sqrt(pow(line[0],2) + pow(line[1],2));
            vector<float> normal = {-line[1]/length, line[0]/length};
            vector<float> first = {static_cast<float>((0.7*x[0] + 0.3*y[0]) + normal[0]), 0, static_cast<float>((0.7*x[2] + 0.3*y[2]) + normal[1])};
            vector<float> second = {static_cast<float>((0.3*x[0] + 0.7*y[0]) + normal[0]), 0, static_cast<float>((0.3*x[2] + 0.7*y[2]) + normal[1])};

            support_nodes.push_back(first);
            support_nodes.push_back(second);
            int sp_size = support_nodes.size();
            addEdge(i, {sp_size-2, sp_size-1, i+1});
        }
        if (static_cast<int>((i+size) / size) != size) {
            addEdge(i, {i+size});
        }
       // cout << "Done2" << endl;

    }
    cout << "Done" << endl;
}

void MainWidget::gridStreets(int start, int size, int increment) {
    for (float i = start; i < (size*increment + start); i += increment) {
        for (float j = start; j < (size*increment + start); j += increment) {
            addNode({i,0,j});
        }
    }

    initAdj();
    initOffset();
    int adj_size = adj.size();
    vector<vector<float>> offset;
    for (int i = 0; i < adj_size; i++) {
        if ((i+1) % size != 0) {
            addEdge(i, {i+1});
            offset = calcOffsetLine(nodes[i], nodes[i+1]);
            addOffset({i, i+1}, offset);
        }
        if (static_cast<int>((i+size) / size) != size) {
            addEdge(i, {i+size});
            offset = calcOffsetLine(nodes[i], nodes[i+size]);
            addOffset({i, i+size}, offset);
        }
    }
}

void MainWidget::cameraRotateHor(int value) { horVal = value; update(); }

void MainWidget::cameraRotateVer(int value) { verVal = value; update(); }

void MainWidget::initAdj() { vector<vector<vector<int>>> test(nodes.size()); adj = test; }

void MainWidget::initOffset() {
    vector<vector<vector<vector<float>>>> test(adj.size());
    offsetLines = test;
    for (int i=0; i<adj.size(); i++) {
        for (int j=0; j<adj.size(); j++) {
            offsetLines[i].push_back({});
        }
    }
}

void MainWidget::addEdge(int u, vector<int> v) {
    // FIXXXXXX for curves
    adj[u].push_back(v);
    adj[v[0]].push_back({u});
}

void MainWidget::addNode(vector<float> location) {
    if (location.size() == 3)
        nodes.push_back(location);
    else
        cout << "NODE LOCATION SIZE IS NOT 3" << endl;
}

void MainWidget::addOffset(vector<int> edge_index, vector<vector<float>> offset) {
    // offset is {s_up, s_down, e_up, e_down}
    cout << "ADD OFFSET" << endl;
    offsetLines[edge_index[0]][edge_index[1]] = offset;
    offsetLines[edge_index[1]][edge_index[0]] = {offset[3], offset[2], offset[1], offset[0]};
    cout << "Offset done" << endl;
}

void MainWidget::OffsetLine(vector<float> s, vector<float> e) {
    float line_length, t = 0, step_size;
    vector<float> line_vector, move, up, down;

    line_vector = {abs(s[0] - e[0]), abs(s[1] - e[1]), abs(s[2] - e[2])};
    line_length = sqrt(pow(line_vector[0],2) + pow(line_vector[1],2) + pow(line_vector[2],2));
    step_size = 1 / (line_length * 10);

    vector<float> normal = {-line_vector[2]/line_length, line_vector[1]/line_length, line_vector[0]/line_length};
    vector<float> rev_normal = {line_vector[2]/line_length, line_vector[1]/line_length, -line_vector[0]/line_length};

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        move = {(1-t)*s[0] + t*e[0], (1-t)*s[1] + t*e[1], (1-t)*s[2] + t*e[2]};
        up = {move[0] + normal[0], move[1] + normal[1], move[2] + normal[2]};
        down = {move[0] + rev_normal[0], move[1] + rev_normal[1], move[2] + rev_normal[2]};
        t += step_size;

        glVertex3f(up[0], up[1], up[2]);
        glVertex3f(down[0], down[1], down[2]);
    }
    glEnd();
}

void MainWidget::DrawCurve(vector<float> p0, vector<float> p1, vector<float> p2, vector<float> p3) {
    float step_size, t, line_length;
    vector<float> a, b, c, d, e, f, up_f, down_f, dir, normal, rev_normal;
    vector<float> triangleVertices;

    if (p0.size() != 2) {
        p0 = {p0[0], p0[2]};
        p1 = {p1[0], p1[2]};
        p2 = {p2[0], p2[2]};
        p3 = {p3[0], p3[2]};
    }

    t = 0;
    step_size = 0.01;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        a = Lerp(p0,p1,t);
        b = Lerp(p1,p2,t);
        c = Lerp(p2,p3,t);
        d = Lerp(a,b,t);
        e = Lerp(b,c,t);
        f = Lerp(d,e,t);

        dir = {e[0] - d[0], e[1] - d[1]};
        line_length = sqrt(pow(dir[0],2) + pow(dir[1],2));
        normal = {-dir[1] / line_length, dir[0] / line_length};
        rev_normal = {dir[1] / line_length, -dir[0] / line_length};

        up_f = {f[0] + normal[0], f[1] + normal[1]};
        down_f = {f[0] + rev_normal[0], f[1] + rev_normal[1]};

        t += step_size;
        glVertex3f(up_f[0], 0, up_f[1]);
        glVertex3f(down_f[0], 0, down_f[1]);
    }
    glEnd();
}


vector<float> MainWidget::Lerp(vector<float> p0, vector<float> p1, float t) {
    float move_x = (1-t)*p0[0] + t*p1[0];
    float move_y = (1-t)*p0[1] + t*p1[1];
    return {move_x, move_y};
}

void MainWidget::resizeGL(int w, int h) { // resizeGL()
    int offset;
    if (w > h) {
        offset = (w - h)/2;
        glViewport(offset, 0, h, h);
    } else {
        offset = (h-w)/2;
        glViewport(0, offset, w, w);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);

//    GLfloat light_pos[] = {0., 0., 10., 1.};
//    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
//    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,180.);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -50.0, 50.0);
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

//    glPushMatrix();
//    glTranslatef(0,50,2);
//    GLfloat light_pos[] = {0., 0., 0., 1.};
//    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
//    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,180.);
//    glPopMatrix();



    for (int i=0; i < adj.size(); i++) {
       for (int j=0; j < adj[i].size(); j++) {
            //cout << i << " " << j << " " << adj[i][j][0] << endl;
            //cout << nodes[i][0] << " " << nodes[adj[i][j][0]][0] << endl;
            glPushMatrix();
            if (adj[i][j].size() == 1) {
                OffsetLine(nodes[i], nodes[adj[i][j][0]]);
            } else {
                cout << "ELSE" << endl;
                cout << nodes[i][0] << support_nodes[adj[i][j][0]][0] << support_nodes[adj[i][j][1]][0] << nodes[adj[i][j][2]][0] << endl;
                cout << nodes[i][1] << support_nodes[adj[i][j][0]][1] << support_nodes[adj[i][j][1]][1] << nodes[adj[i][j][2]][1] << endl;
                cout << nodes[i][2] << support_nodes[adj[i][j][0]][2] << support_nodes[adj[i][j][1]][2] << nodes[adj[i][j][2]][2] << endl;
                DrawCurve(nodes[i], support_nodes[adj[i][j][0]], support_nodes[adj[i][j][1]], nodes[adj[i][j][2]]);
            }

            //cout << "FINISHED" << endl;
            glPopMatrix();
        }
    }

    glPushMatrix();
    drawBlocks();
    glPopMatrix();

    glPushMatrix();
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(1,0,0);
    glVertex3f(1, 0, -9);
    glColor3f(1,1,1);
    glEnd();
    glPopMatrix();


//    glPushMatrix();
//    glTranslatef(0,0,-20);
//    glScalef(35,1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(-20,0,0);
//    glRotatef(90,0,1,0);
//    glScalef(25,1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(20,0,0);
//    glRotatef(-45,0,1,0);
//    glScalef(30,1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(0,0,-30);
//    glRotatef(90,0,1,0);
//    glScalef(10,1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(-10,0,-30);
//    glRotatef(45,0,1,0);
//    glScalef(10*sqrt(2),1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(40,0,-10);
//    glRotatef(90, 0, 1, 0);
//    DrawCurve({-10,-10}, {-10,10}, {10,10}, {10,-10});
//    glPopMatrix();





//    glPushMatrix();
//    DrawCurve({-50,-50}, {-51,-47}, {-51,-43}, {-50,-40});
//    glPopMatrix();

//    glPushMatrix();
//    DrawCurve({-40,-50}, {-41,-47}, {-41,-43}, {-40,-40});
//    glPopMatrix();

//    glPushMatrix();
//    DrawCurve({-50,-50}, {-51,-47}, {-51,-43}, {-50,-40});
//    glPopMatrix();

//    glPushMatrix();
//    DrawCurve({-40,-50}, {-41,-47}, {-41,-43}, {-40,-40});
//    glPopMatrix();






//    glPushMatrix();
//    glTranslatef(-45,0,10);
//    glRotatef(-90,0,1,0);
//    DrawCurve({-10,-10}, {-10,10}, {10,10}, {10,-25});
//    glPopMatrix();


//    glPushMatrix();
//    glScalef(35,1,1);
//    OffsetLine({-1,0,0}, {1,0,0});
//    glPopMatrix();


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(horVal,1,0,0);
    glRotatef(verVal,0,0,1);

    gluLookAt(0,0,1, 0.0,0.0,0.0, 0.0,1.0,0.0);
    glFlush();
}
