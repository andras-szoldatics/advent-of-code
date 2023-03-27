// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line, tmp;

    // distance matrix
    unordered_map<string, unordered_map<string, int64_t>> distances;
    unordered_set<string> places;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // parse first place
        string from;
        getline(buffer, from, ' ');

        // skip next word
        getline(buffer, tmp, ' ');

        // parse second place
        string to;
        getline(buffer, to, ' ');

        // skip next word
        getline(buffer, tmp, ' ');

        // parse distance
        getline(buffer, tmp, ' ');
        int64_t distance = stoll(tmp);

        if (places.count(from) == 0) {
            // fill out vector of places
            places.insert(from);
        }

        if (places.count(to) == 0) {
            // fill out vector of places
            places.insert(to);
        }

        // assign defined value forward & backward
        distances[from][to] = distance;
        distances[to][from] = distance;
    }

    // keep a running tally
    int64_t shortest = numeric_limits<int64_t>::max();
    int64_t longest = numeric_limits<int64_t>::min();

    // setup for iteration
    vector<string> currentOrder(places.begin(), places.end());
    sort(currentOrder.begin(), currentOrder.end());
    bool hasNext = true;

    while (hasNext) {
        // temporary storage
        int64_t length = 0;

        // evaluate current route
        for (size_t i = 1; i < currentOrder.size(); i++) {
            // retrieve short-hands
            const string& past = currentOrder.at(i - 1);
            const string& present = currentOrder.at(i);

            // add distance
            length += distances.at(past).at(present);
        }

        if (shortest > length) {
            // set shortest length
            shortest = length;
        }

        if (longest < length) {
            // set longest length
            longest = length;
        }

        // retrieve next permutation
        hasNext = next_permutation(currentOrder.begin(), currentOrder.end());
    }

    // display results
    cout << "[first] shortest route : " << shortest << endl;
    cout << "[second] longest route : " << longest << endl;

    return 0;
}
