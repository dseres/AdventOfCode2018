/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on January 2, 2019, 9:40 AM
 */

#include <bits/stdc++.h>
#include <limits>
using namespace std;

enum CaveType : int {
    Rocky = 0, Wet, Narrow
};
constexpr int MaxCaveType = CaveType::Narrow + 1;
typedef vector<vector<CaveType>> CaveMapT;

typedef long long ErosionT;
typedef vector<vector<ErosionT>> ErosionMapT;

enum Tool : int {
    Torch = 0, Gear, Neither
};
constexpr int MaxTool = Tool::Neither + 1;

struct Node {
    int x, y;
    Tool tool;
    int distance;

    constexpr static int ToolDistance = 7;
    constexpr static int TypeDistance = 1;

    static Tool firstTool(CaveType type) {
        return static_cast<Tool> (static_cast<int> (type));
    }

    static Tool secondaryTool(CaveType type) {
        return static_cast<Tool> ((static_cast<int> (type) + 1) % MaxTool);
    }

    bool isPassable(CaveType ct, Tool t) const {
        return tool == firstTool(ct) || tool == secondaryTool(ct);
    }
    
    vector<Node> getAdjacents(const CaveMapT &cave_map) const {
        vector<Node> adjacents;
        CaveType type = cave_map[x][y];
        if (x > 0) {
            addAdjacent(cave_map, &adjacents, x - 1, y);
        }
        if (x < cave_map.size() - 1) {
            addAdjacent(cave_map, &adjacents, x + 1, y);
        }
        if (y > 0) {
            addAdjacent(cave_map, &adjacents, x, y - 1);
        }
        if (y < cave_map[x].size() - 1) {
            addAdjacent(cave_map, &adjacents, x, y + 1);
        }
        if (tool == firstTool(type)) {
            adjacents.push_back(Node{x, y, secondaryTool(type), distance + ToolDistance});
        }
        if (tool == secondaryTool(type)) {
            adjacents.push_back(Node{x, y, firstTool(type), distance + ToolDistance});
        }
        return adjacents;
    };

    void addAdjacent(const CaveMapT& cave_map, vector<Node> *adjacents, int adj_x, int adj_y) const {
        if (isPassable(cave_map[adj_x][adj_y], tool)) {
            Node adj{adj_x, adj_y, tool, distance + TypeDistance};
            adjacents->push_back(adj);
        }
    }

    struct CompareByDistance {

        bool operator()(const Node &n, const Node &m) {
            return n.distance < m.distance;
        }
    };

    struct CompareByPos {

        bool operator()(const Node &n, const Node &m) {
            return make_tuple(n.x, n.y, n.tool) < make_tuple(m.x, m.y, m.tool);
        }
    };

};

ErosionMapT getErosionMatrix(int depth, int target_x, int target_y) {
    ErosionMapT erosion_level = ErosionMapT(target_x + 100 + 1, vector<ErosionT>(target_y + 100, 0));
    const ErosionT modulo = 20183;
    for (int x = 0; x < erosion_level.size(); ++x) {
        for (int y = 0; y <= erosion_level[x].size(); ++y) {
            ErosionT geo_index;
            if (x == 0 && y == 0) {
                geo_index = 0;
            } else if (x == target_x && y == target_y) {
                geo_index = 0;
            } else if (x == 0) {
                geo_index = static_cast<ErosionT> (y) * 48271;
            } else if (y == 0) {
                geo_index = static_cast<ErosionT> (x) * 16807;
            } else {
                geo_index = erosion_level[x - 1][y] * erosion_level[x][y - 1];
            }
            erosion_level[x][y] = (geo_index + depth) % modulo;
        }
    }
    return erosion_level;
}

CaveMapT getLevelMatrix(const ErosionMapT &erosion_level) {
    CaveMapT types = CaveMapT(erosion_level.size(), vector<CaveType>(erosion_level.at(0).size(), CaveType::Rocky));
    for (int x = 0; x < types.size(); ++x) {
        for (int y = 0; y < types[x].size(); ++y) {
            CaveType type = static_cast<CaveType> (erosion_level[x][y] % 3);
            types[x][y] = type;
        }
    }
    return types;
}

int getRiskLevel(int target_x, int target_y, const CaveMapT& types) {
    int risk_level = 0;
    for (int y = 0; y <= target_y; ++y) {
        for (int x = 0; x <= target_x; ++x) {
            risk_level += static_cast<int> (types[x][y]);
            //const string display = ".=|";
            //cout << display[types[x][y]];
        }
        //cout << endl;
    }
    return risk_level;
}

int getDistance(int target_x, int target_y, const CaveMapT& types) {
    multiset<Node, Node::CompareByDistance> opened;
    set<Node, Node::CompareByPos> closed;
    opened.insert(Node{0, 0, Tool::Torch, 0});
    while (!opened.empty()) {
        const Node node = *(opened.begin());
        for (Node adj : node.getAdjacents(types)) {
            if (closed.find(adj) == closed.end()) {
                auto it2 = find_if(opened.begin(), opened.end(), [adj](const Node & n1) {
                    return adj.x == n1.x && adj.y == n1.y && adj.tool == n1.tool;
                });
                if (it2 == opened.end()) {
                    opened.insert(adj);
                } else {
                    if (adj.distance < it2->distance) {
                        opened.erase(it2);
                        opened.insert(adj);
                    }
                }
            }
        }
        closed.insert(node);
        opened.erase(opened.begin());
        if (node.x == target_x && node.y == target_y && node.tool == Tool::Torch ) {
            return node.distance;
        }
    }
    return -1;
}

pair<int, int> getSolution(int depth, int target_x, int target_y) {
    auto erosion_level = getErosionMatrix(depth, target_x, target_y);
    CaveMapT types = getLevelMatrix(erosion_level);
    int risk_level = getRiskLevel(target_x, target_y, types);
    int distance = getDistance(target_x, target_y, types);
    return make_pair(risk_level, distance);
}

void unitTest() {
    assert(getSolution(510, 10, 10) == make_pair(114, 45));
}

int main(int argc, char** argv) {
    unitTest();
    std::cout << endl;
    int risk_level, distance;
    tie(risk_level, distance) = getSolution(4848, 15, 700);
    std::cout << "Solution1 : " << risk_level << endl;
    std::cout << "Solution2 : " << distance << endl;
    return 0;
}
