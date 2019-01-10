/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 20., 21:27
 */

#include <bits/stdc++.h>

using namespace std;

typedef array<int, 4> RegisterT;
typedef array<int, 4> InstrunctionT;

struct State {
    RegisterT before{};
    InstrunctionT instruction{};
    RegisterT after{};

    friend istream &operator>>(istream &strm, State &s) {
        string tmp;
        char c;
        strm >> tmp>>c;
        if (strm) {
            for (int i = 0; i < 4; ++i)
                strm >> s.before[i] >> c;
            for (int i = 0; i < 4; ++i)
                strm >> s.instruction[i];
            strm >> tmp>>c;
            for (int i = 0; i < 4; ++i)
                strm >> s.after[i] >> c;
        }
        return strm;
    }
};

struct Device {
    RegisterT registers;
    typedef function<void(Device &, int, int, int) > OperationT;
    vector<OperationT> operations;
    int no_of_operations;
    map<int, OperationT> opcodes;

    Device() {
        registers.fill(0);
        operations.push_back(&Device::addr);
        operations.push_back(&Device::addi);
        operations.push_back(&Device::mulr);
        operations.push_back(&Device::muli);
        operations.push_back(&Device::banr);
        operations.push_back(&Device::bani);
        operations.push_back(&Device::borr);
        operations.push_back(&Device::bori);
        operations.push_back(&Device::setr);
        operations.push_back(&Device::seti);
        operations.push_back(&Device::gtir);
        operations.push_back(&Device::gtri);
        operations.push_back(&Device::gtrr);
        operations.push_back(&Device::eqir);
        operations.push_back(&Device::eqri);
        operations.push_back(&Device::eqrr);
        no_of_operations = operations.size();
    }

    void addr(int r1, int r2, int r3) {
        registers[r3] = registers[r1] + registers[r2];
    }

    void addi(int r1, int v2, int r3) {
        registers[r3] = registers[r1] + v2;
    }

    void mulr(int r1, int r2, int r3) {
        registers[r3] = registers[r1] * registers[r2];
    }

    void muli(int r1, int v2, int r3) {
        registers[r3] = registers[r1] * v2;
    }

    void banr(int r1, int r2, int r3) {
        registers[r3] = registers[r1] & registers[r2];
    }

    void bani(int r1, int v2, int r3) {
        registers[r3] = registers[r1] & v2;
    }

    void borr(int r1, int r2, int r3) {
        registers[r3] = registers[r1] | registers[r2];
    }

    void bori(int r1, int v2, int r3) {
        registers[r3] = registers[r1] | v2;
    }

    void setr(int r1, int, int r3) {
        registers[r3] = registers[r1];
    }

    void seti(int v1, int, int r3) {
        registers[r3] = v1;
    }

    void gtir(int v1, int r2, int r3) {
        registers[r3] = (registers[r2] < v1 ? 1 : 0);
    }

    void gtri(int r1, int v2, int r3) {
        registers[r3] = (v2 < registers[r1] ? 1 : 0);
    }

    void gtrr(int r1, int r2, int r3) {
        registers[r3] = (registers[r2] < registers[r1] ? 1 : 0);
    }

    void eqir(int v1, int r2, int r3) {
        registers[r3] = (registers[r2] == v1 ? 1 : 0);
    }

    void eqri(int r1, int v2, int r3) {
        registers[r3] = (v2 == registers[r1] ? 1 : 0);
    }

    void eqrr(int r1, int r2, int r3) {
        registers[r3] = (registers[r2] == registers[r1] ? 1 : 0);
    }

    vector<OperationT> match(const State &state) {
        vector<OperationT> ops;
        for (auto op : operations) {
            registers = state.before;
            op(*this, state.instruction[1], state.instruction[2], state.instruction[3]);
            if (state.after == registers) {
                ops.push_back(op);
            }
        }
        return ops;
    }

    int countMatches(const vector<State> &states) {
        auto &d = *this;
        count_if(states.begin(), states.end(), [&d](auto st) {
            auto m = d.match(st);
            return m.size() >= 3;
        });
    }

    vector<OperationT> non_match(const State &state) {
        vector<OperationT> ops;
        for (auto op : operations) {
            registers = state.before;
            op(*this, state.instruction[1], state.instruction[2], state.instruction[3]);
            if (state.after != registers) {
                ops.push_back(op);
            }
        }
        return ops;
    }

    void stipulate(const vector<State> &states) {
        for (auto st : states) {
            if (opcodes.find(st.instruction[0]) == opcodes.end()) {
                auto m = match(st);
                if (m.size() == 1) {
                    //Found one.
                    auto op = *(m.begin());
                    opcodes.insert(make_pair(st.instruction[0], op));
                    operations = non_match(st);
                }
            }
        }
        if (opcodes.size() == no_of_operations)
            return;
        else stipulate(states);
    }

    int runCodes(ifstream &is) {
        registers.fill(0);
        while (is) {
            InstrunctionT inst;
            for (auto &i : inst)
                is >> i;
            if (is) {
                (opcodes[inst[0]])(*this, inst[1], inst[2], inst[3]);
            }
        }
        return registers[0];
    }
};

auto readStates() {
    vector<State> states;
    ifstream ifile("input.txt");
    State state;
    while (ifile >> state) {
        states.push_back(state);
    }
    return states;
}

/*
 * 
 */
int main(int argc, char** argv) {
    vector<State> states = readStates();
    cout << states.size() << endl;
    Device d;
    int count = d.countMatches(states);
    cout << "Samples in my puzzle input behave like three or more opcodes : " << count << endl;
    d.stipulate(states);
    ifstream ifile("input2.txt");
    int result = d.runCodes(ifile);
    cout << "Value is contained in register 0 after executing the test program : " << result << endl;

    return 0;
}

