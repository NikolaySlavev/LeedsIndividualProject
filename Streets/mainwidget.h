#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QObject>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include "RGBVal.h"

using namespace std;

class MainWidget: public QGLWidget
    { Q_OBJECT//

    public slots:
        void cameraRotateHor(int);
        void cameraRotateVer(int);

    public:
        MainWidget(QWidget *parent);
        void DrawLine(float s_x, float s_y, float s_z, float e_x, float e_y, float e_z);
        void OffsetLine(vector<float> s, vector<float> e);
        void DrawCurve(vector<float> p0, vector<float> p1, vector<float> p2, vector<float> p3);
        vector<float> Lerp(vector<float> p0, vector<float> p1, float t);
        void addEdge(int u, vector<int> v);
        void gridStreets(int start, int size, int increment);
        void radialStreets();
        void findBlocks();
        void DFS(int v, bool visited[]);
        void drawBlocks();
        void removeBlocks();
        void findBlocks_test();
        void DFS_test(int v, vector<vector<bool>> visited_edges);


    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

    private:
        int horVal = 0;
        int verVal = 0;
        vector<vector<vector<int>>> adj;
        vector<vector<float>> nodes = {};
        vector<vector<float>> support_nodes = {};
        vector<int> stack = {};
        vector<int> object_nodes = {};
        vector<vector<int>> objects = {};
        vector<vector<float>> edges = {};
        int count = 0;





}; // class


#endif // MAINWIDGET_H
