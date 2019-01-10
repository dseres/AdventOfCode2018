/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2019. január 3., 20:25
 */

#include <bits/stdc++.h>
using namespace std;

struct Cube {
    int x, y, z, s;
    int intersections;
    int dist;

    Cube(int x, int y, int z, int s) : x(x), y(y), z(z), s(s) {
        intersections = 0;
        int distx = min(abs(x), abs(x + s));
        int disty = min(abs(y), abs(y + s));
        int distz = min(abs(z), abs(z + s));
        dist = distx + disty + distz;
    };

    Cube(const Cube &) = default;
    Cube(Cube &&) = default;
    Cube &operator=(const Cube &) = default;
    Cube &operator=(Cube &&) = default;

    array<Cube, 8> split() {
        int ns = s / 2;
        return array<Cube, 8>{
            Cube{x, y, z, ns},
            Cube{x, y, z + ns, ns},
            Cube{x, y + ns, z, ns},
            Cube{x, y + ns, z + ns, ns},
            Cube{x + ns, y, z, ns},
            Cube{x + ns, y, z + ns, ns},
            Cube{x + ns, y + ns, z, ns},
            Cube{x + ns, y + ns, z + ns, ns},};
    }

    friend bool operator<(const Cube &c, const Cube &d) {
        return make_pair(c.intersections, c.dist) < make_pair(d.intersections, d.dist);
    }
};

struct NanoBot {
    int x, y, z, r;

    int getDistance(const NanoBot &n) const {
        return abs(x - n.x) + abs(y - n.y) + abs(z - n.z);
    }

    bool inRadius(const NanoBot &n) const {
        return getDistance(n) <= r;
    }

    bool hasIntersection(const NanoBot &n) const {
        return getDistance(n) <= r + n.r;
    }

    bool hasIntersectionWithCube(const Cube &c) const {
        int dist = 0;
        if (x < c.x) dist += c.x - x;
        if (c.x+c.s < x) dist += x - (c.x+c.s);
        if (y < c.y) dist += c.y - y;
        if (c.y+c.s < y) dist += y - (c.y+c.s);
        if (z < c.z) dist += c.z - z;
        if (c.z+c.s < z) dist += z - (c.z+c.s);
        return dist <= r;
    }
};

auto readStream(istream &is) {
    vector<NanoBot> nanobots;
    for (string line; getline(is, line);) {
        static regex r(R"(^pos=<(-?\d+),(-?\d+),(-?\d+)>, r=(-?\d+)$)");
        smatch m;
        if (regex_match(line, m, r)) {
            int x = stoll(m[1]);
            int y = stoll(m[2]);
            int z = stoll(m[3]);
            int r = stoll(m[4]);
            nanobots.push_back(NanoBot{x, y, z, r});
        } else {
            cerr << "Bad data in input file." << endl;
            exit(-1);
        }
    }
    return nanobots;
}

int getSolution1(const vector<NanoBot> &nanobots) {
    NanoBot nb = *max_element(nanobots.begin(), nanobots.end(), [](const NanoBot &n1, const NanoBot & n2) {
        return n1.r < n2.r;
    });
    int in_radius = count_if(nanobots.begin(), nanobots.end(), [nb](const NanoBot & n) {
        return nb.inRadius(n); });
    return in_radius;
}

class Solver2 {
private:
    const vector<NanoBot> nanobots;
    Cube initial_cube;
    constexpr static bool DEBUG = false;
public:

    Solver2(const vector<NanoBot> nanobots) : nanobots(nanobots), initial_cube(computeInitialCube()) {
    };

    int getSolution2() {
        return solve();
    }


private:

    Cube computeInitialCube() {
        int max_d = 0;
        for (auto nb : nanobots) {
            max_d = max(max_d, abs(nb.x));
            max_d = max(max_d, abs(nb.y));
            max_d = max(max_d, abs(nb.z));
        }

        int cube_d = 1;
        while (cube_d < max_d) {
            cube_d *= 2;
        }
        if (DEBUG) {
            cout << "Longest ditance from origo: " << max_d << endl;
            cout << "Size of initial cube: " << cube_d *2  << endl;
        }
        return Cube(-cube_d, -cube_d, -cube_d, cube_d * 2);
    }

    int solve() {
        priority_queue<Cube> cubes;
        cubes.push(initial_cube);
        while (true) {
            Cube c = cubes.top();
            cubes.pop();
            if ( c.s == 1){
                return c.dist;
            }
            for(Cube nc: c.split()) {
                nc.intersections = getIntersections(nc);
                cubes.push(nc);
                if (DEBUG){
                    cout << +"Intersections of cube {"<<nc.x<<", "<<nc.y<<", "<<nc.z<<", "<<nc.s<<"}: " << nc.intersections << endl;
                }
            }
        }
        return 0;
    }

    int getIntersections(const Cube &c) {
        auto l = [c](const NanoBot & nb) {
            return nb.hasIntersectionWithCube(c);
        };
        return count_if(nanobots.begin(), nanobots.end(), l);
    }
};

/*
 * 
 */
int main(int argc, char** argv) {
    ifstream input("input.txt");
    auto nanobots = readStream(input);
    cout << "Solution1: " << getSolution1(nanobots) << endl;
    Solver2 solver(nanobots);
    cout << "Solution2: " << solver.getSolution2() << endl;
    return 0;
}

