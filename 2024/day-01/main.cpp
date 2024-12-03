#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // list storage
    vector<uint64_t> left;
    vector<uint64_t> right;
    unordered_map<uint64_t, uint64_t> rightSummary;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        string tmp;

        // assign line to buffer
        stringstream buffer(line);

        // read left value
        getline(buffer, tmp, ' ');
        uint64_t l = stoull(tmp);
        left.push_back(l);

        // read right value
        getline(buffer, tmp);
        uint64_t r = stoull(tmp);
        right.push_back(r);

        // count right value
        if (rightSummary.count(r) == 0) {
            rightSummary[r] = 1;
        } else {
            rightSummary.at(r) += 1;
        }
    }

    // sort vectors
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());

    uint64_t totalDistance = 0;
    uint64_t similarity = 0;

    // process vectors and map
    for (size_t i = 0; i < left.size(); i++) {
        // retrieve short-hands
        uint64_t& l = left.at(i);
        uint64_t& r = right.at(i);

        totalDistance += max(l, r) - min(l, r);

        if (rightSummary.count(l) != 0) {
            similarity += rightSummary.at(l) * l;
        }
    }

    cout << "[first] total distance: " << totalDistance << endl;
    cout << "[second] similarity: " << similarity << endl;

    return 0;
}
