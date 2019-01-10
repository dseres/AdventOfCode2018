/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 22., 19:44
 */


#include <bits/stdc++.h>
#include <istream>

using namespace std;

class LumberCollectionArea {
public:

    enum Tile : char {
        Open = '.', Trees = '|', Lumberyard = '#', Undef = '\0'
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

        vector<Pos> getAdjacents(vector<vector<Tile>> area) {
            vector<Pos> adjs;
            for( int i=-1; i<=1; ++i){
                for( int j=-1; j<=1; ++j){
                    if ( !(i==0 && j==0)
                         && x+i>=0 && x+i<area.size()
                         && y+j>=0 && y+j<area[x+i].size() ){
                        adjs.push_back(Pos(x+i, y+j));
                    }
                }
            }
            return adjs;
        }

    };

    typedef vector<vector<Tile>> AreaT;
    
    

private:
    AreaT area;

public:

    Tile &at(Pos pos) {
        return area[pos.x][pos.y];
    }

    Tile &at(int x, int y) {
        return at(Pos(x, y));
    }

    friend istream &operator>>(istream &strm, LumberCollectionArea &lca) {
        string line;
        while (getline(strm, line)) {
            if (!line.empty()) {
                AreaT::value_type row;
                for (char c : line)
                    row.push_back(static_cast<Tile> (c));
                lca.area.push_back(row);
            }
        }
        return strm;
    }

    friend ostream &operator<<(ostream &strm, const LumberCollectionArea &lca) {
        for (auto r : lca.area) {
            for (auto t : r) {
                cout << static_cast<char> (t);
            }
            cout << endl;
        }
        return strm;
    }

    int iterate(int count) {
        for (int i = 0; i < count; ++i) {
            tick();
            cout<<*this<<"Counter : "<<i<<endl;
        }
    }

    int iterateToRepetition() {
        map<AreaT, int> saved;
        saved.insert( make_pair( area, 0));
        for(int counter = 1; true ; ++counter){
            tick();
            cout//<<*this
                    <<"Counter : "<<counter<<" Resource value : <<"<<getResourceValue()<<endl;
            auto it = saved.find(area);
            if ( it != saved.end() ){
                cout<<"Current counter : "<<counter<<" found repetition counter : "<<it->second<<endl;
                long long future = 1000000000;
                int period = counter - it->second;
                int index = static_cast<int>(( future - it->second) % period) + it->second;
                auto it2 = find_if(saved.begin(), saved.end(), [index](auto pr){return pr.second == index;});
                int resource_value = getResourceValue(it2->first);
                cout<<"Resource value in " << future<<" : " <<resource_value<<endl;
                break;
            }
            saved.insert( make_pair(area, counter));
        }
    }

    void tick() {
        AreaT area2 = AreaT(area.size(), vector<Tile>(area[0].size(), Tile::Undef));
        for (int x = 0; x < area2.size(); ++x) {
            for (int y = 0; y < area2[x].size(); ++y) {
                area2[x][y] = getNewTile(x, y);
            }
        }
        area = area2;
    }

    Tile getNewTile(int x, int y) {
        Tile t = at(x,y);
        switch (at(x, y)) {
            case Tile::Open:
                if ( chekAdj(x,y, Tile::Trees,3))
                    t = Tile::Trees;
                break;
            case Tile::Trees:
                if ( chekAdj(x,y, Tile::Lumberyard,3))
                    t = Tile::Lumberyard;
                break;
            case Tile::Lumberyard:
                if ( !(chekAdj(x,y, Tile::Lumberyard,1) && chekAdj(x,y, Tile::Trees,1)))
                    t = Tile::Open;
                break;
        }
        return t;
    }
    
    bool chekAdj(int x, int y, Tile type, int min_no){
        auto adjs = (Pos(x,y)).getAdjacents(area);
        auto &area = this->area;
        int  count = count_if( adjs.begin(), adjs.end(), [type, area ](auto p){ return area[p.x][p.y] == type;});
        return count >= min_no;
    }
    
    int getResourceValue(){
        return getResourceValue(this->area);
    }

    int getResourceValue(const AreaT &area){
        int tc=0, lc=0;
        for(auto r: area){
            for(auto t: r){
                if ( t == Tile::Trees)
                    ++tc;
                else if ( t == Tile::Lumberyard)
                    ++lc;
            }
        }
        return tc * lc;
    }

};

void unitTest() {
    string input = R"(.#.#...|#.
.....#|##|
.|..|...#.
..|#.....#
#.#|||#|#|
...#.||...
.|....|...
||...#|.#|
|.||||..|.
...#.|..|.
)";
    istringstream is(input);
    LumberCollectionArea lca;
    is>>lca;
    cout << lca<<endl;
    lca.iterate(10);
    assert ( lca.getResourceValue() == 31*37);
}

void getSolution1() {
    LumberCollectionArea lca;
    (ifstream("input.txt")) >> lca;
    cout << lca;
    lca.iterate(10);
    cout<<"Resource value : "<<lca.getResourceValue()<<endl;
}

void getSolution2() {
    LumberCollectionArea lca;
    (ifstream("input.txt")) >> lca;
    cout << lca;
    lca.iterateToRepetition();
}

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest();
    getSolution1();
    getSolution2();
    return 0;
}

