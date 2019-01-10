/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 10, 2018, 2:46 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>

using namespace std;

constexpr bool debug = false;

struct Marbles {
    typedef vector<long long> PlayersT;
    typedef list<int> MarblesT;
    typedef MarblesT::iterator MarblesIterator;

    PlayersT players;
    MarblesT marbles;
    MarblesIterator it;
    int no_of_players;
    int last_marble;
    int player;
    int marble;

    Marbles(int _no_of_players, int _last_marble) : no_of_players(_no_of_players), last_marble(_last_marble) {
        players = PlayersT(no_of_players, 0);
        player = 0;
        it = marbles.insert(marbles.end(), 0);
        marble = 1;
    }

    void getScores() {
        while (marble <= last_marble) {
            if (marble % 23 == 0) {
                rule23();
            } else {
                it = marbles.insert(++it, marble);
            }
            if (debug) {
                //Debug
                for (int m : marbles) {
                    if (*it == m)
                        cout << "*";
                    cout << m << " ";
                }
                cout << endl;
                char c;
                cin>>c;
            }
            if (++it == marbles.end())
                it = marbles.begin();
            player = (++player) % players.size();
            ++marble;
        }
    }

    void rule23() {
        for (int i = 0; i < 8; ++i)
            if (it == marbles.begin())
                it = --marbles.end();
            else
                --it;
        players[player] += marble + *it;
        it = marbles.erase(it);
    }

    void getSolution() {
        getScores();
        long long max_score = *max_element(players.begin(), players.end());
        long long sum_score = accumulate(players.begin(), players.end(), 0L);
        cout << "Players " << no_of_players << ", marbles " << last_marble << " : " << max_score << endl;
    }
};

/*
 * 
 */
int main(int argc, char** argv) {

    (Marbles(9, 25)).getSolution();
    (Marbles(1, 48)).getSolution();
    (Marbles(10, 1618)).getSolution();
    (Marbles(30, 5807)).getSolution();
    (Marbles(470, 72170)).getSolution();
    (Marbles(470, 7217000)).getSolution();
    return 0;
}
