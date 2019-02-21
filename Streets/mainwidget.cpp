#include <Windows.h>
#include <iostream>
#include <QtGui>
#include "MainWidget.h"
#include <algorithm>

using namespace std;

MainWidget::MainWidget(QWidget *parent):QGLWidget(parent) {
}

// called when OpenGL context is set up
void MainWidget::initializeGL() {
    glClearColor(0.3, 0.3, 0.3, 0.0);
    gridStreets(-20, 5, 10);
    //radialStreets();
    //findBlocks();
    findBlocks_test();
    cout << "SIZEEAFAFAF " << objects.size() << endl;
    removeBlocks();
    cout << "SIZEEAFAFAF " << objects.size() << endl;


}

void MainWidget::removeBlocks() {
//    for (int i=0; i < objects.size(); i++) {
//        for (int j=0; j < objects[i].size(); j++) {
//           cout << objects[i][j] << " | ";
//        }x
//        cout << endl;
//    }

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
            visited_edges[i].push_back({});
        }
    }
    stack = {v};
    DFS_test(v, visited_edges);
}

void MainWidget::DFS_test(int v, vector<vector<bool>> visited_edges) {
    count++;
    if (count % 1000 == 0)
        cout << count << " % ";

    int check;
    bool stop;
    for (int i = 0; i != adj[v].size(); ++i) {
        stop = false;
        if (adj[v][i].size() > 1)
            check = adj[v][i][-1];
        else
            check = adj[v][i][0];
        //cout << "CHECKKK: " << check << " " << v << endl;

        if (!visited_edges[v][i]) {
            //cout << "V" << check;
            int index = -1;
            for (int i=0; i!=stack.size(); i++) {
                //cout << " " << stack[i] << " ";
                if (check == stack[i]) {
                    index = i;
                    stop = true;
                    break;
                }
            }
            //cout << "INDEX " << index;
            if (index >= 0 && stack.size() - index > 2) {
                object_nodes={};
                for (int j=index; j<stack.size(); j++) {
                    object_nodes.push_back(stack[j]);
                }
//                for (int j=0; j!=object_nodes.size(); j++) {
//                    cout << "OBJECT_NODES " << object_nodes[j];
//                }
                objects.push_back(object_nodes);
            }
            //cout << endl;

            visited_edges[v][i] = true;
            if (!stop) {
                stack.push_back(check);
                DFS_test(check, visited_edges);
            }
        }
    }
    stack.pop_back();
}

void MainWidget::findBlocks() {
    // Mark all the vertices as not visited
    int v = 0;
    int V = nodes.size();
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;
    stack = {v};
    DFS(v, visited);
}

void MainWidget::DFS(int v, bool visited[]) {
    visited[v] = true;
    cout << v << " % ";

    int check;
    for (int i = 0; i != adj[v].size(); ++i) {
        if (adj[v][i].size() > 1)
            check = adj[v][i][-1];
        else
            check = adj[v][i][0];
        cout << "CHECKKK: " << check << " " << i << endl;

        if (!visited[check]) {
            stack.push_back(check);
            DFS(check, visited);
        } else {
            cout << "V" << check;
            int index = -1;
            for (int i=0; i!=stack.size(); i++) {
                cout << " " << stack[i] << " ";
                if (check == stack[i]) {
                    index = i;
                    //break;
                }
            }
            cout << "INDEX " << index;
            if (index >= 0 && stack.size() - index > 2) {
                object_nodes={};
                for (int j=index; j<stack.size(); j++) {
                    object_nodes.push_back(stack[j]);
                }
                for (int j=0; j!=object_nodes.size(); j++) {
                    cout << "OBJECT_NODES " << object_nodes[j];
                }
                objects.push_back(object_nodes);
            }
            cout << endl;
        }
    }

    //if (pop == true)
        stack.pop_back();
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
    vector<vector<vector<int>>> test(size*size);
    adj = test;

    float inc_i = start, inc_j = start;
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            nodes.push_back({inc_i,0,inc_j});
            if (j+1 < size) {
                addEdge(i*size+j, {i*size+j+1});

            }
            if (i+1 < size) {
                addEdge(i*size+j, {(i+1)*size+j});

            }
            //cout << "SSSSSSSS " << i << " " << j << inc_i << " " << inc_j << endl;
            inc_j += increment;
        }
        inc_j = start;
        inc_i += increment;
        cout << "ITERATION" << nodes.size() << adj.size() << endl;
    }
}

void MainWidget::cameraRotateHor(int value) { horVal = value; update(); }

void MainWidget::cameraRotateVer(int value) { verVal = value; update(); }

void MainWidget::addEdge(int u, vector<int> v) {
//    vector<vector<int>> test = {{}, {}, {}};
//    test[2].push_back(1);
//    cout << test[2][0] << "asddd" << endl;
    // FIXXXXXX
    adj[u].push_back(v);
    adj[v[0]].push_back({u});
}
void MainWidget::DrawLine(float s_x, float s_y, float s_z, float e_x, float e_y, float e_z) {
    float x, y, z, line_length, move_x, move_y, move_z, t, step_size;
    x = abs(s_x - e_x);
    y = abs(s_y - e_y);
    z = abs(s_z - e_z);

    line_length = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
    t = 0;
    step_size = 1 / (line_length * 10);

    glBegin(GL_POINTS);
    while ((1 - t) > -DBL_EPSILON) {
        move_x = (1-t)*s_x + t*e_x;
        move_y = (1-t)*s_y + t*e_y;
        move_z = (1-t)*s_z + t*e_z;
        t += step_size;
        glVertex3f(move_x, move_y, move_z);
    }
    glEnd();
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
    while ((1 - t) > -DBL_EPSILON) {
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
    while ((1 - t) > -DBL_EPSILON) {
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

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_pos[] = {0., 0., 10., 1.};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,180.);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -50.0, 50.0);
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(0,50,2);
    GLfloat light_pos[] = {0., 0., 0., 1.};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,180.);
    glPopMatrix();



    for (int i=0; i < adj.size(); i++) {
       for (int j=0; j < adj[i].size(); j++) {
            //cout << i << " " << j << " " << adj[i][j][0] << endl;
            //cout << nodes[i][0] << " " << nodes[adj[i][j][0]][0] << endl;
            glPushMatrix();
            if (adj[i][j].size() == 1) {
                cout << "IF" << endl;
                OffsetLine(nodes[i], nodes[adj[i][j][0]]);
            } else {
                cout << "ELSE" << endl;
                //vector<vector<float>> tmp = {nodes[i]};
                //tmp.insert( tmp.end(), support_nodes[adj[i][j]].begin(), support_nodes[adj[i][j]].end());
                //tmp.insert( tmp.end(), nodes[adj[i][j][-1]].begin(), nodes[adj[i][j][-1]].end());
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
