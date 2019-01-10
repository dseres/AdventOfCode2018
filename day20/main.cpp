/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 23., 22:16
 */

#include <bits/stdc++.h>

using namespace std;

enum Direction {
    North = 0, East, South, West
};

Direction toDirection(char c) {
    switch (c) {
        case 'N': return North;
        case 'E': return East;
        case 'S': return South;
        case 'W': return West;
        default:
            cerr << "Bad char for direction." << endl;
            exit(-1);
    }
}

Direction getOpposite(Direction d) {
    return static_cast<Direction> ((static_cast<int> (d) + 2) % 4);
}

struct Pos {
    int x = 0, y = 0;

    Pos() = default;
    Pos(const Pos &) = default;
    Pos(Pos &&) = default;
    Pos &operator=(Pos &) = default;
    Pos &operator=(Pos &&) = default;
    ~Pos() = default;

    Pos(int _x, int _y) : x(_x), y(_y) {
    };

    friend bool operator<(const Pos &p, const Pos &q) {
        return make_pair(p.x, p.y) < make_pair(q.x, q.y);
    }

    friend bool operator==(const Pos &p, const Pos &q) {
        return make_pair(p.x, p.y) == make_pair(q.x, q.y);
    }

    Pos swapValues() const {
        return Pos(y, x);
    }

    static bool swapCompare(const Pos &p, const Pos &q) {
        return p.swapValues() < q.swapValues();
    }

    Pos getSouth() {
        return Pos(x, y + 1);
    }

    Pos getWest() {
        return Pos(x - 1, y);
    }

    Pos getEast() {
        return Pos(x + 1, y);
    }

    Pos getNorth() {
        return Pos(x, y - 1);
    }

    Pos getAdjacent(Direction d) {
        switch (d) {
            case North:
                return getNorth();
            case East:
                return getEast();
            case South:
                return getSouth();
            case West:
                return getWest();
            default:
                throw make_pair(-1, "Bad direction");
        }
    }

    static Pos Invalid() {
        return Pos(numeric_limits<int>::max(), numeric_limits<int>::max());
    }

    bool isInvalid() const {
        return *this == Invalid();
    }

    string to_string() const {
        return string("(") + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    friend ostream &operator<<(ostream &strm, const Pos &pos) {
        return strm << pos.to_string();
    }
};

struct Node {
    Pos pos;
    array<Pos, 4> adjacents;
    int distance;

    Node() {
        pos = Pos::Invalid();
        for (auto &a : adjacents)
            a = Pos::Invalid();
        distance = 0;
    };
    Node(const Node &) = default;
    Node(Node &&) = default;
    Node &operator=(Node &) = default;
    Node &operator=(Node &&) = default;
    ~Node() = default;

    Node(Pos p, int d = 0) : Node() {
        pos = p;
        distance = d;
    };

    void addRoute(Direction d, Pos p) {
        adjacents[d] = p;
    }
};

struct Map {
    map<Pos, Node> nmap;

    void load(string input_str) {
        istringstream input(input_str);
        return load(input);
    }

    void parse() {
        ifstream input("input.txt");
        return load(input);
    }

    void load(istream &input) {
        Node node(Pos(0, 0), 0);
        nmap.insert(make_pair(node.pos, node));
        vector<Node> stack;
        for (char c; input >> c;) {
            switch (c) {
                case '^':
                case '$':
                    break;
                case 'N':
                case 'E':
                case 'S':
                case 'W':
                    node = insertNode(node, toDirection(c));
                    break;
                case '(':
                    stack.push_back(node);
                    break;
                case '|':
                {
                    node = stack.back();
                    break;
                }
                case ')':
                    node = stack.back();
                    stack.pop_back();
                    break;
            }
        }
    }

    Node insertNode(Node node, Direction dir) {
        Pos pos = node.pos.getAdjacent(dir);
        auto it = nmap.find(pos);
        if (it == nmap.end()) {
            Node nn(pos, node.distance + 1);
            nn.adjacents[getOpposite(dir)] = node.pos;
            nmap.insert(make_pair(nn.pos, nn));
            nmap[node.pos].adjacents[dir] = nn.pos;
            return nn;
        } else {
            Node &nn = it->second;
            nn.adjacents[getOpposite(dir)] = node.pos;
            nmap[node.pos].adjacents[dir] = nn.pos;
            if (nn.distance > node.distance + 1) {
                changeDistance(nn, node.distance + 1);
            }
            return nn;
        }
    }

    void changeDistance(Node &node, int distance) {
        node.distance = distance;
        for (Pos p : node.adjacents) {
            auto it = nmap.find(p);
            if (it != nmap.end() && it->second.distance > distance + 1) {
                changeDistance(it->second, distance + 1);
            }
        }
    }

    int getLargestDistance() {
        auto it = max_element(nmap.begin(), nmap.end(), [](const auto &p1, const auto &p2) {
            return p1.second.distance < p2.second.distance; });
        return it->second.distance;
    }

    int countRoomsAtDistance(int distance = 1000) {
        int count = count_if(nmap.begin(), nmap.end(), [](auto pr) {
            return pr.second.distance >= 1000;
        });
        return count;
    }
};

void unitTestImpl(string path, int distance) {
    Map m;
    m.load(path);
    assert(m.getLargestDistance() == distance);
}

void unitTest() {
    unitTestImpl("^ESSWWN(E|NNENN(EESS(WNSE|)SSS|WWWSSSSE(SW|NNNE)))$", 23);
    unitTestImpl("^WSSEESWWWNW(S|NENNEEEENN(ESSSSW(NWSW|SSEN)|WSWWN(E|WWS(E|SS))))$", 31);
}

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest();
    Map m;
    ifstream input("input.txt");
    m.load(input);
    cout << "Solution1 : " << m.getLargestDistance() << endl;
    cout << "Solution2 : " << m.countRoomsAtDistance() << endl;
    return 0;
}

