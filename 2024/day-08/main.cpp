#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

// extend hash functions

namespace std {
template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) + hash<int64_t>() (p.at(1));
    }
};

}

typedef unordered_map<char, vector<Position>> Antennas;

bool inBounds(const Position& widthAndHeight,
              int64_t row,
              int64_t col) {
    if ((row < 0) ||
        (row >= widthAndHeight.at(0))) {
        return false;
    }

    if ((col < 0) ||
        (col >= widthAndHeight.at(1))) {
        return false;
    }

    return true;
}

size_t findAntiNodes(const Antennas& antennas,
                     const Position& widthAndHeight,
                     bool simplified) {
    // setup for iteration
    unordered_set<Position> antinodes;

    // iterate over each set of frequencies
    for (const pair<const char, vector<Position>> entry : antennas) {
        // check every pair of antennas
        for (size_t f = 0; f < entry.second.size(); f++) {
            // retrieve short-hands
            const Position& from = entry.second.at(f);

            for (size_t t = 0; t < entry.second.size(); t++) {
                // skip the same antenna
                if (f == t) {
                    continue;
                }

                // retrieve short-hands
                const Position& to = entry.second.at(t);

                // calculate the distance between the two
                Position delta = { to.at(0) - from.at(0), to.at(1) - from.at(1) };

                // setup for iteration
                bool canRoll = true;
                int64_t factor = (simplified ? 1 : 0);

                while (canRoll) {
                    // generate antinode
                    Position antinode = { to.at(0) + delta.at(0) * factor,
                                          to.at(1) + delta.at(1) * factor };

                    // check if this is within bounds
                    if (inBounds(widthAndHeight, antinode.at(0), antinode.at(1))) {
                        antinodes.insert(antinode);
                    } else {
                        canRoll = false;
                    }

                    if (simplified) {
                        // first mode exits eraly
                        break;
                    }

                    // next factor
                    factor += 1;
                }
            }
        }
    }

    return antinodes.size();
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    Antennas antennas;

    Position cursor = { 0, 0 };
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset column
        cursor.at(1) = 0;

        for (char c : line) {
            if (c != '.') {
                antennas[c].push_back(cursor);
            }

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // make note of the map's extent
    Position widthAndHeight = cursor;

    // display first result
    size_t firstResult = findAntiNodes(antennas, widthAndHeight, true);
    cout << "[first] # of unique antinodes: " << firstResult << endl;

    // display second result
    size_t secondResult = findAntiNodes(antennas, widthAndHeight, false);
    cout << "[second] # of unique antinodes: " << secondResult << endl;

    return 0;
}
