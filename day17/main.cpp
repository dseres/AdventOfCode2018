/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 21, 2018, 9:13 AM
 */

#include <bits/stdc++.h>
#include <istream>

using namespace std;

class WaterSystem {
public:
    //typedef vector<vector<char>> WorldT;

    enum Tile : char {
        Sand = '.', Clay = '#', Spring = '+', Passing = '|', Resting = '~'
    };

    enum Direction {
        Top, Down, Left, Right
    };

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

        Pos getUnder() {
            return Pos(x, y + 1);
        }

        Pos getLeft() {
            return Pos(x - 1, y);
        }

        Pos getRight() {
            return Pos(x + 1, y);
        }
    };



    typedef map< Pos, Tile> WorldT;

    static bool swapCompare(const WorldT::value_type &p, const WorldT::value_type &q) {
        return Pos::swapCompare(p.first, q.first);
    }
private:
    WorldT world = {make_pair(Pos(500, 0), Tile::Spring)};
    pair<Pos, Pos> rect = make_pair(Pos(500, 0), Pos(500, 0));
    pair<int, int> size = make_pair(1, 1);

public:

    Tile getTile(Pos pos) const {
        auto it = world.find(pos);
        if (it == world.end()) {
            return Tile::Sand;
        } else return it->second;
    }

    Tile getTile(int x, int y) const {
        return getTile(Pos(x, y));
    }

    pair<Pos, Pos> getRect() const {
        auto its_x = minmax_element(world.begin(), world.end());
        auto its_y = minmax_element(world.begin(), world.end(), &swapCompare);
        Pos minp(its_x.first->first.x, its_y.first->first.y);
        Pos maxp(its_x.second->first.x, its_y.second->first.y);
        return make_pair(minp, maxp);
    }

    pair<int, int> getSize() const {
        return make_pair(rect.second.x - rect.first.x + 1, rect.second.y - rect.first.y + 1);
    }

    friend istream &operator>>(istream &strm, WaterSystem &ws) {
        string line;
        while (getline(strm, line)) {
            static regex r(R"(^\s*([xy])=(\d+),\s*([xy])=(\d+)..(\d+)\s*$)");
            smatch m;
            if (regex_match(line, m, r)) {
                if (m[1] == "x") {
                    int x = stoi(m[2]);
                    if (m[3] != "y") {
                        cerr << "Bad input : " << line << endl;
                        exit(-1);
                    }
                    int y1 = stoi(m[4]), y2 = stoi(m[5]);
                    for (int i = y1; i <= y2; ++i) {
                        ws.world[ Pos(x, i) ] = Tile::Clay;
                    }
                } else {
                    int y = stoi(m[2]);
                    if (m[3] != "x") {
                        cerr << "Bad input : " << line << endl;
                        exit(-1);
                    }
                    int x1 = stoi(m[4]), x2 = stoi(m[5]);
                    for (int i = x1; i <= x2; ++i) {
                        ws.world[ Pos(i, y) ] = Tile::Clay;
                    }
                }
            } else {
                cerr << "Bad input : " << line << endl;
                exit(-1);
            }
        }
        ws.rect = ws.getRect();
        ws.size = ws.getSize();
        return strm;
    }

    friend ostream &operator<<(ostream &strm, const WaterSystem &ws) {
        Pos min, max;
        tie(min, max) = ws.getRect();
        cout << "Rectangle of world : " << min.x << " " << min.y << " " << max.x << " " << max.y << "\n";
        auto size = ws.getSize();
        cout << "Size of world : " << size.first << " " << size.second << "\n";
        for (int y = min.y; y <= max.y; ++y) {
            for (int x = min.x; x <= max.x; ++x) {
                cout << static_cast<char> (ws.getTile(x, y));
            }
            cout << "\n";
        }
        return strm;
    }

    int iterate() {
        auto p = step(Pos(500, 0).getUnder(), Direction::Down);
        return p.first;
    }

    pair<int, bool> step(Pos pos, Direction dir) {
        //        cout<<*this; char c; cin>>noskipws>>c;

        //Check end conditions
        if (pos.y > rect.second.y)
            return make_pair(0, true);
        if (dir == Direction::Down && getTile(pos) == Tile::Passing)
            return make_pair(0, true);
        if (getTile(pos) != Tile::Sand)
            return make_pair(0, false);

        //Add current value
        int counter = 1;
        world[pos] = Tile::Passing;
        //Check adjacents
        auto u = step(pos.getUnder(), Direction::Down);
        counter += u.first;
        bool opened = u.second;
        if (!u.second) {
            auto l = step(pos.getLeft(), Direction::Left);
            counter += l.first;
            opened = opened || l.second;
            auto r = step(pos.getRight(), Direction::Right);
            counter += r.first;
            opened = opened || r.second;
            if ( r.second && !l.second)
                clearLeft(pos.getLeft());
            if ( l.second && !r.second)
                clearRight(pos.getRight());
        }
        if (!opened)
            world[pos] = Tile::Resting;
        return make_pair(counter, opened);
    }
    
    void clearLeft(Pos pos){
        if ( getTile(pos) == Tile::Resting )
        {
            world[pos]=Tile::Passing;
            clearLeft(pos.getLeft());
        }
    }
    
    void clearRight(Pos pos){
        if ( getTile(pos) == Tile::Resting )
        {
            world[pos]=Tile::Passing;
            clearRight(pos.getRight());
        }
    }
    
    int countWater(){
        return count_if(world.begin(), world.end(), [](auto p){ return p.second == Tile::Passing || p.second == Tile::Resting;});
    }

    int countRestingWater(){
        return count_if(world.begin(), world.end(), [](auto p){ return p.second == Tile::Resting;});
    }
    
    int minYofClays(){
        WorldT w2;
        copy_if(world.begin(), world.end(), inserter(w2, w2.end()), [](auto p){return p.second == Tile::Clay;});
        auto it = min_element(w2.begin(), w2.end(), &swapCompare);
        return it->first.y;
    }
};

void unitTest() {
    string input = R"(x=495, y=2..7
y=7, x=495..501
x=501, y=3..7
x=498, y=2..4
x=506, y=1..2
x=498, y=10..13
x=504, y=10..13
y=13, x=498..504)";
    istringstream is(input);
    WaterSystem wsystem;
    is>>wsystem;
    int count = wsystem.iterate();
    cout << wsystem;
    cout << "Counter : " << count << endl;
    assert(count == 57);
}

void getSolution1() {
    WaterSystem wsystem;
    (ifstream("input.txt")) >> wsystem;
    int count = wsystem.iterate();
    cout << wsystem;
    cout << "Counter : " << count << endl;
    cout << "Corrigated count : "<<(count-wsystem.minYofClays()+1)<<endl;
    cout << "Count of resting water : "<<wsystem.countRestingWater()<<endl;
}

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest();
    getSolution1();
    return 0;
}

