/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 14, 2018, 10:05 AM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>
using namespace std;

struct Kitchen {
    typedef vector<unsigned int> RecipesT;
    typedef RecipesT::size_type SizeT;
    typedef unsigned long long CounterT;
    RecipesT recipes{3, 7};
    SizeT e1 = 0;
    SizeT e2 = 1;
    CounterT counter = 0;

    friend ostream& operator<<(ostream &os, const Kitchen &k) {
        os << k.counter << " : ";
        for (int i = 0; i < k.recipes.size(); ++i) {
            if (i == k.e1)
                os << "(" << k.recipes[i] << ")";
            else if (i == k.e2)
                os << "[" << k.recipes[i] << "]";
            else
                os << " " << k.recipes[i] << "";
        }
        os << endl;
        return os;
    }

    void tick() {
        unsigned int nr = recipes[e1] + recipes[e2];
        if (nr >= 10)
            recipes.push_back(nr / 10);
        recipes.push_back(nr % 10);
        stepElv(&e1);
        stepElv(&e2);
        ++counter;
    }

    void stepElv(SizeT *e) {
        *e = (*e + 1 + recipes[*e]) % recipes.size();
    }

    string getScores(const SizeT recipe_no, const SizeT length = 10) {
        while (recipes.size() < recipe_no + length) {
            tick();
        }
        RecipesT score(recipes.begin() + recipe_no, recipes.begin() + recipe_no + length);
        ostringstream os;
        for (int i : score)
            os << i;
        return os.str();
    }

    RecipesT toRecipes(const string& recipe_no){
        RecipesT pattern;
        for (char c : recipe_no) {
            pattern.push_back(stoi(string() + c));
        }
        return pattern;
    }

    SizeT findRecipes(const string recipe_no) {
        RecipesT pattern = toRecipes(recipe_no);
        auto it = search(recipes.begin(), recipes.end(), pattern.begin(), pattern.end());
        if (it != recipes.end())
            return distance(recipes.begin(), it);
        while (true) {
            tick();
            auto it = search(recipes.end() - pattern.size() - 2, recipes.end(), pattern.begin(), pattern.end());
            if (it != recipes.end())
                return distance(recipes.begin(), it);
        }
        return 0;
    }
};

void unitTest() {
    Kitchen k;
    assert(k.getScores(5) == "0124515891");
    assert(k.getScores(9) == "5158916779");
    assert(k.getScores(18) == "9251071085");
    //assert( k.getScores(2018) == "5941429882");

    assert(k.findRecipes("01245") == 5);
    assert(k.findRecipes("51589") == 9);
    assert(k.findRecipes("92510") == 18);
    assert(k.findRecipes("59414") == 2018);
}

int main(int argc, char** argv) {
    unitTest();

    Kitchen k;
    cout << k.getScores(825401) << endl;
    cout<<k.findRecipes("825401")<<endl;
    return 0;
}

