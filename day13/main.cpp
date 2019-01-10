/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 13., 8:35
 */

#include <bits/stdc++.h>

using namespace std;

string test_input = R"(/->-\        
|   |  /----\
| /-+--+-\  |
| | |  | v  |
\-+-/  \-+--/
  \------/   )";

string test_input2 = R"(/>-<\  
|   |  
| /<+-\
| | | v
\>+</ |
  |   ^
  \<->/)";

struct TransportSystem {

    enum TrackElem {
        Vertical = '|', Horizontal = '-', CurveLeft = '\\', CurveRight = '/', Intersection = '+', Empty = ' '
    };

    struct Car {

        enum Operation {
            Left = 0, Straight, Right, MaxOp
        };

        enum Direction {
            North = '^', South = 'v', East = '>', West = '<'
        };

        int id = -1;
        int x = 0;
        int y = 0;
        Operation op = Left;
        Direction dir = North;
        bool dead = false;

        Car() = default;
        Car(const Car &) = default;
        Car(Car &&) = default;
        Car &operator=(const Car &) = default;
        Car &operator=(Car &&) = default;
        ~Car() = default;

        Car(int _id, int _x, int _y, char d): id(_id), x(_x), y(_y) {
            dir = static_cast<Direction> (d);
        }

        friend bool operator<(const Car c, const Car d) {
            return make_pair(c.y, c.x) < make_pair( d.y, d.x);
        }

        bool collided(const Car &other) const {
            return other.id != id && isOnPosition(other.x ,other.y );
        }

        bool isOnPosition(const int x, const int y) const {
            return this->x == x && y == this->y;
        }

        void nextOperation() {
            op = static_cast<Operation> ((static_cast<int> (op) + 1) % static_cast<int> (MaxOp));
        };

        char toChar() const {
            if (dead)
                return 'X';
            return static_cast<char> (dir);
        }

        void nextDirectrion(TrackElem t) {
            switch (t) {
                case Vertical:
                    break;
                case Horizontal:
                    break;
                case CurveLeft:
                    switch (dir) {
                        case North:
                            dir = West;
                            break;
                        case South:
                            dir = East;
                            break;
                        case East:
                            dir = South;
                            break;
                        case West:
                            dir = North;
                            break;
                    }
                    break;
                case CurveRight:
                    switch (dir) {
                        case North:
                            dir = East;
                            break;
                        case South:
                            dir = West;
                            break;
                        case East:
                            dir = North;
                            break;
                        case West:
                            dir = South;
                            break;
                    }
                    break;
                case Intersection:
                    switch (dir) {
                        case North:
                            switch (op) {
                                case Left:
                                    dir = West;
                                    break;
                                case Straight:
                                    break;
                                case Right:
                                    dir = East;
                                    break;
                            }
                            break;
                        case South:
                            switch (op) {
                                case Left:
                                    dir = East;
                                    break;
                                case Straight:
                                    break;
                                case Right:
                                    dir = West;
                                    break;
                            }
                            break;
                        case East:
                            switch (op) {
                                case Left:
                                    dir = North;
                                    break;
                                case Straight:
                                    break;
                                case Right:
                                    dir = South;
                                    break;
                            }
                            break;
                        case West:
                            switch (op) {
                                case Left:
                                    dir = South;
                                    break;
                                case Straight:
                                    break;
                                case Right:
                                    dir = North;
                                    break;
                            }
                            break;
                    }
                    nextOperation();
                    break;
            }
        }

        void tick( const TransportSystem st) {
            switch (dir) {
                case North:
                    --y;
                    break;
                case South:
                    ++y;
                    break;
                case East:
                    ++x;
                    break;
                case West:
                    --x;
                    break;
            }
            nextDirectrion(st.tracks[y][x]);
        }
        
        void checkCollition( vector<Car> &cars){
            for(Car &c: cars)
                if ( collided(c) )
                {
                    dead = true;
                    c.dead = true;
                }
        }
    };

    typedef vector<Car> CarContainerT;
    typedef vector<TrackElem> RowT;
    typedef vector<RowT> TracksT;

    CarContainerT cars;
    TracksT tracks;

    void readFile() {
        ifstream ifile("input.txt");
        ifile>>*this;
    }

    friend istream &operator>>(istream &is, TransportSystem &st) {
        char c;
        is>>noskipws;
        RowT row;
        int x = 0, y = 0;
        while (is >> c) {
            switch (c) {
                case Empty:
                case Vertical:
                case Horizontal:
                case CurveLeft:
                case CurveRight:
                case Intersection:
                    row.push_back(static_cast<TrackElem> (c));
                    break;
                case '>':
                case '<':
                {
                    st.cars.push_back(Car(st.cars.size(), x, y, c));
                    row.push_back(Horizontal);
                    break;
                }
                case 'v':
                case '^':
                {
                    st.cars.push_back(Car(st.cars.size(), x, y, c));
                    row.push_back(Vertical);
                    break;
                }
                case '\a':
                    break;
                case '\n':
                    x = -1;
                    ++y;
                    st.tracks.push_back(move(row));
                    row = RowT();
                    break;
            }
            ++x;
        }
        st.tracks.push_back(move(row));
        return is;
    }

    friend ostream &operator<<(ostream &os, const TransportSystem &st) {
        for (Car c : st.cars)
            os << "Car : " << c.x << " " << c.y << " " << c.toChar() << endl;
        for (int y = 0; y < st.tracks.size(); ++y) {
            for (int x = 0; x < st.tracks[y].size(); ++x) {
                auto it = find_if(st.cars.begin(), st.cars.end(), [x, y](Car c) {
                    return c.isOnPosition(x, y);
                });
                if (it != st.cars.end())
                    cout << it->toChar();
                else
                    cout << static_cast<char> (st.tracks[y][x]);
            }
            cout << endl;
        }
    }


    void tick() {
        sort(cars.begin(), cars.end());
        for (Car &c : cars) {
            if ( !c.dead )
            {
                c.tick(*this);
                c.checkCollition(cars);
            }
        }
        //cout<<*this;
    }


    CarContainerT removeDeads(){
        CarContainerT deads;
        CarContainerT cars2;
        for (Car &c : cars)
            if (c.dead)
                deads.push_back(move(c));
            else
                cars2.push_back(move(c));
        cars = move(cars2);
        return move(deads);
    }
    
    Car iterate() {
        while (true) {
            tick();
            auto deads = removeDeads();
            if (!deads.empty())
                return deads[0];
        }
    }

    Car iterate2() {
        while (1 < cars.size()) {
            iterate();
        }
        return *(cars.begin());
    }
};


void unitTest() {
    istringstream is(test_input);
    TransportSystem st;
    is>>st;
    cout << st;
    auto c = st.iterate();
    cout << st;
    cout << "Cars were collided in position : " << c.x << " " << c.y << endl;
    assert(c.isOnPosition(7, 3));
}

void unitTest2() {
    istringstream is(test_input2);
    TransportSystem st;
    is>>st;
    cout << st;
    auto c = st.iterate2();
    cout << "Position of last car : " << c.x << " " << c.y << endl;
    cout << st;
    assert(c.isOnPosition(6, 4));
}

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest();
    unitTest2();

    TransportSystem st;
    st.readFile();
    auto c = st.iterate();
    cout << "Cars were collided in position: " << c.x << " " << c.y << endl;
    c = st.iterate2();
    cout << "Position of last car : " << c.x << " " << c.y << endl;
    assert(c.isOnPosition(21, 91));
    
    return 0;
}

