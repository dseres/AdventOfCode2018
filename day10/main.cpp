/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 10, 2018, 5:23 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <regex>

using namespace std;

string test =
        R"(position=< 9,  1> velocity=< 0,  2>
position=< 7,  0> velocity=<-1,  0>
position=< 3, -2> velocity=<-1,  1>
position=< 6, 10> velocity=<-2, -1>
position=< 2, -4> velocity=< 2,  2>
position=<-6, 10> velocity=< 2, -2>
position=< 1,  8> velocity=< 1, -1>
position=< 1,  7> velocity=< 1,  0>
position=<-3, 11> velocity=< 1, -2>
position=< 7,  6> velocity=<-1, -1>
position=<-2,  3> velocity=< 1,  0>
position=<-4,  3> velocity=< 2,  0>
position=<10, -3> velocity=<-1,  1>
position=< 5, 11> velocity=< 1, -2>
position=< 4,  7> velocity=< 0, -1>
position=< 8, -2> velocity=< 0,  1>
position=<15,  0> velocity=<-2,  0>
position=< 1,  6> velocity=< 1,  0>
position=< 8,  9> velocity=< 0, -1>
position=< 3,  3> velocity=<-1,  1>
position=< 0,  5> velocity=< 0, -1>
position=<-2,  2> velocity=< 2,  0>
position=< 5, -2> velocity=< 1,  2>
position=< 1,  4> velocity=< 2,  1>
position=<-2,  7> velocity=< 2, -2>
position=< 3,  6> velocity=<-1, -1>
position=< 5,  0> velocity=< 1,  0>
position=<-6,  0> velocity=< 2,  0>
position=< 5,  9> velocity=< 1, -2>
position=<14,  7> velocity=<-2,  0>
position=<-3,  6> velocity=< 2, -1>
)";

struct Light {
    int x;
    int y;
    int vx;
    int vy;

    void move() {
        x += vx;
        y += vy;
    }
    void move_back() {
        x -= vx;
        y -= vy;
    }
};

typedef vector<Light> LightsT;

struct Map {
    LightsT lights;

    void readFile(istream &ifile) {
        //position=< 50200,  10144> velocity=<-5, -1>
        while (ifile) {
            int x, y, vx, vy;
            char c;
            while (ifile >> c && c != '<');
            if ( ifile.eof())
                break;
            ifile >> x >> c>>y>>c;
            while (ifile >> c && c != '<');
            ifile >> vx >> c>>vy>>c;
            lights.push_back({x, y, vx, vy});
        }
        std::cout<<"Count of lights : "<<lights.size()<<endl;
    }

    long long area(){
        auto px = minmax_element(lights.begin(), lights.end(), [](auto l, auto m) {
            return l.x < m.x;
        });
        auto py = minmax_element(lights.begin(), lights.end(), [](auto l, auto m) {
            return l.y < m.y;
        });
        int minx = px.first->x, maxx = px.second->x, miny = py.first->y, maxy = py.second->y;
        int sizex=maxx - minx + 1, sizey=maxy - miny + 1;
        return (long long)sizex*(long long)sizey;
    }
    
    void display() {
        auto px = minmax_element(lights.begin(), lights.end(), [](auto l, auto m) {
            return l.x < m.x;
        });
        auto py = minmax_element(lights.begin(), lights.end(), [](auto l, auto m) {
            return l.y < m.y;
        });
        int minx = px.first->x, maxx = px.second->x, miny = py.first->y, maxy = py.second->y;
        typedef vector<char> Row;
        typedef vector<Row> Display;
        int sizex=maxx - minx + 1, sizey=maxy - miny + 1;
        Display points(sizey, Row(sizex, '.'));
        for (auto l : lights) {
            points[l.y - miny ][l.x - minx] = '#';
            std::cout<<"{"<<l.x<<", "<<l.y<<", "<<l.vx<<", "<<l.vy<<"}"<<" ";
        }
        cout<<endl;
        for (auto v : points) {
            for (auto i : v) {
                cout << i;
            }
            cout << endl;
        }
    }
    
    void move(){
        for(Light &l: lights)
            l.move();
    }
    void move_back(){
        for(Light &l: lights)
            l.move_back();
    }
    
    void iterate(){
        int secs=0;
        while(true){
            long long before=area();
//            display();
//            char c;
//            cin >> c;
            move();
            ++secs;
            long long after = area();
            std::cout<<after<<" ";
            if ( after > before)
            {
                move_back();
                --secs;
                cout<<"Seconds : "<<secs<<endl;
                display();
                break;
            }
        }
                
    }
};

/*
 * 
 */
int main(int argc, char** argv) {
//    istringstream ss(test);
//    Map m;
//    m.readFile(ss);
//    m.iterate();
    ifstream ifile("input.txt");
    Map m2;
    m2.readFile(ifile);
    m2.iterate();
    return 0;
}
