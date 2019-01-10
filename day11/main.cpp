/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 11., 8:07
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include <tuple>
#include <limits>
#include <cassert>

using namespace std;

constexpr int size = 300;
typedef array<array<int, size + 1 >, size + 1 > Rack;
typedef tuple<int, int, int> SolutionT;

int getLevel(int x, int y, int serial) {
    int rack_id = x + 10;
    int level = rack_id * y;
    level += serial;
    level *= rack_id;
    if (level < 100)
        level = 0;
    else
        level = (level / 100) % 10;
    level -= 5;
    return level;
}

Rack createRack(const int serial_no) {
    Rack rack;
    for (auto &r : rack)
        r.fill(0);
    for (int x = 1; x <= size; ++x) {
        for (int y = 1; y <= size; ++y) {
            rack[x][y] = getLevel(x, y, serial_no);
        }
    }
    return rack;
}

Rack createSummedRack(const Rack &rack) {
    Rack summed;
    for (auto &r : summed)
        r.fill(0);
    for (int x = 1; x <= size; ++x) {
        for (int y = 1; y <= size; ++y) {
            summed[x][y] = rack[x][y] + summed[x - 1][y] + summed[x][y - 1] - summed[x - 1][y - 1];
        }
    }
    return summed;
}

/**
 * 
 * @param s Grid size
 * @param summed Summed-area table.
 * @return 
 */
SolutionT getMaxGridLevel(const int s, const Rack& summed) {
    int max_level = numeric_limits<int>::min();
    int maxx = 0, maxy = 0;
    for (int x = s; x <= size; ++x) {
        for (int y = s; y <= size; ++y) {
            int sum = summed[x][y] - summed[x - s][y] - summed[x][y - s] + summed[x - s][y - s];
            if (max_level < sum) {
                max_level = sum;
                maxx = x - s + 1;
                maxy = y - s + 1;
            }
        }
    }
    return make_tuple(maxx, maxy, max_level);
}

SolutionT getSolution2(const Rack& summed) {
    int max_x=0, max_y=0, max_size = 0;
    int max_level = numeric_limits<int>::min();
    for (int s = 1; s <= size; ++s) {
        int x, y, level;
        tie(x, y, level) = getMaxGridLevel(s, summed);
        //cout << "Grid size : " << s << " max level coord: " << x << "," << y << " level : " << level << endl;
        if ( max_level < level) {
            max_level = level;
            max_x=x;
            max_y=y;
            max_size = s;
        }
    }
    return make_tuple(max_x, max_y, max_size);
}

int main(int argc, char** argv) {

    assert(getLevel(3, 5, 8) == 4);
    assert(getLevel(122, 79, 57) == -5);
    assert(getLevel(217, 196, 39) == 0);
    assert(getLevel(101, 153, 71) == 4);

    //Test data
    {
        Rack rack = createRack(18);
        Rack summed = createSummedRack(rack);
        //33,45,29
        SolutionT t = getMaxGridLevel(3, summed);
        assert(t == make_tuple(33, 45, 29));
    }

    Rack rack = createRack(1309);
    Rack summed = createSummedRack(rack);
    SolutionT t = getMaxGridLevel(3, summed);
    cout << "Solution1 : " << get<0>(t) << "," << get<1>(t) << " level : " << get<2>(t) << endl;

    SolutionT t2 = getSolution2(summed);
    cout << "Solution2 : " << get<0>(t2) << "," << get<1>(t2) << " grid size : " << get<2>(t2) << endl;
    return 0;
}

