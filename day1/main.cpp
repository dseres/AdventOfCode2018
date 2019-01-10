#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <numeric>

using namespace std;

vector<long> read_freqs();
void search_number(const vector<long> &numbers);

int main()
{
    vector<long> numbers = read_freqs();
    search_number(numbers);
    return 0;
}

vector<long> read_freqs()
{
    vector<long> numbers;
    ifstream f("input.txt");
    for(long l=0; f>>l;)
    {
        numbers.push_back(l);
        cout<<l<<"\n";
    }
    return numbers;
}

void search_number(const vector<long> &numbers)
{
    long sum_of_numbers= accumulate(numbers.begin(), numbers.end(), 0);
    cout<<"sum_of_numbers:"<<sum_of_numbers<<endl;
    set<long> sums;
    bool found=false;
    long sum=0,iteration=0;
    while(!found)
    {
        for(long l:numbers)
        {
            sum+=l;
            ++iteration;
            if (sums.find(sum) != sums.end())
            {
                cout<<"The folling state was repeated first: "<<sum<<" in iteration "<<iteration<<endl;
                found=true;
                break;
            }
            sums.insert(sum);
        }
    }
}
