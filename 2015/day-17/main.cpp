#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // list of containers
    vector<int64_t> containers;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        int64_t container = stoll(line);

        containers.push_back(container);
    }

    // setup for iteration
    const int64_t NUMBER_OF_CONTAINERS = static_cast<int64_t>(containers.size());
    const int64_t MAX_COMBINATION = (1 << NUMBER_OF_CONTAINERS);
    const int64_t TOTAL_CAPACITY = 150;
    int64_t numberOfMatches = 0;

    unordered_map<int64_t, int64_t> specificMatches;

    // iterate over possible combinations
    for (int64_t combination = 1; combination < MAX_COMBINATION; combination++) {
        // temporary storage
        int64_t remainder = combination;
        int64_t capacity = 0;
        int64_t used = 0;

        // iterate over containers
        for (const int64_t& container : containers) {
            // check for flag
            if (remainder % 2 == 1) {
                capacity += container;
                used += 1;
            }

            // divide remainder
            remainder /= 2;
        }

        // check for valid capacity
        if (capacity == TOTAL_CAPACITY) {
            numberOfMatches += 1;

            // mark specific matches
            if (specificMatches.count(used) != 0) {
                specificMatches.at(used) += 1;
            } else {
                specificMatches[used] = 1;
            }
        }
    }

    // display results
    cout << "[first] possible matches to handle the eggnog : " << numberOfMatches << endl;
    for (int64_t i = 1; i < NUMBER_OF_CONTAINERS; i++) {
        // check for first specific match
        if (specificMatches.count(i) != 0) {
            cout << "[second] minimum sized matches to handle the eggnog : " << specificMatches.at(i) << endl;
            break;
        }
    }

    return 0;
}
