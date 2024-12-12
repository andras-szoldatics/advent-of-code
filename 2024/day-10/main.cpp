#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>

using namespace std;

typedef array<int64_t, 2> Position;
typedef unordered_map<Position, uint8_t> Topography;

// extend hash functions

namespace std {
template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) + hash<int64_t>() (p.at(1));
    }
};

}

const array<Position, 4> DIRECTIONS = { {
    { -1, 0 },
    { 0, 1 },
    { 1, 0 },
    { 0, -1 }
} };

pair<size_t, size_t> walkTrail(const Topography& map,
                               const Position& start) {
    // check for early return
    if ((map.count(start) == 0) ||
        (map.at(start) != 0)) {
        return { 0, 0 };
    }

    // setup for iteration
    unordered_map<Position, size_t> peaks;
    queue<Position> list;
    list.push(start);

    while (!list.empty()) {
        // get current position & value
        Position current = list.front();
        uint8_t value = map.at(current);
        list.pop();

        if (map.at(current) == 9) {
            // peak found, terminate walk
            peaks[current] += 1;
            continue;
        }

        // check neighbors
        for (const Position& direction : DIRECTIONS) {
            // calculate neighbor
            Position neighbor = {
                current.at(0) + direction.at(0),
                current.at(1) + direction.at(1)
            };

            // check if valid
            if ((map.count(neighbor) == 0) ||
                (map.at(neighbor) != value + 1)) {
                continue;
            }

            // add to list
            list.push(neighbor);
        }
    }

    // check total rating
    size_t rating = 0;
    for (const pair<const Position, size_t>& entry : peaks) {
        rating += entry.second;
    }

    return { peaks.size(), rating };
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    Topography map;

    Position cursor = { 0, 0 };
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset column
        cursor.at(1) = 0;

        for (char c : line) {
            // convert from char
            uint8_t value = 255;
            if ((c >= '0') &&
                (c <= '9')) {
                value = c - '0';
            }

            // store value
            map[cursor] = value;

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // setup for iteration
    size_t firstCount = 0;
    size_t secondCount = 0;

    // iterate over all positions
    for (const pair<const Position, uint8_t>& entry : map) {
        pair<size_t, size_t> values = walkTrail(map, entry.first);

        firstCount += values.first;
        secondCount += values.second;
    }

    // display results
    cout << "[first] total score: " << firstCount << endl;
    cout << "[second] total rating: " << secondCount << endl;

    return 0;
}
