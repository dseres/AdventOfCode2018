/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: DSeres
 *
 * Created on December 4, 2018, 9:06 AM
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>
#include <regex>
#include <tuple>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

struct Log
{

    enum Action
    {
        SHIFT, FALL_ASLEEP, WAKE_UP
    };

    boost::posix_time::ptime time;
    int id{};
    Action action;

    int getMinute()
    {
        return static_cast<int>(time.time_of_day().minutes());
    }

    friend bool operator<(const Log &l1, const Log &l2)
    {
        return l1.time < l2.time;
    }

    friend istream& readLog(istream &is, Log &l)
    {
        string line;
        if ( getline(is, line))
        {
            static regex r(R"(^\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2})\] (falls asleep|wakes up|Guard #(\d+) begins shift) *$)");
            smatch m;
            if (regex_match(line, m, r))
            {
                l.time = boost::posix_time::time_from_string(m[1]);
                if (m[2] == "wakes up")
                {
                    l.action = Log::WAKE_UP;
                }
                else if (m[2] == "falls asleep")
                {
                    l.action = Log::FALL_ASLEEP;
                }
                else
                {
                    l.action = Log::SHIFT;
                    l.id = std::stoi(m[3]);
                }
            }
            else
            {
                cerr << "Bad match.\nLine : " << line << "\n";
                exit(-1);
            }
        }
        return is;
    }

    friend ostream &operator<<(ostream &os, const Log &l)
    {
        os << "Log{time : " << boost::posix_time::to_simple_string(l.time) << "; id : " << l.id << "; action : ";
        switch(l.action)
        {
        case Log::SHIFT:
            os<<"shift";
            break;
        case Log::FALL_ASLEEP:
            os<<"fall_asleep";
            break;
        case Log::WAKE_UP:
        default:
            os<<"wake_up";
            break;
        }
        os<< "}";
        return os;
    }
};

struct Guard
{
    int id;
    int sum_mins;
    vector<int> mins_stat;

    Guard(): Guard(0){};
    Guard(int _id): id(_id), sum_mins(0)
    {
        mins_stat.resize(60,0);
    };

    void add_period(int start_minute, int end_minute)
    {
        sum_mins+=end_minute - start_minute;
        for(int i=start_minute; i<end_minute; ++i)
            ++(mins_stat[i]);
    };

    tuple<int,int> findMostSleptMin() const
    {
        auto it = max_element(mins_stat.begin(), mins_stat.end());
        int minute = distance(mins_stat.begin(),it);
        int min_count = mins_stat[minute];
        return make_tuple(minute,min_count);
    };
};

typedef vector<Guard> Guards;

set<Log> readFile();
Guards computeSleep_Times(const set<Log> &logs);
void findFirstMax(const Guards &guards);
void findSecondMax(const Guards &guards);

/*
 *
 */
int main(int argc, char** argv)
{
    auto logs = readFile();
    cout << "There are " << logs.size() << " logs.\n";
    auto guards = computeSleep_Times(logs);
    /*for(auto g: guards)
    {
        cout<<"Id : "<<g.id<<" all minutes : "<<g.sum_mins<<"\n";
    }*/
    findFirstMax(guards);
    findSecondMax(guards);
    return 0;
}

set<Log> readFile()
{
    ifstream ifile("input.txt");
    set<Log> logs;
    for(Log l; readLog(ifile, l); )
    {
        //cout << l << "\n";
        logs.insert(move(l));
        l= {};
    }
    return logs;
}

Guards computeSleep_Times(const set<Log> &logs)
{
    map<int, Guard> guards;
    int last_id, start_min;
    for(Log l:logs)
    {
        if ( l.id != 0)
        {
            last_id = l.id;
        }
        if ( l.action == Log::FALL_ASLEEP)
        {
            start_min=l.getMinute();
        }
        else if ( l.action == Log::WAKE_UP)
        {
            int end_min = l.getMinute();
            if ( guards.find(last_id) == guards.end())
                guards[last_id]=Guard(last_id);
            ;
            guards[last_id].add_period(start_min,end_min);
        }
    }
    Guards ret;
    transform(guards.cbegin(), guards.cend(), back_inserter(ret), [](auto it){return it.second;});
    return ret;
}


void findFirstMax(const Guards &guards)
{
    const Guard &g=*max_element(guards.begin(), guards.end(), [](Guard g1, Guard g2) {return g1.sum_mins<g2.sum_mins; });
    int minute,min_count;
    tie(minute,min_count)=g.findMostSleptMin();
    cout<<"Max sleep time was "<<g.sum_mins<<" minutes with id "<<g.id<<"\n";
    cout<<"Most slept minute :"<<minute<<"\n";
    cout<<"Solution 1: "<<minute*(g.id)<<"\n";
}


void findSecondMax(const Guards &guards)
{

    const Guard &g=*max_element(guards.begin(), guards.end(), [](Guard g1, Guard g2)
    {
        int min1,count1,min2,count2;
        tie(min1,count1)=g1.findMostSleptMin();
        tie(min2,count2)=g2.findMostSleptMin();
        return count1 < count2;
    });
    int minute,count;
    tie(minute,count) = g.findMostSleptMin();
    cout<<"Id "<<g.id<<"\n";
    cout<<"Most slept minute :"<<minute<<"\n";
    cout<<"Count :"<<count<<"\n";
    cout<<"Solution 2: "<<(g.id)*(minute)<<"\n";

}
