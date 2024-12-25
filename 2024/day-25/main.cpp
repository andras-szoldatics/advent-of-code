#include <array>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const size_t PINS = 5;
const int64_t SIZING = 7;

class Lock {
public:
    array<int64_t, PINS> tumblers;

    Lock() {
        tumblers.fill(0);
    }
};

class Key {
public:
    array<int64_t, PINS> notches;

    Key() {
        notches.fill(0);
    }

    bool fitsInto(const Lock& lock) const {
        for (size_t i = 0; i < PINS; i++) {
            if (notches.at(i) + lock.tumblers.at(i) > SIZING) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    // temporary storage
    string full = string(PINS, '#');
    string line;

    // store the lock & keys
    vector<Lock> locks;
    vector<Key> keys;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        vector<string> lines = { line };

        // read rest of the entry
        for (size_t i = 1; i < SIZING; i++) {
            getline(input, line);
            lines.push_back(line);
        }

        if (lines.front() == full) {
            // temporary storage
            Lock lock;

            // check mappings
            for (const string& line : lines) {
                for (size_t i = 0; i < PINS; i++) {
                    if (line.at(i) == '#') {
                        lock.tumblers.at(i) += 1;
                    }
                }
            }

            // add to locks
            locks.push_back(lock);
        } else if (lines.back() == full) {
            // temporary storage
            Key key;

            // check mappings
            for (const string& line : lines) {
                for (size_t i = 0; i < PINS; i++) {
                    if (line.at(i) == '#') {
                        key.notches.at(i) += 1;
                    }
                }
            }

            // add to keys
            keys.push_back(key);
        } else {
            // invalid entry
            break;
        }

        // skip to next entry
        getline(input, line);
    }

    // setup for iteration
    size_t matching = 0;

    // iterate over all the pairs of locks and keys
    for (const Lock& lock : locks) {
        for (const Key& key : keys) {
            if (key.fitsInto(lock)) {
                matching += 1;
            }
        }
    }

    // display progress
    cout << "[first] # of matching pairs: " << matching << endl;

    return 0;
}
