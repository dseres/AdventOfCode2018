/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 9., 14:20
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <tuple>

using namespace std;

typedef pair<char, char> Edge;

struct Vertex {
    char label;
    int counter = 0;
    int time_left;
    int start_time;
    set<char> out_edges;
    set<char> in_edges;

    Vertex(char _label) : label(_label) {
        time_left = 60 + static_cast<int> (label - 'A' + 1);
    }
    Vertex(const Vertex &other) = default;
    Vertex(Vertex &&other) = default;
    Vertex &operator=(const Vertex &other) = default;
    Vertex &operator=(Vertex &&other) = default;

    friend bool operator<(const Vertex &a, const Vertex & b) {
        return a.label < b.label;
    }
};

class Graph {
private:
    typedef map<char, Vertex> VertexMap;
    typedef set<char> AvailablesT;
    typedef vector<char> SolutionT;
    typedef set<char> TasksT;

    int workers;
    int time_counter;
    VertexMap vertices;
    AvailablesT availables;
    SolutionT solution;
    TasksT active_tasks;


public:

    Graph(const vector<Edge> &edges, int _workers) : workers(_workers) {
        for (Edge e : edges) {
            insert(e);
        }
        time_counter = 0;
    }

    void compute() {
        findFirstAvailables();
        iterate();
        printSolution();
    }

    void printSolution() {
        cout << "Solution1 : ";
        for (auto c : solution) {
            cout << c;
        }
        cout << " Solution2 : " << time_counter;
        cout << endl;
    }
private:

    void insert(Edge e) {
        insertVertex(e.first);
        vertices.at(e.first).out_edges.insert(e.second);
        insertVertex(e.second);
        vertices.at(e.second).in_edges.insert(e.first);
    }

    void insertVertex(char label) {
        if (vertices.find(label) == vertices.end()) {
            Vertex v(label);
            vertices.insert(VertexMap::value_type{label, v});
        }
    }

    void findFirstAvailables() {
        for (auto &f : vertices) {
            if (f.second.in_edges.empty()) {
                availables.insert(f.second.label);
            }
        }
    }

    void iterate() {
        while (!availables.empty() || !active_tasks.empty()) {
            printTasks();
            schedule();
            char c = getNextFinished();
            solution.push_back(c);
            getNextAvailables(c);
        }
    }

    void printTasks() {
        cout << "Availables : ";
        for (auto a : availables) {
            cout << a;
        }
        cout << "\tActive tasks : ";
        for (auto a : active_tasks) {
            cout << a;
        }
        cout << endl;
    }

    char pop() {
        char f = *(availables.begin());
        availables.erase(availables.begin());
        return f;
    }

    bool compareTasks(char c, char d) {
        return vertices.at(c).time_left < vertices.at(d).time_left;
    }

    void schedule() {
        while (active_tasks.size() < workers && !availables.empty()) {
            char c = pop();
            active_tasks.insert(c);
        }
    }

    char getNextFinished() {
        while (true) {
            auto it = find_if(active_tasks.begin(), active_tasks.end(), [this](char c) {
                return this->vertices.at(c).time_left == 0;
            });
            if (it != active_tasks.end()) {
                char label = *it;
                active_tasks.erase(it);
                return label;
            } else {
                ++time_counter;
                for (char c : active_tasks)
                    --vertices.at(c).time_left;
            }
        }
    }

    void getNextAvailables(char c) {
        Vertex &v = vertices.at(c);
        for (auto n : v.out_edges) {
            if (checkAdjacent(n))
                availables.insert(n);
        }
    }

    bool checkAdjacent(char c) {
        Vertex &v = vertices.at(c);
        ++v.counter;
        int in_counter = v.in_edges.size();
        if (v.counter == in_counter) {
            //It is available
            return true;
        }
        return false;
    }
};

vector<Edge> readFile(ifstream& ifile) {
    vector<Edge> edges;
    for (string line; getline(ifile, line);) {
        static regex r(R"(^Step ([A-Z]) must be finished before step ([A-Z]) can begin.$)");
        smatch m;
        if (regex_match(line, m, r)) {
            char from = (m[1].str())[0];
            char to = (m[2].str())[0];
            //cout << from << "->" << to << endl;
            edges.push_back(Edge(from, to));
        } else {
            cerr << "Bad input record : " << line << endl;
            exit(-1);
        };
    }
    return move(edges);
}

void printElements(vector<Edge>& edges) {
    set<char> elements;
    for (auto e : edges) {
        elements.insert(e.first);
        elements.insert(e.second);
    }
    for (auto c : elements)
        cout << c << " ";
    cout << endl;
    cout << "Size of elements : " << elements.size() << endl;
    char minv = *min_element(elements.begin(), elements.end());
    char maxv = *max_element(elements.begin(), elements.end());
    cout << "Min element : " << minv << endl;
    cout << "Max element : " << maxv << endl;
}

/*
 * 
 */
int main(int argc, char** argv) {


    vector<Edge> testedges{
        Edge('C', 'A'),
        Edge('C', 'F'),
        Edge('A', 'B'),
        Edge('A', 'D'),
        Edge('B', 'E'),
        Edge('D', 'E'),
        Edge('F', 'E')};

    Graph g(testedges, 2);
    g.compute();
    g.printSolution();

    ifstream ifile("input.txt");
    vector<Edge> edges = readFile(ifile);
    printElements(edges);
    Graph g2(edges, 5);
    g2.compute();
    return 0;
}

