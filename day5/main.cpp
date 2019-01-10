#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <vector>
#include <locale>

using namespace std;

list<char> readFile();
list<char> react(list<char> poly);
bool reactable(const char &c1, const char &c2);
size_t removeAndReact(list<char> poly, const char c);
list<char> remove(list<char> poly, const char c);
void searchReducedSizes(list<char> poly);

int main()
{
    auto poly=readFile();
    //for(char c:poly)
    //    cout<<c;
    //cout<<"Polymer length : "<<poly.size()<<"\n";
    poly = react(move(poly));
    cout<<"Polymer length : "<<poly.size()<<"\n";
    searchReducedSizes(move(poly));
}

list<char> readFile()
{
    list<char> polymer;
    ifstream ifile("input.txt");
    for(char c; ifile>>c; )
    {
        polymer.push_back(c);
    }
    return polymer;
}

list<char> react(list<char> poly)
{
    for(auto it=poly.begin(); it!=poly.end();)
    {
        char c1 = *it++;
        if ( it != poly.end())
        {
            char c2 = *it;
            if ( reactable(c1,c2))
            {
                --it;
                it = poly.erase(it);
                it = poly.erase(it);
                if ( it != poly.begin())
                    --it; //Positioning iterator to check the char before ereasing
            }
        }
    }
    return poly;
}

bool reactable(const char &c1, const char &c2)
{
    return
           ( c1 == tolower(c1) && c2 == toupper(c1) )//aA
        || ( c1 == toupper(c1) && c2 == tolower(c1) )//Aa
        ;
}

void searchReducedSizes(list<char> poly)
{
    vector< size_t> sizes;
    for(char c='a'; c<='z'; ++c)
    {
        size_t s=removeAndReact(poly,c);
        sizes.push_back( s);
        //cout<<"Size after reducing "<<c<<" :"<<s<<"\n";
    }
    int min_ind = distance(sizes.begin(),min_element(sizes.begin(),sizes.end()));
    char minc='a' + min_ind;
    cout<<"Minimum : "<<minc<<", minimal size : "<<sizes[min_ind]<<"\n";
}

size_t removeAndReact(list<char> poly, const char c)
{
    poly = remove(move(poly),c);
    poly = react(move(poly));
    return poly.size();
}

list<char> remove(list<char> poly, const char c)
{
    for(auto it=poly.begin(); it!=poly.end();)
        if ( *it == c || *it == toupper(c))
            it = poly.erase(it);
        else
            ++it;
    return move(poly);
}
