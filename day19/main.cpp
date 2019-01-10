/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2018. december 23., 19:33
 */

#include <bits/stdc++.h>

using namespace std;

struct Device {
    typedef array<int, 6> RegisterT;

    struct Instruction {
        string op;
        int p1, p2, p3;

        friend istream &operator>>(istream &is, Instruction &inst) {
            string opcode;
            int p1, p2, p3;
            is >> inst.op >> inst.p1 >> inst.p2 >> inst.p3;
            return is;
        }
    };
    typedef function<void(Device &, int, int, int) > OperationT;
    int ip_reg = 0;
    RegisterT registers;
    map<string, OperationT> operations;
    vector<Instruction> instructions;

    Device(istream &is) {
        registers.fill(0);
        initOperations();
        readInstructions(is);
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

    void initOperations() {
#define ADD_OP(op) operations.insert( make_pair<string, OperationT>(string(#op), &Device::op))
        ADD_OP(addr);
        ADD_OP(addi);
        ADD_OP(mulr);
        ADD_OP(muli);
        ADD_OP(banr);
        ADD_OP(bani);
        ADD_OP(borr);
        ADD_OP(bori);
        ADD_OP(setr);
        ADD_OP(seti);
        ADD_OP(gtir);
        ADD_OP(gtri);
        ADD_OP(gtrr);
        ADD_OP(eqir);
        ADD_OP(eqri);
        ADD_OP(eqrr);
#undef ADD_OP
    }

    void readInstructions(istream &is) {
        //#ip 1
        readIp(is);
        Instruction it;
        while (is >> it) {
            instructions.push_back(it);
        }
    }

    void readIp(istream& is) {
        string opcode;
        int p1;
        is >> opcode>>p1;
        if (opcode == "#ip") {
            this->ip_reg = p1;
        } else {
            exit(-1);
            cerr << "Bad input file." << endl;
        }
    }

    int runCodes() {
        int ip = 0;
        ofstream ofile("trace2.txt");
        while( ip < instructions.size()){
            if ( ip == 1){
                int input_no = registers[4];
                return compute(input_no);
            }
            registers[ip_reg]=ip;
            Instruction inst = instructions[ip];
            ofile<<"Ip="<<ip<<" [";
            for(auto r: registers)
                ofile<<" "<<r;
            ofile<<"] "<<inst.op<<" "<<inst.p1<<" "<<inst.p2<<" "<<inst.p3<<" [";
            OperationT op = operations[inst.op];
            op(*this, inst.p1, inst.p2, inst.p3);
            for(auto r: registers)
                ofile<<" "<<r;
            ofile<<"]\n";
            ip = registers[ip_reg];
            ++ip;
        }
        cout<<"Registers :";
        for( auto r: registers)
            cout<<" "<<r;
        cout<<endl;        
        return registers[0];
    }
    
    int runCodes2(){
        registers.fill(0);
        registers[0]=1;
        return runCodes();
    }
    
    int compute(int number){
        auto dividers = factorize(number);
        for(auto p:dividers)
            cout<<" "<<p;
        cout<<endl;
        return accumulate(dividers.begin(), dividers.end(),0 );
    }

    vector<int> factorize(int number){
        vector<int> dividers;
        for( int i=1; i<=number;++i){
            if ( number / i *i == number){
                dividers.push_back(i);
            }
        }
        return dividers;
    }
       
};

int main(int argc, char** argv) {
    ifstream ifile("input.txt");
    Device dev(ifile);
    cout<<"Solution1 : "<<dev.runCodes()<<endl;
    cout<<"Solution2 : "<<dev.runCodes2()<<endl;
    return 0;
}

