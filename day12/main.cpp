/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 12, 2018, 10:12 AM
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <deque>
#include <array>
#include <string>
#include <locale>
#include <cassert>
#include <algorithm>


using namespace std;

class customBoolNames : public std::numpunct_byname<char> {
public:

    customBoolNames(const std::string& name)
    : std::numpunct_byname<char>(name) {
    }
protected:

    virtual std::string do_truename() const {
        return "#";
    }

    virtual std::string do_falsename() const {
        return ".";
    }
};

string test_input = R"(#..#.#..##......###...###)";
string test_rules = R"(...## => #
..#.. => #
.#... => #
.#.#. => #
.#.## => #
.##.. => #
.#### => #
#.#.# => #
#.### => #
##.#. => #
##.## => #
###.. => #
###.# => #
####. => #)";

string input = R"(##.#..########..##..#..##.....##..###.####.###.##.###...###.##..#.##...#.#.#...###..###.###.#.#)";
string rules = R"(####. => #
##.#. => .
.##.# => .
..##. => .
..... => .
.#.#. => #
.###. => .
.#.## => .
#.#.# => .
.#... => #
#..#. => #
....# => .
###.. => .
##..# => #
#..## => #
..#.. => .
##### => .
.#### => #
#.##. => #
#.### => #
...#. => .
###.# => .
#.#.. => #
##... => #
...## => #
.#..# => .
#.... => .
#...# => .
.##.. => #
..### => .
##.## => .
..#.# => #)";

class AocLifeGame {
public:
    static constexpr int RuleSize = 5;
    typedef deque<bool> BitSet;
    typedef array<bool, RuleSize> RuleT;
    typedef vector<RuleT> RuleContainer;

    BitSet bits;
    int padding = 0;
    RuleContainer rules;

    AocLifeGame(const string &input, const string &str_rules) {
        readInput(input);
        readRules(str_rules);
        printInputs();
    }

    int getSolution1(int iterations = 20) {

        printBitset(0, "Input         ");
        for (int i = 0; i < iterations; ++i) {
            next();
            printBitset(i + 1);
        }
        return computeSum();
    }

    long long getSolution2(long long iterations = 50000000000)
    {
        int it_no=20;
        bool equal = false;
        int sum_prev = 0;
        while(it_no<200){
            printBitset(++it_no);
            BitSet prev=bits;
            sum_prev = computeSum();
            next();
            continue;
            auto first_prev = find(prev.begin(), prev.end(), true);
            auto first_curr = find(bits.begin(), bits.end(), true);
            auto last_prev = find(prev.rbegin(), prev.rend(), true);
            auto last_curr = find(bits.rbegin(), bits.rend(), true);
            BitSet::size_type s_prev = distance(prev.begin(), first_prev);
            BitSet::size_type s_curr = distance(bits.begin(), first_curr);
            BitSet::size_type e_prev = prev.size() - distance(prev.rbegin(),++last_prev);
            BitSet::size_type e_curr = bits.size() - distance(bits.rbegin(),++last_curr);
            if ( e_prev - s_prev == e_curr - s_curr)
            {
                equal = true;
                for(int j=0; j<e_prev - s_prev; ++j){
                    equal = equal && ( prev[s_prev + j] == bits[s_curr + j] );
                }
            }
        }
        long long cum_curr = computeSum();
        long long valut_at_it = ( iterations -it_no  )* (cum_curr - sum_prev) + cum_curr;
        return valut_at_it;        return valut_at_it;
    }
    
private:

    void readInput(const string& input) {
        istringstream is(input);
        is.imbue(cout.getloc());
        is>>boolalpha;
        bool b;
        while (is >> b) {
            bits.push_back(b);
        }
    }

    void readRules(const string& str) {
        istringstream is(str);
        is.imbue(cout.getloc());
        is>>boolalpha;
        while (is) {
            RuleT rule;
            for (int i = 0; i < rule.size(); ++i)
                is >> rule[i];
            string s;
            bool b;
            is >> s>>b;
            if (b) {
                rules.push_back(move(rule));
            }
        }
    }

    void printInputs() {
        cout << "Input: ";
        for (auto b : bits)
            cout << b;
        cout << endl;
        cout << "Rules: " << endl;
        for (auto r : rules) {
            for (auto b : r)
                cout << b;
            cout << " => #" << endl;
        }
    }

    void printBitset(int i, string label = "Next iteration") {
        auto first = find(bits.begin(), bits.end(), true);
        auto last = find(bits.rbegin(), bits.rend(), true);
        ++last;
        BitSet::size_type d2 = distance(bits.begin(), first);
        BitSet::size_type d = distance(bits.rbegin(),last);
        cout << label << setw(5)<<i<<"(from "<<setw(5)<<(d2+padding)<<")"<<" : ";
        //cout << string((Iterations - i)*4, ' ');
        for(int i=d2; i<= bits.size()-d;++i)
            cout<<bits[i];
        cout << " Sum : "<<computeSum()<<endl;
    }

    void next() {
        BitSet ibits = bits;
        for (int j = 0; j < RuleSize - 1; ++j) {
            ibits.push_front(false);
            ibits.push_back(false);
        }
        padding -= (RuleSize - 1);

        BitSet nbits = BitSet(ibits.size(), false);
        for (int i = RuleSize / 2; i < ibits.size() - RuleSize / 2; ++i) {
            for (RuleT r : rules) {
                //Matching rule
                bool m = true;
                for (int j = 0; j < r.max_size(); ++j) {
                    m = m && (ibits[i - 2 + j] == r[j]);
                }
                //If matched new value
                if (m) {
                    nbits[i] = true;
                }
            }
        }
        bits = move(nbits);
    }

    int computeSum() {
        long long sum = 0, value = padding;
        for (bool b : bits) {
            if (b)
                sum += value;
            ++value;
        }
        return sum;
    }


};

void unit_test() {
    AocLifeGame g(test_input, test_rules);
    int s1 = g.getSolution1();
    cout << "Test solution : " << s1 << endl;
    assert(s1 == 325);
}

int main(int argc, char** argv) {
    std::locale loc(std::locale("C"), new customBoolNames(""));
    cout.imbue(loc);
    cout << boolalpha;

    unit_test();

    AocLifeGame g(input, rules);
    int s1 = g.getSolution1();
    cout << "Solution1 : " << s1 << endl;
    cout << "Solution2 : " << g.getSolution2() <<endl;
    return 0;
}

