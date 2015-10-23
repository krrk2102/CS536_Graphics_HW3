//
//  main.cpp
//  CG_hw3
//
//  Created by Shangqi Wu on 14/10/27.
//  Copyright (c) 2014 Shangqi Wu. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// Define global variant----------------------------------------------------------------------------
int a=275, b=81, c=550, d=502, m=0, n=0, r=-37, j=123, k=217, o=373, p=467; // These vars are used to be attributes of the image and window.
float s = 1;
const double pi = acos((double)-1);
string input = "./hw3_split.ps", output = "./out.xpm";
vector<vector<char> > xpmpix; // This vector represents status of all pixels in the world window.

// Define all basic functions------------------------------------------------------------------------
void help();
int rnd(float arg);
void optana (int argc, char * const argv[]);
string setheader();
string setend();
void outfile(string output);

// Define classes needed-----------------------------------------------------------------------------
class Point {
private:
    int x;
    int y;
public:
    void set(int argx, int argy) {
        x = argx;
        y = argy;
    }
    int getx() {
        return x;
    }
    int gety() {
        return y;
    }
    void trans(int m, int n) { // Tanslation
        int ptold[3] = {x, y, 1};
        int matrix[3][3] = {1, 0, m, 0, 1, n, 0, 0, 1};
        int pt[3] = {0, 0, 0};
        for (int i = 0; i < 3; i++) {
            pt[i] = pt[i] + matrix[i][0] * ptold[0];
            pt[i] = pt[i] + matrix[i][1] * ptold[1];
            pt[i] = pt[i] + matrix[i][2] * ptold[2];
        }
        x=pt[0]; y=pt[1];
    }
    void scale(float sx, float sy) { // Scaling
        float ptold[3] = {(float)x, (float)y, 1};
        float matrix[3][3] = {sx, 0, 0, 0, sy, 0, 0, 0, 1};
        float pt[3] = {0, 0, 0};
        for (int i = 0; i <3  ; i++) {
            pt[i] += matrix[i][0] * ptold[0];
            pt[i] += matrix[i][1] * ptold[1];
            pt[i] += matrix[i][2] * ptold[2];
        }
        x=rnd(pt[0]); y=rnd(pt[1]);
    }
    void rot(int r) { // Rotation
        double ang = (double)r * pi / 180.0;
        float ptold[3] = {(float)x, (float)y, (float)1};
        float pt[3] = {0, 0, 0};
        float matrix[3][3] = {(float)cos(ang), (float)-sin(ang), 0, (float)sin(ang), (float)cos(ang), 0, 0, 0, 1};
        for (int i = 0; i < 3; i++) {
            pt[i] += matrix[i][0] * ptold[0];
            pt[i] += matrix[i][1] * ptold[1];
            pt[i] += matrix[i][2] * ptold[2];
        }
        x=rnd(pt[0]); y=rnd(pt[1]);
    }
    bool operator==(Point a){
        return (x==a.getx() && y==a.gety());
    }
    bool operator!=(Point a){
        return (x!=a.getx() || y!=a.gety());
    }
};

class Line { // present the line by: y = slope*x + d
private:
    Point start;
    Point end;
    int xmax;
    int xmin;
    int ymax;
    int ymin;
    bool sd_exist;
    float slope;
    float d;
    void cal() {
        if (start.getx() != end.getx()) {
            if (start.getx()>end.getx()) {
                xmax = start.getx();
                xmin = end.getx();
            }else {
                xmax = end.getx();
                xmin = start.getx();
            }
            if (start.gety()>=end.gety()) { // set ymax and ymin
                ymax = start.gety();
                ymin = end.gety();
            }else {
                ymax = end.gety();
                ymin = start.gety();
            }
            slope = (float)(start.gety() - end.gety()) / (float)(start.getx() - end.getx());
            d = (float)start.gety() - (slope * (float)start.getx());
            sd_exist = true;
        }else {
            if (start.gety()>=end.gety()) {
                ymax = start.gety();
                ymin = end.gety();
            }else {
                ymax = end.gety();
                ymin = start.gety();
            }
            xmax = start.getx();
            xmin = end.getx();
            slope = numeric_limits<float>::max();
            d = numeric_limits<float>::max();
            sd_exist = false;
        }
    }
public:
    void setall(int argx1, int argy1, int argx2, int argy2) {
        start.set(argx1, argy1);
        end.set(argx2, argy2);
        cal();
    }
    void setp(Point a, Point b) {
        start = a;
        end = b;
        cal();
    }
    bool sd() {
        return sd_exist;
    }
    Point gets() {
        return start;
    }
    Point gete() {
        return end;
    }
    Point cal_inter_wlr(float x) { // return the intersaction point of x=x
        Point tmp;
        if (sd_exist) {
            float y = slope*x +d;
            tmp.set(rnd(x), rnd(y));
        }else {
            tmp.set(numeric_limits<int>::max(), numeric_limits<int>::max());
        }
        return tmp;
    }
    Point cal_inter_wtb(float y) { // return the intersaction point of y=y
        Point tmp;
        if (sd_exist) {
            float x = (y - d)/ slope;
            tmp.set(rnd(x), rnd(y));
        }else {
            tmp.set(start.getx(), (int)y);
        }
        return tmp;
    }
    float getslope() {
        return slope;
    }
    int getxmin() {
        return xmin;
    }
    int getxmax() {
        return xmax;
    }
    int getymin() {
        return ymin;
    }
    int getymax() {
        return ymax;
    }
    void trans(int m,int n) { // Translation for both ends of the line.
        start.trans(m, n);
        end.trans(m, n);
        cal();
    }
    void scale(float sx, float sy) { // Scaling for both ends of the line.
        start.scale(sx,sy);
        end.scale(sx,sy);
        cal();
    }
    void rot(int r) { // Rotating for both ends of the line.
        start.rot(r);
        end.rot(r);
        cal();
    }
    vector<vector<int> > showall(vector<vector<int> > scanreg) { // Calculate all the points of the line.
        if (sd_exist == true) {
            // Fllowing codes are of Bresenham Algorithm, presented in L-02_Lines.pdf. Codes are odifiied for this cpp file.
            int dx, dy, D, x, y;
            dx = xmax - xmin;
            dy = ymax - ymin;
            if (0<slope && slope<1) {
                D = 2*dy - dx;
                y = ymin;
                for (x = xmin; x <= xmax; x++) {
                    xpmpix[y][x] = '+';
                    if (D <= 0) {
                        D += 2*dy;
                    }else {
                        if (y != ymax) { // Push x for scanline instead of calculating intersections.
                            scanreg[y].push_back(x);
                        }
                        D += 2*(dy - dx);
                        y++;
                    }
                }
            }else if (slope > 1) {
                D = 2*dx - dy;
                x = xmin;
                for (y = ymin; y <= ymax; y++) {
                    xpmpix[y][x] = '+';
                    if (y != ymax) {
                        scanreg[y].push_back(x);
                    }
                    if (D <= 0) {
                        D += 2*dx;
                    }else {
                        D += 2*(dx - dy);
                        x++;
                    }
                }
            }else if (-1<slope && slope<0) {
                D = 2*dy - dx;
                y = ymin;
                for (x = xmax; x >= xmin; x--) {
                    xpmpix[y][x] = '+';
                    if (D <= 0) {
                        D += 2*dy;
                    }else {
                        if (y != ymax) {
                            scanreg[y].push_back(x);
                        }
                        D += 2*(dy - dx);
                        y++;
                    }
                }
            }else if (slope == 1) {
                y = ymin;
                for (x = xmin; x <= xmax; x++) {
                    xpmpix[y][x]='+';
                    if (y != ymax) {
                        scanreg[y].push_back(x);
                    }
                    y++;
                }
            }else if (slope == -1) {
                y = ymax;
                for (x = xmin; x <= xmax; x++) {
                    xpmpix[y][x] = '+';
                    if (y != ymax) {
                        scanreg[y].push_back(x);
                    }
                    y--;
                }
            }else if (slope == 0) {
                y = ymin;
                for (x = xmin; x <= xmax; x++) {
                    xpmpix[y][x] = '+';
                    // The algorithm ignores horizontal lines.
                }
            }
            else { // i.e., slope<-1
                D = 2*dx - abs(dy);
                x = xmin;
                for (y = ymax; y >= ymin; y--) {
                    xpmpix[y][x] = '+';
                    if (y != ymax) {
                        scanreg[y].push_back(x);
                    }
                    if (D <= 0) {
                        D += 2*dx;
                    }else {
                        D += 2*(dx - dy);
                        x++;
                    }
                }
            }
        }else if (sd_exist == false) { // for vertical lines
            int x = xmin;
            for (int y = ymin; y <= ymax; y++) {
                xpmpix[y][x] = '+';
                if (y != ymax) {
                    scanreg[y].push_back(x);
                }
            }
        }
        return scanreg;
    }
};

class Polygon{ // A set of all vertices and edges of a polygon, with necessary function.
private:
    vector<Point> vertice;
    vector<Line> edge;
    vector<vector<int> > scanreg;
    bool out;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    void calp() {
        edge.resize(vertice.size()-1);
        xmin = vertice[0].getx();
        xmax = xmin;
        ymin = vertice[0].gety();
        ymax = ymin;
        for (int i = 0; i < vertice.size()-1; i++) {
            edge[i].setall(vertice[i].getx(), vertice[i].gety(), vertice[i+1].getx(), vertice[i+1].gety());
            if (vertice[i].getx() < xmin) {
                xmin = vertice[i].getx();
            }
            if (vertice[i].getx() > xmax) {
                xmax = vertice[i].getx();
            }
            if (vertice[i].gety() < ymin) {
                ymin = vertice[i].gety();
            }
            if (vertice[i].gety() > ymax) {
                ymax = vertice[i].gety();
            }
        }
        scanreg.resize(501);
    }
    void call() {
        vertice.resize(edge.size());
        xmax = edge[0].gets().getx();
        xmin = xmax;
        ymax = edge[0].gets().gety();
        ymin = ymax;
        for (int i = 0; i < edge.size(); i++) {
            vertice[i] = edge[i].gets();
            if (vertice[i].getx() < xmin) {
                xmin = vertice[i].getx();
            }
            if (vertice[i].getx() > xmax) {
                xmax = vertice[i].getx();
            }
            if (vertice[i].gety() < ymin) {
                ymin = vertice[i].gety();
            }
            if (vertice[i].gety() > ymax) {
                ymax = vertice[i].gety();
            }
        }
        vertice.push_back(vertice[0]);
        scanreg.resize(501);
    }
public:
    void setp(vector<Point> argp) { // input vertices of a polygon and then calculate its edges
        vertice.clear();
        edge.clear();
        if (argp.size() != 0) {
            if (argp[0] != argp[argp.size()-1]) {
                argp.push_back(argp[0]);
            } // If you choose to set up a polygon by points, the first point and the last must be the same.
            for (int i = 0; i < argp.size(); i++) {
                vertice.push_back(argp[i]);
            }
            calp();
            out = false;
        }else {
            out = true;
        }
    }
    void setl(vector<Line> argl) { // input lines to calculate vertices
        edge.clear();
        vertice.clear();
        if (argl.size() != 0) {
            if (argl[0].gets() != argl[argl.size()-1].gete()) {
                Line buff;
                buff.setp(argl[argl.size()-1].gete(), argl[0].gets());
                argl.push_back(buff);
            } // If you choose to set up a polygon by edges, starting point of the first edge and ending point of the last edge must be the same.
            edge = argl;
            call();
            out = false;
        }else {
            out = true;
        }
    }
    vector<Point> getparr() { // return all vertices
        return vertice;
    }
    vector<Line> getlarr() { // return all lines
        return edge;
    }
    Point getp(int i) { // return the ith point
        return vertice[i];
    }
    Line getl(int i) { // return the ith line
        return edge[i];
    }
    bool getout() {
        return out;
    }
    int getxmin() {
        return xmin;
    }
    int getxmax() {
        return xmax;
    }
    int getymin() {
        return ymin;
    }
    int getymax() {
        return ymax;
    }
    void trans(int m,int n) {
        for (int i = 0; i < vertice.size(); i++) {
            vertice[i].trans(m,n);
        }
        calp();
    }
    void scale(float sx, float sy) {
        for (int i = 0; i < vertice.size(); i++) {
            vertice[i].scale(sx,sy);
        }
        calp();
    }
    void rot(int r) {
        for (int i = 0; i < vertice.size(); i++) {
            vertice[i].rot(r);
        }
        calp();
    }
    void showlines() {
        for (int i = 0; i < edge.size(); i++) {
            scanreg = edge[i].showall(scanreg);
        }
    }
    void toviewport() {
        float sx = (float)(o - j)/(float)(c - a);
        float sy = (float)(p - k)/(float)(d - b);
        for (int i = 0; i < vertice.size(); i++) {
            vertice[i].trans(-a, -b);
            vertice[i].scale(sx, sy);
            vertice[i].trans(j, k);
        }
        calp();
    }
    void fillin() { // Fill in polygons by each scanline
        for (int i = ymin; i < ymax; i++) {
            int size = (int)scanreg[i].size();
            if (size>0 && (size%2==0)) {
                sort(scanreg[i].begin(), scanreg[i].end());
                for (int j = 0; j < scanreg[i].size(); j+=2) {
                    for (int k = scanreg[i][j]+1; k < scanreg[i][j+1]; k++) {
                        xpmpix[i][k] = '+';
                    }
                }
            }
        }
    }
};

//Define functions for Sutherland-Hodgman Algorithm to clip polygon-------------------------------
Polygon shclip(Polygon argp);
vector<Polygon> getfile(string input);

//Here is main function---------------------------------------------------------------------------
int main(int argc, char * argv[]) {
    // analyze all the input options
    optana(argc, argv);
    int width = 501; // height of the world window
    int height = 501; // width of the world window
    xpmpix.resize(height); // set vector as a 2-d array
    for (int i = 0; i < height; i++) { // Initailize all char to be '-', which stands for white pixel
        xpmpix[i].resize(width);
        for (int j = 0; j < width; j++) { // i.e., all pixels should be white at first
            xpmpix[i][j] = '-'; // stands for point(y,x)
        }
    }
    // read and buffer input *.ps file
    input = "/Users/wushangqi/hw3_split.ps"; // dir only for debug*********************************
    vector<Polygon> vecpoly;
    vecpoly = getfile(input);
    // Do transformations before clip a line.
    int poly = (int)vecpoly.size();
    if (s != 1.0) { // Do the scaling first.
        for (int i = 0; i < poly; i++) {
            vecpoly[i].scale(s,s);
        }
    }
    if (r != 0) { // Do rotation second.
        for (int i = 0; i < poly; i++) {
            vecpoly[i].rot(r);
        }
    }
    if (m != 0 || n != 0) { // Do translation last.
        for (int i = 0; i < poly; i++) {
            vecpoly[i].trans(m, n);
        }
    }
    // Use Sutherland-Hodgman Algorithm to clip polygons
    for (int i = 0; i < poly; i++) {
        vecpoly[i] = shclip(vecpoly[i]);
    }
    for (int i = 0; i < poly; i++) {
        if (vecpoly[i].getout() == false) {
            vecpoly[i].toviewport();
        }
    }
    for (int i = 0; i < poly; i++) { // Write pixels for all lines in the world window.
        if (vecpoly[i].getout() == false) {
            vecpoly[i].showlines();
        }
    }
    for (int i = 0; i < poly; i++) {
        if (vecpoly[i].getout() == false) {
            vecpoly[i].fillin();
        }
    }
    // Prepare to wirte output file.
    output = "/Users/wushangqi/out.xpm"; // dir only for debug ******************************************
    outfile(output);
    string shell = "display " + output;
    system(shell.c_str());
    return 0;
}

//-------------------------------------------------------------------------------------
Polygon shclip(Polygon argp){ // Use Sutherland-Hodgman Algorithm to clip the polygon.
    // Clip edges of each polygon
    vector<Point> vecbuff;
    vector<Point> vecp = argp.getparr();
    Point buffp1, buffp2;
    Line buffl;
    int x1;
    int y1;
    int x2;
    int y2;
    // Clipping edge: x=a
    if (vecp.size() >= 2) { // Due to the for loop setting, we need to use a different method if vecp.size<2.
        if (a <= vecp[0].getx()) { // S-h Algorithm preprarition: If first point is inside, add it.
            vecbuff.push_back(argp.getp(0));
        }
        for (int i = 0; i < vecp.size()-1; i++) {
            buffp1 = vecp[i];
            buffp2 = vecp[i+1];
            x1 = buffp1.getx();
            x2 = buffp2.getx();
            if (a<=x1 && a<=x2) { // S-h Algorithm case 1
                vecbuff.push_back(buffp2);
            }else if (a<=x1 && x2<a) { // S-h Algorithm case 2
                buffl.setp(buffp1, buffp2);
                int ya = buffl.cal_inter_wlr(a).gety();
                buffp2.set(a, ya);
                if (buffp1 != buffp2) { // This part is to avoid pushing consecutive same vertices into one polygon.
                    vecbuff.push_back(buffp2);
                }
            }else if (x1<a && x2>=a) { // S-h Algorithm case 4
                buffl.setp(buffp1, buffp2);
                int ya = buffl.cal_inter_wlr(a).gety();
                buffp1.set(a, ya);
                if (buffp1 != buffp2) { // Avoiding pushing consecutive same vertices into one polygon
                    vecbuff.push_back(buffp1);
                }
                vecbuff.push_back(buffp2);
            } // else: S-h Algorithm case 3, just ignore
        }
        vecp.clear();
        vecp = vecbuff;
        vecbuff.clear();
        if (vecp.back() != vecp[0]) {
            vecp.push_back(vecp[0]);
        }
    }else vecp.clear();
    // Clipping edge: y=b
    if (vecp.size() >= 2) { // Due to the for loop setting, we need to use a different method if vecp.size<2.
        if (b <= vecp[0].gety()) { // S-h Algorithm preprarition: If first point is inside, add it.
            vecbuff.push_back(vecp[0]);
        }
        for (int i = 0; i < vecp.size()-1; i++) {
            buffp1 = vecp[i];
            buffp2 = vecp[i+1];
            y1 = buffp1.gety();
            y2 = buffp2.gety();
            if (b<=y1 && b<=y2) { // S-h Algorithm case 1
                vecbuff.push_back(buffp2);
            }else if (b<=y1 && y2<b) { // S-h Algorithm case 2
                buffl.setp(buffp1, buffp2);
                int xb = buffl.cal_inter_wtb(b).getx();
                buffp2.set(xb, b);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp2);
                }
            }else if (y1<b && b<=y2) { // S-h Algorithm case 4
                buffl.setp(buffp1, buffp2);
                int xb = buffl.cal_inter_wtb(b).getx()  ;
                buffp1.set(xb, b);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp1);
                }
                vecbuff.push_back(buffp2);
            } // else: S-h Algorithm case 3, just ignore
        }
        vecp.clear();
        vecp = vecbuff;
        vecbuff.clear();
        if (vecp.back() != vecp[0]) {
            vecp.push_back(vecp[0]);
        }
    } else vecp.clear();
    // Clipping edge: x=c
    if (vecp.size() >= 2) { // Due to the for loop setting, we need to use a different method if vecp.size<2.
        if (vecp[0].getx() <= c) { // S-h Algorithm preprarition: If first point is inside, add it.
            vecbuff.push_back(vecp[0]);
        }
        for (int i = 0; i < vecp.size()-1; i++) {
            buffp1 = vecp[i];
            buffp2 = vecp[i+1];
            x1 = buffp1.getx();
            x2 = buffp2.getx();
            if (x1<=c && x2<=c) { // S-h Algorithm case 1
                vecbuff.push_back(buffp2);
            }else if (x1<=c && c<x2) { // S-h Algorithm case 2
                buffl.setp(buffp1, buffp2);
                int yc = buffl.cal_inter_wlr(c).gety();
                buffp2.set(c, yc);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp2);
                }
            }else if (c<x1 && x2<=c) { // S-h Algorithm case 4
                buffl.setp(buffp1, buffp2);
                int yc = buffl.cal_inter_wlr(c).gety();
                buffp1.set(c, yc);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp1);
                }
                vecbuff.push_back(buffp2);
            } // else: S-h Algorithm case 3, just ignore
        }
        vecp.clear();
        vecp = vecbuff;
        vecbuff.clear();
        if (vecp.back() != vecp[0]) {
            vecp.push_back(vecp[0]);
        }
    }else vecp.clear();
    // Clipping edge: y=d
    if (vecp.size() >= 2) {
        if (vecp[0].gety() <= d) { // S-h Algorithm preprarition: If first point is inside, add it.
            vecbuff.push_back(vecp[0]);
        }
        for (int i = 0; i < vecp.size()-1; i++) {
            buffp1 = vecp[i];
            buffp2 = vecp[i+1];
            y1 = buffp1.gety();
            y2 = buffp2.gety();
            if (y1<=d && y2<=d) { // S-h Algorithm case 1
                vecbuff.push_back(buffp2);
            }else if (y1<=d && d<y2) { // S-h Algorithm case 2
                buffl.setp(buffp1, buffp2);
                int xd = buffl.cal_inter_wtb(d).getx();
                buffp2.set(xd, d);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp2);
                }
            }else if (d<y1 && y2<=d) { // S-h Algorithm case 4
                buffl.setp(buffp1, buffp2);
                int xd = buffl.cal_inter_wtb(d).getx();
                buffp1.set(xd, d);
                if (buffp1 != buffp2) {
                    vecbuff.push_back(buffp1);
                }
                vecbuff.push_back(buffp2);
            } // else: S-h Algorithm case 3, just ignore
        }
        vecp.clear();
        vecp = vecbuff;
        vecbuff.clear();
        if (vecp.back() != vecp[0]) {
            vecp.push_back(vecp[0]);
        }
    }else vecp.clear();
    argp.setp(vecp);
    vecp.clear();
    return argp;
}

//-------------------------------------------------------------------------
vector<Polygon> getfile(string input){ // Interpreting ps file to polygons
    ifstream infile(input.c_str());
    if (!infile) {
        cout<<"File does not exist, please check your path."<<endl;
        abort();
    }
    string buff;
    bool store = false;
    int buff_pt[2];
    int buff_i = 0;
    vector<Polygon> vecpoly;
    vector<Point> vecpoint;
    while (infile) {
        infile>>buff;
        if (buff.compare("stroke") == 0) {
            Polygon poly_buff;
            poly_buff.setp(vecpoint);
            vecpoly.push_back(poly_buff);
            vecpoint.clear();
        }else if (buff.compare("%%%END") == 0) {
            store = false;
        }else if (buff.compare("%%%BEGIN") == 0) {
            store = true;
        }else if (store == true && (buff.compare("moveto")==0 || buff.compare("lineto")==0)) {
            if (buff_i == 2) {
                buff_i = 0;
            }else {
                cout<<"There must be 2 values for one vertice. Please check your input file."<<endl;
                abort();
            }
        }else {
            if (store == true) {
                buff_pt[buff_i] = atoi(buff.c_str());
                buff_i++;
                if (buff_i == 2) {
                    Point pt_buff;
                    pt_buff.set(buff_pt[0], buff_pt[1]);
                    vecpoint.push_back(pt_buff);
                }
            }
        }
    }
    infile.close();
    return vecpoly;
}

//--------------------------------------------------------------------
void outfile(string output){ // Write xpm file
    ofstream out(output.c_str());
    string line = "";
    if (!out) {
        cout<<"Cannot write an output file, please check the validity of output path."<<endl;
    }
    out<<setheader()<<endl;
    int height = 501;
    int width = 501;
    cout<<setheader()<<endl;
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j< width; j++) {
            line += xpmpix[i][j];
        }
        line = "\"" + line + "\"";
        if (i != 0) {
            line = line + ",";
        }
        out<<line<<endl;
        cout<<line<<endl;
        line.clear();
    }
    out<<setend()<<endl;
    cout<<setend()<<endl;
    out.close();
    
}

//----------------------------------------------------------------------------------------
void help(){
    cout<<"[-f] The next argument is the input \"Postscript\" file."<<endl;
    cout<<"[-s] This next argument is a float specifying the scaling factor in both dimensions about the world origin."<<endl;
    cout<<"[-r] This next argument is an integer specifying the number of degrees for a counter-clockwise rotation about the world origin."<<endl;
    cout<<"[-m] The next argument is an integer specifying a translation in the x dimension."<<endl;
    cout<<"[-n] The next argument is an integer specifying a translation in the y dimension."<<endl;
    cout<<"[-a] The next argument is an integer lower bound in the x dimension of the world window."<<endl;
    cout<<"[-b] The next argument is an integer lower bound in the y dimension of the world window."<<endl;
    cout<<"[-c] The next argument is an integer upper bound in the x dimension of the world window."<<endl;
    cout<<"[-d] The next argument is an integer upper bound in the y dimension of the world window."<<endl;
    cout<<"[-j] The next argument is an integer lower bound in the x dimension of the viewport window."<<endl;
    cout<<"[-k] The next argument is an integer lower bound in the y dimension of the viewport window."<<endl;
    cout<<"[-o] The next argument is an integer upper bound in the x dimension of the viewport window."<<endl;
    cout<<"[-p] The next argument is an integer upper bound in the y dimension of the viewport window."<<endl;
    cout<<"This program will generate ./out.xpm file automatically."<<endl;
}


//-----------------------------------------------------------------------------
int rnd(float arg){ //return a rounded vaule of a float
    if (arg > 0) {
        return (int)(arg + 0.5);
    }else {
        return (int)(arg - 0.5);
    }
}

//------------------------------------------------------------------------------
void optana(int argc, char * const argv[]){
    // analyze input option and set defualt options
    float temp;
    int opt;
    while ((opt = getopt(argc, argv, "f:a:b:c:d:s:m:n:r:j:k:o:p:h"))!= -1) {
        switch (opt) {
            case 'f':{
                input = optarg;
                break;
            }
            case 'a':{
                string astr(optarg);
                temp = atof(astr.c_str());
                a = rnd(temp);
                break;
            }
            case 'b':{
                string bstr(optarg);
                temp = atof(bstr.c_str());
                b = rnd(temp);
                break;
            }
            case 'c':{
                string cstr(optarg);
                temp = atof(cstr.c_str());
                c = rnd(temp);
                break;
            }
            case 'd':{
                string dstr(optarg);
                temp = atof(dstr.c_str());
                d = rnd(temp);
                break;
            }
            case 'm':{
                string mstr(optarg);
                temp = atof(mstr.c_str());
                m = rnd(temp);
                break;
            }
            case 'n':{
                string nstr(optarg);
                temp = atof(nstr.c_str());
                n = rnd(temp);
                break;
            }
            case 'r':{
                string rstr(optarg);
                temp = atof(rstr.c_str());
                r = rnd(temp);
                break;
            }
            case 's':{
                string sstr(optarg);
                s = atof(sstr.c_str());
                break;
            }
            case 'j':{
                string jstr(optarg);
                temp = atof(jstr.c_str());
                j = rnd(temp);
                break;
            }
            case 'k':{
                string kstr(optarg);
                temp = atof(kstr.c_str());
                k = rnd(temp);
                break;
            }
            case 'o':{
                string ostr(optarg);
                temp = atof(ostr.c_str());
                o = rnd(temp);
                break;
            }
            case 'p':{
                string pstr(optarg);
                temp = atof(pstr.c_str());
                p = rnd(temp);
                break;
            }
            case 'h':help();abort();
                break;
            default:cout<<"Your input is not correct, please enter -h for help."<<endl;abort();
                break;
        }
    }
    if (a > c) { // switch if a is bigger than c
        int tmp = c;
        c = a;
        a = tmp;
    }
    if (b > d) {
        int tmp = d;
        d = b;
        b = tmp;
    }
    if (a == c || b == d) {
        cout<<"Lower bound and upper bound of the world window cannot be the same."<<endl;
        abort();
    }
    if (j > o) {
        int tmp = j;
        j = o;
        o = tmp;
    }
    if (k > p) {
        int tmp = k;
        k = p;
        p = tmp;
    }
    if (j ==o || k ==p) {
        cout<<"Lower bound and upper bound of the viewport cannot be the same."<<endl;
        abort();
    }
    if (j<0 || k<0 || 500<o || 500<p) {
        cout<<"Viewport must within the picture."<<endl;
        abort();
    }
}


//-------------------------------------------------------------------------
string setheader() {
    stringstream tmp;
    int w = 501;
    int h = 501;
    tmp<<w;
    string intw;
    tmp>>intw;
    tmp.clear();
    tmp<<h;
    string inth;
    tmp>>inth;
    string str = "/* XPM */\nstatic char *CG_hw3[] = {\n/* width height num_colors chars_per_pixel */\n\"" + intw + " " + inth +" 2 1\",\n/* colors */\n\"- c #ffffff\",\n\"+ c #000000\",\n/* pixels */";
    return str;
}

//-------------------------------------------------------------------------
string setend() {
    string str = "};";
    return str;
}