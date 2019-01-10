/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 10, 2018, 1:52 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <tuple>

using namespace std;

deque<int> readFile() {
    deque<int> ints;
    ifstream ifile("input.txt");
    for (int i; ifile >> i;) {
        ints.push_back(i);
    }
    return ints;
}

pair<int, int> getChecksum(ifstream &ifile) {
    int checksum = 0, value=0, nodes = 0, data_count = 0;
    ifile >> nodes;
    vector<int> node_values(nodes,0);
    ifile>>data_count;
    for (int i = 0; i < nodes; ++i) {
        int node_checksum, node_value;
        tie(node_checksum, node_value) = getChecksum(ifile);
        checksum += node_checksum;
        node_values[i]=node_value;
    }
    for (int i = 0; i < data_count; ++i) {
        int ind;
        ifile>>ind;
        checksum += ind;
        if ( nodes == 0)
        {
            value+=ind;
        }
        else if ( ind <= nodes )
        {
            value+=node_values[ind-1];
        }
    }
    return make_pair(checksum,value);
}

/*
 * 
 */
int main(int argc, char** argv) {
//    auto ints = readFile();
//    for (int i : ints)
//        cout << " " << i;
//    cout << endl;
//    cout << "Size if ints : " << ints.size() << endl;
    ifstream ifile("input.txt");
    int checksum, value;
    tie(checksum, value) = getChecksum(ifile);
    cout << "Solution1 : " << checksum  << endl;
    cout << "Solution2 : " << value  << endl;

    return 0;
}
