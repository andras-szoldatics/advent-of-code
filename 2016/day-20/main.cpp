// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>

using namespace std;

typedef pair<uint32_t, uint32_t> Range;

int main() {
    // temporary storage
    string line, tmp;

    // setup ranges
    vector<Range> ranges;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        Range range;
        // parse interval start
        getline(buffer, tmp, '-');
        range.first = stoul(tmp);

        // parse interval end
        getline(buffer, tmp);
        range.second = stoul(tmp);

        // add to vectors
        ranges.push_back(range);
    }

    // sort range
    sort(ranges.begin(), ranges.end());

    // setup for iteration
    const uint32_t LIMIT = numeric_limits<uint32_t>::max();
    uint32_t cursor = 0;
    size_t index = 0;

    uint32_t firstResult = LIMIT;
    size_t numberOfResults = 0;

    while (cursor < LIMIT) {
        // retrieve short-hands
        Range range = make_pair(LIMIT, 0u);
        if (index < ranges.size()) {
            range = ranges.at(index);
        }

        if (cursor >= range.first) {
            if (cursor <= range.second) {
                // assign new cursor position
                if (range.second == LIMIT) {
                    cursor = LIMIT;
                } else {
                    cursor = range.second + 1;
                }
            } else {
                // shift to next range
                index += 1;
            }
        } else {
            if (firstResult > cursor) {
                // we have our result, mark it
                firstResult = cursor;
            }

            // count results
            numberOfResults += (range.first - cursor);

            // move cursor
            cursor = range.first;
        }
    }

    // display results
    cout << "[first] lowest unblocked IP : " << firstResult << endl;
    cout << "[second] number of unblocked IPs : " << numberOfResults << endl;

    return 0;
}
