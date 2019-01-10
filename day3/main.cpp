/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 3, 2018, 11:19 AM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>

using namespace std;

const int FABRIC_SIZE = 1000;

struct Claim {
    int id;
    int left;
    int top;
    int width;
    int height;

    bool isValid() {
        return ( id >= 0 && left >= 0 && top >= 0 && width >= 0 && height >= 0)
                && (left + width <= FABRIC_SIZE)
                && (top + height <= FABRIC_SIZE);
    }

    friend bool operator<(const Claim &c1, const Claim &c2) {
        return c1.id < c2.id;
    }

    friend istream &operator>>(istream &is, Claim &claim) {
        char c;
        is >> c >> claim.id >> c >> claim.left >> c >> claim.top >> c >> claim.width >> c >> claim.height;
        return is;
    }

    friend ostream &operator<<(ostream &os, const Claim &claim) {
        os << "Claim{Id : " << claim.id << "; left : " << claim.left << "; top : " << claim.top << "; width : " << claim.width << "; height : " << claim.height << "}";
        return os;
    }
    
    bool overlapping(const Claim &other) const{
        return 
                (
                    (
                        other.left <= left && left < (other.left + other.width)
                        ||
                        left <= other.left && other.left < (left + width)
                    )
                    &&
                    (
                        other.top <= top && top < (other.top + other.height)
                        ||
                        top <= other.top && other.top < (top + height)
                    )
                );
    }    
};



vector<Claim> readFile();
vector<vector<int>> getFabric(const vector<Claim> &claims);
int countOfIntersect(const vector<vector<int>> &fabric);
void findNonOverlappingClaim(const vector<Claim> &claims);
bool isOverlapping(const Claim &c1, const Claim &c2);
void findNonOverlappingClaim2(const vector<vector<int>> &fabric, const vector<Claim> &claims);
void checkOverlapping(const vector<vector<int>> &fabric, const Claim &claims);

/*
 * 
 */
int main(int argc, char** argv) {
    auto claims = readFile();
    cout << "There are " << claims.size() << " claims.\n";
    auto fabric = getFabric(claims);
    int count = countOfIntersect(fabric);
    cout << "Common square inches : " << count << endl;
    findNonOverlappingClaim(claims);
    findNonOverlappingClaim2(fabric, claims);
    return 0;
}

vector<Claim> readFile() {
    ifstream ifile("input.txt");
    vector<Claim> claims;
    for (Claim claim; ifile >> claim;) {
        cout << claim << "\n";
        if (!claim.isValid()) {
            cout << "Claim is not valid!\n";
            exit(-1);
        }
        claims.push_back(claim);
    }
    return claims;
}

vector<vector<int>> getFabric(const vector<Claim> &claims) {
    vector<vector<int>> fabric(FABRIC_SIZE, vector<int>(FABRIC_SIZE, 0));
    for (Claim c : claims) {
        for (int i = c.left; i < c.left + c.width; ++i)
            for (int j = c.top; j < c.top + c.height; ++j)
                ++(fabric.at(i).at(j));
    }
    return fabric;
}

int countOfIntersect(const vector<vector<int>> &fabric) {
    int count = 0;
    for (auto v : fabric)
        for (int i : v)
            if (i > 1)
                ++count;
    return count;
}

void findNonOverlappingClaim2(const vector<vector<int>> &fabric, const vector<Claim> &claims) {
    for (auto c : claims)
        checkOverlapping(fabric, c);
}

void checkOverlapping(const vector<vector<int>> &fabric, const Claim &c) {
    bool overlapping = false;
    for (int i = c.left; i < c.left + c.width; ++i)
        for (int j = c.top; j < c.top + c.height; ++j) {
            int v = fabric.at(i).at(j);
            if (v > 1)
                overlapping = true;
        }
    if (!overlapping)
        cout << "Claim is not overlapping : " << c << endl;
}

void findNonOverlappingClaim(const vector<Claim> &claims) {
    set<Claim> overlapping;
    for (int i = 0; i < claims.size(); ++i) {
        for (int j = i + 1; j < claims.size(); ++j) {
            const Claim &c1 = claims[i];
            const Claim &c2 = claims[j];
            if (c1.overlapping(c2)) {
                overlapping.insert(c1);
                overlapping.insert(c2);
            }
        }
    }
    cout << "overlapping founds : " << overlapping.size() << "\n";
    vector<Claim> diff;
    set_difference(claims.begin(), claims.end(), overlapping.begin(), overlapping.end(), inserter(diff, diff.begin()));
    cout << "Non-overlapping claims : " << diff.size() << "\n";
    cout << "The first claim having no intersection with others : " << *(diff.begin()) << endl;
}

