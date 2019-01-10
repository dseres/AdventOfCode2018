/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on January 7, 2019, 10:00 AM
 */

#include <bits/stdc++.h>
using namespace std;

struct Point {
    int x, y, z, t;

    int distance(const Point &other) const {
        return abs(x - other.x) + abs(y - other.y) + abs(z - other.z) + abs(t - other.t);
    }

    bool inConsellation(const Point &other) const {
        return distance(other) <= 3;
    }

    friend istream &operator>>(istream &strm, Point &p) {
        char delim;
        return strm >> p.x >> delim >> p.y >> delim >> p.z >> delim >> p.t;
    }
};

auto readInput(istream &strm) {
    vector<Point> points;
    for (Point p; strm >> p;) {
        points.push_back(p);
    }
    return points;
}

auto readInput(string input) {
    istringstream is(input);
    return readInput(is);
}

bool inConstellation(const Point &p, const vector<Point> &c) {
    for (auto q : c) {
        if (p.inConsellation(q))
            return true;
    }
    return false;
}

void findConstellationForPoint(list<vector<Point>>& cs, auto p){
    bool found = false;
    auto found_it = cs.end();
    for (auto it = cs.begin(); it != cs.end(); ++it) {
        if (inConstellation(p, *it)) {
            if (!found){
                it->push_back(p);
                found = true;
                found_it = it;
            }
            else {
                //Other constellation
                vector<Point> &to = *found_it;
                vector<Point> &from = *it;
                to.insert(to.end(), from.begin(), from.end());
                it = cs.erase(it);
                --it;
            }
        }
    }
    if (!found) {
        //no constellation
        vector<Point> c = {p};
        cs.push_back(c);
    }
}

list<vector<Point>> getConstellations(const vector<Point> &points) {
    list<vector < Point>> cs;
    for (auto p : points) {
        findConstellationForPoint(cs, p);
    }
    return cs;
}

int countConstellactions(const vector<Point> &points) {

    return getConstellations(points).size();
}

void unitTest1() {

    string input = R"( 0,0,0,0
 3,0,0,0
 0,3,0,0
 0,0,3,0
 0,0,0,3
 0,0,0,6
 9,0,0,0
12,0,0,0)";
    auto points = readInput(input);
    assert(2 == countConstellactions(points));
}

void unitTest2() {

    string input = R"(-1,2,2,0
0,0,2,-2
0,0,0,-2
-1,2,0,0
-2,-2,-2,2
3,0,2,-1
-1,3,2,2
-1,0,-1,0
0,2,1,-2
3,0,0,0)";
    auto points = readInput(input);
    assert(4 == countConstellactions(points));
}

void unitTest3() {

    string input = R"(1,-1,0,1
2,0,-1,0
3,2,-1,0
0,0,3,1
0,0,-1,-1
2,3,-2,0
-2,2,0,0
2,-2,0,-1
1,-1,0,-1
3,2,0,2)";
    auto points = readInput(input);
    assert(3 == countConstellactions(points));
}

void unitTest4() {

    string input = R"(1,-1,-1,-2
-2,-2,0,1
0,2,1,3
-2,3,-2,1
0,2,3,-2
-1,-1,1,-2
0,-2,-1,0
-2,2,3,-1
1,2,2,0
-1,-2,0,-2)";
    auto points = readInput(input);
    assert(8 == countConstellactions(points));
}

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest1();
    unitTest2();
    unitTest3();
    unitTest4();

    ifstream input("input.txt");
    auto points = readInput(input);
    cout << "Solution1: " << countConstellactions(points) << endl;
    return 0;
}

