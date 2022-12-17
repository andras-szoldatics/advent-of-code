// C++ standard includes
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // running tally
    vector<int64_t> changes;
    unordered_set<int64_t> reachedFrequencies;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        int64_t change = stoll(line);
        changes.push_back(change);
    }

    // list of second visits
    int64_t afterIteration = 0;
    vector<int64_t> multiReached;

    // running tally
    size_t index = 0;
    int64_t current = 0;

    bool fullIteration = false;
    bool reachedTwice = false;
    // check for previous item
    while (!fullIteration || !reachedTwice) {
        // check current frequency
        if (reachedFrequencies.count(current) == 0) {
            // insert current frequency
            reachedFrequencies.insert(current);
        } else {
            // insert into multi-reached
            multiReached.push_back(current);

            // mark repeated frequeny
            reachedTwice = true;
        }

        // apply actual change
        current += changes.at(index);

        // go to the next change
        index++;

        // loop-around for changes
        if (index >= changes.size()) {
            // shift index
            index = 0;

            if (!fullIteration) {
                // mark current frequency
                afterIteration = current;

                // mark finished iteration
                fullIteration = true;
            }
        }
    }

    cout << "final frequency after first iteration : " << afterIteration << endl;
    cout << "first frequency reached twice : " << multiReached.front() << endl;

    return 0;
}
