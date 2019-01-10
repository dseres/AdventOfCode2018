/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 3, 2018, 10:05 AM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <tuple>
#include <limits>
#include <algorithm>
#include <set>

using namespace std;

vector<string> readFile();
tuple<long, long> countChars(const vector<string> &codes);
vector<long> countChars(const string &code);
bool checkCount(const vector<long> &counts, long count);
set<string> findCloseCodes(const vector<string> &codes);
bool compareCode(const string &code1, const string &code2);
string findCommonLetters(const set<string> &codes);

/*
 * 
 */
int main(int argc, char** argv) {
    auto codes = readFile();
    for (auto code : codes) {
        cout << code << endl;
    }
    cout << codes.size() << endl;
    long twos = 0, threes = 0;
    tie(twos, threes) = countChars(codes);
    cout << "twos : " << twos << endl;
    cout << "threes : " << threes << endl;
    cout << "checksum : " << twos * threes << endl;
 
    auto closed_codes = findCloseCodes(codes);
    cout << "The following are closed codes:\n";
    for (auto ccode : closed_codes) {
        cout << ccode << endl;
    }
    string common_letters = findCommonLetters(closed_codes);
    cout<<"Common letters :\n"<<common_letters<<endl;
    return 0;
}

vector<string> readFile() {
    vector<string> codes;
    ifstream ifile("input.txt");
    for (string code; ifile >> code;) {
        codes.push_back(code);
    }
    return codes;
}

tuple<long, long> countChars(const vector<string> &codes) {
    long twos = 0, threes = 0;
    for (string code : codes) {
        vector<long> counts = countChars(code);
        if ( checkCount(counts, 2))
            ++twos;
        if ( checkCount(counts, 3))
            ++threes;    
    }
    return make_tuple(twos, threes);
}

vector<long> countChars(const string &code) {
    vector<long> counts(numeric_limits<unsigned char>::max(), 0);
    for (char c : code)
        ++counts[static_cast<unsigned char> (c)];
    return counts;
}

bool checkCount(const vector<long> &counts, long count) {
    return find(counts.begin(), counts.end(), count) != counts.end();
}

set<string> findCloseCodes(const vector<string> &codes) {
    set<string> closed_codes;
    for (string code1 : codes) {
        for (string code2 : codes) {
            if (compareCode(code1, code2)) {
                closed_codes.insert(code1);
                closed_codes.insert(code2);
            }
        }
    }
    return closed_codes;
}

bool compareCode(const string &code1, const string &code2) {
    string::size_type count_of_diffs = 0;
    for (int i = 0; i < code1.size(); ++i) {
        if (code1[i] != code2[i])
            ++count_of_diffs;
    }
    return count_of_diffs == 1;
}

string findCommonLetters(const set<string> &codes) {
    string s;
    for (int i = 0; i < codes.begin()->size(); ++i) {
        char c=codes.begin()->at(i);
        bool equal=true;
        for (auto code1 : codes) {
            equal = equal && (code1[i] == c);
        }
        if (equal)
            s.push_back(c);
    }
    return s;
}