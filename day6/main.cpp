#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <utility>
#include <iomanip>
#include <set>
#include <array>
#include <cassert>

using namespace std;

class Picture {
public:
    typedef const vector<pair<int, int>> CoordinatesT;
    typedef vector<vector<int>> PictureT;

    CoordinatesT coords;
    PictureT pic;
    PictureT pic_diff;

    Picture(const CoordinatesT &_coords) : coords(_coords) {
        init();
        computePoints();
    };


    void displayInLinuxTerm() {
        for (unsigned int x = 0; x < pic.size(); ++x) {
            for (unsigned int y = 0; y < (pic)[x].size(); ++y) {
                int i = pic[y][x];
                cout << "\033[48;5;" << i << "m" << (i > 0 ? 'X' : ' ') << "\033[m";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    int getSolution1() {
        vector<int> sizes(coords.size() + 1, 0);
        for (auto v : pic) {
            for (int i : v) {
                ++sizes[i];
            }
        }
        for (int i : *pic.begin()) {
            sizes[i] = 0;
        }
        for (int i : *(--pic.end())) {
            sizes[i] = 0;
        }
        for (auto v : pic) {
            sizes[*v.begin()] = 0;
            sizes[*(--v.end())] = 0;
        }
        int max_size = *max_element(sizes.begin(), sizes.end());
        return max_size;
    }

    int getSolution2(int limit = 10000) {
        int count = 0;
        for (unsigned int x = 0; x < pic.size(); ++x) {
            for (unsigned int y = 0; y < pic[x].size(); ++y) {
                int sum_dist = 0;
                for (auto p : coords)
                    sum_dist += std::abs(static_cast<int> (x) - p.first) + std::abs(static_cast<int> (y) - p.second);
                if (sum_dist < limit)
                    ++count;
            }
        }
        return count;
    }

private:

    void init() {
        auto p1 = max_element(coords.begin(), coords.end(), [](auto p, auto q) {
            return p.first < q.first;
        });
        auto p2 = max_element(coords.begin(), coords.end(), [](auto p, auto q) {
            return p.second < q.second;
        });
        int size = max(p1->first, p2->second) + 1;
        cout << "Size of map : " << size << "\n";
        pic = PictureT(size, vector<int>(size, 0));
        pic_diff = pic;

    }

    void computePoints() {
        for (unsigned int x = 0; x < pic.size(); ++x) {
            for (unsigned int y = 0; y < pic[x].size(); ++y) {
                //if ( pic[x][y] == 0)
                {
                    vector<int> dists(coords.size(), 0);
                    for (unsigned int i = 0; i < coords.size(); ++i) {
                        dists[i] = std::abs(static_cast<int> (x) - coords[i].first) + std::abs(static_cast<int> (y) - coords[i].second);
                    }
                    auto m1 = min_element(dists.begin(), dists.end());
                    auto m2 = min_element(m1 + 1, dists.end());
                    if (m2 == dists.end() || (m2 != dists.end() && *m1 < *m2)) {
                        pic[x][y] = 1 + distance(dists.begin(), m1);
                    }
                }
            }
        }
    }

};

Picture::CoordinatesT readFile() {
    char c;
    int x, y;
    vector<pair<int, int>> coordinates;
    ifstream ifile("input.txt");
    while (ifile >> x >> c >> y) {
        coordinates.push_back(make_pair(x, y));
    }
    return coordinates;
};

void unitTest() {
    Picture::CoordinatesT coordinates = {
        {1, 1},
        {1, 6},
        {8, 3},
        {3, 4},
        {5, 5},
        {8, 9}};
    Picture p = coordinates;
    p.displayInLinuxTerm();
    assert(p.getSolution1() == 17);
    assert(p.getSolution2() == 100);
}

int main() {
    unitTest();

    auto coordinates = readFile();
    for (auto c : coordinates)
        cout << c.first << " " << c.second << "\n";
    cout << "Coordinates count: " << coordinates.size() << "\n";

    Picture p = coordinates;
    p.displayInLinuxTerm();
    cout << "Solution 1: " << p.getSolution1() << endl;
    cout << "Solution 2: " << p.getSolution2() << endl;

    return 0;
}


