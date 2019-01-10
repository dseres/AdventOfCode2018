/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 15., 15:47
 */

#include <bits/stdc++.h>
#include <cassert>
#include "unit_test_input.h"

using namespace std;

enum Tile {
    Wall = '#', Cavern = '.', Elv = 'E', Goblin = 'G', InRange = '?', Reachable = '@', Target = '!', UnDef = '\0'
};

ostream &operator<<(ostream &os, const Tile &t) {
    return os << static_cast<char> (t);
}

istream &operator>>(istream &is, Tile &t) {
    char c;
    is >> c;
    t = static_cast<Tile> (c);
    return is;
}

typedef vector<vector<Tile>> BoardT;
BoardT board;

ostream &operator<<(ostream &os, const BoardT &board) {
    for (auto row : board) {
        for (auto c : row) {
            os << c;
        }
        os << endl;
    }
}

istream &operator>>(istream &is, BoardT &board) {
    Tile t;
    is>>noskipws;
    BoardT::value_type row;
    while (is >> t) {
        switch (t) {
            case '\n':
                assert(board.size() == 0 || board.size() > 0 && row.size() == board[0].size());
                board.push_back(move(row));
                row = BoardT::value_type();
                break;
            case '.':
            case '#':
            case 'E':
            case 'G':
                row.push_back(t);
                break;
            default:
                assert(false);
        }
    }
    if (!row.empty()) {
        assert(board.size() == 0 || board.size() > 0 && row.size() == board[0].size());
        board.push_back(move(row));
    }
}

struct Pos {
    int x, y;

    static Pos Invalid() {
        return Pos{-1, -1};
    }

    bool isInvalid() {
        return x == -1 && y == -1;
    }

    /**
     * Return available adjacent positions in read order.
     * @param board
     * @return 
     */
    auto getAdjacents(const BoardT &board) const {
        vector<Pos> adjacents;
        if (y > 0)
            adjacents.push_back({x, y - 1}); //Top
        if (x > 0)
            adjacents.push_back({x - 1, y}); //Left
        if (x < board[y].size() - 1)
            adjacents.push_back({x + 1, y}); //Right
        if (y < board.size() - 1)
            adjacents.push_back({x, y + 1}); //Down
        return adjacents;
    }

    friend bool operator<(const Pos &p, const Pos &q) {
        return make_pair(p.y, p.x) < make_pair(q.y, q.x);
    }

    friend bool operator==(const Pos &p, const Pos &q) {
        return make_pair(p.y, p.x) == make_pair(q.y, q.x);
    }
};

struct Puppet {
    static constexpr int max_hp = 200;
    static constexpr int gobling_attack_power = 3;
    static int elv_attack_power;
    Tile type;
    Pos pos;
    int hp = max_hp;

    Puppet(Tile _type, int _x, int _y) : Puppet(_type, Pos{_x, _y}) {
    }

    Puppet(Tile _type, Pos _pos) : type(_type), pos(_pos) {
        assert(type == Elv || type == Goblin);
    }

    Tile getOpposite() const {
        if (type == Elv)
            return Goblin;
        if (type == Goblin)
            return Elv;
    }

    int getAttackPower() const {
        if (type == Tile::Goblin)
            return gobling_attack_power;
        else
            return elv_attack_power;
    }

    friend bool operator<(const Puppet &p, const Puppet &q) {
        return p.pos < q.pos;
    }

    friend bool operator==(const Puppet &p, const Puppet &q) {
        return p.pos == q.pos;
    }

};

struct Game {
    int sizex, sizey;
    BoardT board;
    map<Pos, Puppet> puppets;
    int died_elves = 0;

    Game() = default;
    Game(const Game &) = default;
    Game(Game &&) = default;
    Game &operator=(const Game &) = default;
    Game &operator=(Game &&) = default;
    ~Game() = default;

    Game(BoardT b) : board(b) {
        sizex = board[0].size();
        sizey = b.size();
        for (int y = 0; y < sizey; ++y) {
            for (int x = 0; x < sizex; ++x) {
                if (board[y][x] == Tile::Goblin || board[y][x] == Tile::Elv) {
                    Puppet p(board[y][x], x, y);
                    puppets.insert(make_pair(p.pos, p));
                }
            }
        }
    }
};

ostream &operator<<(ostream &os, const Game &game) {
    auto p = game.puppets.begin();
    ostringstream tmp_str;
    for (auto row : game.board) {
        for (auto c : row) {
            if (c == Tile::Elv || c == Tile::Goblin) {
                tmp_str << ", " << (p->second.type) << "(" << (p->second.hp) << ")";
                ++p;
            }
            os << c;
        }
        if (tmp_str.tellp() > 0) {
            os << "   " << tmp_str.str().substr(2);
            tmp_str = ostringstream();
        }
        os << endl;
    }
}

class PuppetLogic {
private:
    Game game;
    typedef vector<vector<int>> DistanceMatrix;
    Puppet puppet;

public:

    PuppetLogic(Puppet _puppet, const Game &_game) : puppet(_puppet), game(_game) {
        auto it = game.puppets.find(puppet.pos);
        puppet = it->second;
        game.puppets.erase(it);
    }

    Game getGame() {
        return move(game);
    }

    bool turn() {
        if (hasOpponent()) {
            attack();
            putBackPuppet();
            return true;
        } else {
            Pos target = findTarget();
            if (!target.isInvalid()) {
                step(target);
                cleanBoard();
                if (hasOpponent())
                    attack();
                putBackPuppet();
                return true;
            } else {
                cleanBoard();
                putBackPuppet();
                return false;
            }
        }
    }

private:

    void cleanBoard() {
        for (int y = 0; y < game.sizey; ++y) {
            for (int x = 0; x < game.sizex; ++x) {
                if (isEmptyPlace(game.board[y][x]))
                    game.board[y][x] = Tile::Cavern;
            }
        }
    }

    void putBackPuppet() {
        game.puppets.insert(make_pair(puppet.pos, puppet));
        game.board[puppet.pos.y][puppet.pos.x] = puppet.type;
    }

    bool isEmptyPlace(Tile t) {
        return (t == Tile::Cavern || t == Tile::InRange || t == Tile::Reachable || t == Tile::Target);
    }

    DistanceMatrix createEmptyDistanceM() {
        return vector<vector<int>>(game.sizey, vector<int>(game.sizex, -1));
    }

    DistanceMatrix createDistanceM(Pos pos) {
        auto m = createEmptyDistanceM();
        m[pos.y][pos.x] = 0;
        for (bool has_movement = true; has_movement;) {
            has_movement = false;
            for (int y = 0; y < game.sizey; ++y) {
                for (int x = 0; x < game.sizex; ++x) {
                    if (m[y][x] >= 0) {
                        for (auto q : Pos{x, y}
                            .getAdjacents(game.board)) {
                            if (m[q.y][q.x] == -1 && isEmptyPlace(game.board[q.y][q.x])) {
                                m[q.y][q.x] = m[y][x] + 1;
                                has_movement = true;
                            }
                        }
                    }
                }
            }
        }
        return m;
    }

    Pos findTarget() {
        //Check whether in range default.
        //Computing tiles in range
        set<Pos> in_range;
        for (auto par : game.puppets) {
            Puppet &p = par.second;
            if (p.type == puppet.getOpposite()) {
                for (Pos pos : p.pos.getAdjacents(game.board)) {
                    if (isEmptyPlace(game.board[pos.y][pos.x])) {
                        in_range.insert(pos);
                        game.board[pos.y][pos.x] = Tile::InRange;
                    }
                }
            }
        }
        if (in_range.empty())
            return Pos::Invalid();
//        cout << game.board;
        return findReachables(in_range);
    }

    Pos findReachables(const set<Pos> &in_range) {
        set<pair<int, Pos>> reachables;
        DistanceMatrix distances = createDistanceM(puppet.pos);
        for (auto p : in_range) {
            int distance = distances[p.y][p.x];
            if (distance >= 0){
                reachables.insert(make_pair(distance, p));
                game.board[p.y][p.x] = Tile::Reachable;
            }
        }
        if (reachables.empty())
            return Pos::Invalid();
        Pos target = (reachables.begin())->second;
        //Visualisation
        game.board[target.y][target.x] = Tile::Target;
//        cout << game.board;
//        for (auto r : distances) {
//            for (int i : r)
//                cout << setw(3) << i;
//            cout << endl;
//        }
        return target;
    }

    Pos getFirstReachable(const set<Pos> &reachables, const auto & distance_matrix) {
        vector<int> distances;
        transform(reachables.begin(), reachables.end(), back_inserter(distances), [ distance_matrix ](Pos p) {
            return distance_matrix[p.y][p.x];
        });
        int min_distance = *min_element(distances.begin(), distances.end());
        set<Pos> closed;
        copy_if(reachables.begin(), reachables.end(), inserter(closed, closed.end()), [min_distance, distance_matrix](Pos p) {
            return distance_matrix[p.y][p.x] == min_distance;
        });
        Pos target = *closed.begin();
        game.board[target.y][target.x] = Tile::Target;
        //cout << game.board;
        return target;
    }

    bool hasOpponent() {
        bool has_opponent = false;
        for (auto p : puppet.pos.getAdjacents(game.board)) {
            auto it = game.puppets.find(p);
            if (it != game.puppets.end()) {
                if (it->second.type == puppet.getOpposite())
                    return true;
            }
        }
        return false;
    }

    Puppet *findOpponent() {
        Puppet *opponent = &puppet;
        for (auto p : puppet.pos.getAdjacents(game.board)) {
            auto it = game.puppets.find(p);
            if (it != game.puppets.end()) {
                Puppet &next = it->second;
                if (next.type == puppet.getOpposite() && (next.hp < opponent->hp || *opponent == puppet)) {
                    opponent = &next;
                }
            }
        }
        return opponent;
    }

    void attack() {
        Puppet *opponent = findOpponent();
        if (!opponent->pos.isInvalid()) {
            opponent->hp -= puppet.getAttackPower();
            if (opponent->hp <= 0) {
                //Died
                if (opponent->type == Tile::Elv)
                    ++game.died_elves;
                game.board[opponent->pos.y][opponent->pos.x] = Tile::Cavern;
                game.puppets.erase(opponent->pos);
            }
        }
    }

    void step(Pos target) {
        game.board[puppet.pos.y][puppet.pos.x] = Tile::Cavern;
        puppet.pos = findStep(target);
        game.board[puppet.pos.y][puppet.pos.x] = puppet.type;
    }

    Pos findStep(Pos target) {
        auto distances = createDistanceM(target);
        Pos next_step;
        int distance = numeric_limits<int>::max();
//        for (auto r : distances) {
//            for (int i : r)
//                cout << setw(3) << i;
//            cout << endl;
//        }
        for (auto adjacent : puppet.pos.getAdjacents(game.board)) {
            int next_d = distances[adjacent.y][adjacent.x];
            if ( next_d != -1 && next_d < distance) {
                distance = next_d;
                next_step = adjacent;
            }
        }
        
        return next_step;
    }
    
    

};

class GameLogic {
public:
    Game game;
    Game save_game;

    GameLogic(Game _game) : game(_game), save_game(_game) {
    };

    bool isEnd(){
        int elv_count = 0, goblin_count = 0;
        for( auto p: game.puppets){
            if ( p.second.type == Tile::Elv)
                ++elv_count;
            else
                ++goblin_count;
        }
        return ( elv_count == 0 || goblin_count == 0);
    }
    
    bool tick() {
        auto puppets = game.puppets;
        bool has_turn= false;
        for (auto ppair : puppets) {
            auto &puppet = ppair.second;
            //Check if died
            if (game.puppets.find(puppet.pos) != game.puppets.end()) {
                PuppetLogic pl(puppet, game);
                has_turn = pl.turn();
                game = pl.getGame();
            }
//                cout << game;
//                char c;
//                cin >> noskipws>>c;
        }
        return !(isEnd()) || has_turn;
    }

    int iterate() {
        int count = 0;
        bool has_target = false;
        game = save_game;
        while (tick()) {
            ++count;
//                        cout << game;
//                        cout <<"Counter : "<<count<<endl;
//                        char c;
//                        cin >> noskipws>>c;
        }
        cout << game;
        cout << "Rounds:" << count << endl;
        int sum_hp = 0;
        for (auto p : game.puppets)
            sum_hp += p.second.hp;
        cout << "Sum of hp:" << sum_hp << endl;
        cout << "Solution : " << ((count) * sum_hp) << endl;
        return (count)*sum_hp;
    }

    int solution2() {
        int ap = 3;
        while (true) {
            game = save_game;
            Puppet::elv_attack_power = ap;
            iterate();
            std::cout << "Attack power: " << ap << endl;
            if (game.died_elves == 0)
                break;
            ++ap;
        }
    }
};

void unitTestImpl(string input, int solution) {
    Puppet::elv_attack_power = Puppet::gobling_attack_power;
    BoardT b;
    istringstream(input)>>b;
    Game game(b);
    cout << game;
    cout << endl;
    GameLogic logic(game);
    assert(logic.iterate() == solution);
    logic.solution2();
}

void unitTest() {
    unitTestImpl(ut::input1, ut::solution1);
    unitTestImpl(ut::input2, ut::solution2);
    unitTestImpl(ut::input3, ut::solution3);
    unitTestImpl(ut::input4, ut::solution4);
    unitTestImpl(ut::input5, ut::solution5);
    unitTestImpl(ut::input6, ut::solution6);
}

void getSolution() {
    Puppet::elv_attack_power = Puppet::gobling_attack_power;
    ifstream ifile("input.txt");
    BoardT b;
    ifile>>b;
    Game game(b);
    cout << game;
    cout << endl;
    GameLogic logic(game);
    cout << flush;
    logic.iterate();
    logic.solution2();
}

int Puppet::elv_attack_power = 3;

/*
 * 
 */
int main(int argc, char** argv) {
    unitTest();
    getSolution();
    return 0;
}

