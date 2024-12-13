#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;
typedef unordered_map<Position, char> Farm;
typedef unordered_set<Position> Plot;

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
    { -1, 0 }, // UP
    { 0, 1 }, // RIGHT
    { 1, 0 }, // DOWN
    { 0, -1 } // LEFT
} };

const array<Position, 8> NEIGHBORS = { {
    { -1, -1 }, // UP-LEFT
    { -1, 0 }, // UP
    { -1, 1 }, // UP-RIGHT
    { 0, 1 }, // RIGHT
    { 1, 1 }, // DOWN-RIGHT
    { 1, 0 }, // DOWN
    { 1, -1 }, // DOWN-LEFT
    { 0, -1 } // LEFT
} };

Plot floodFill(const Farm& map,
               const Position& start,
               unordered_set<Position>& visited) {
    // check for early return
    if ((map.count(start) == 0) ||
        (visited.count(start) != 0)) {
        return {};
    }

    // temporary storage
    Plot plot;

    // setup for iteration
    char match = map.at(start);
    queue<Position> list;
    list.push(start);

    while (!list.empty()) {
        // get current position & value
        Position current = list.front();
        list.pop();

        if (visited.count(current) == 0) {
            // mark as visited
            visited.insert(current);
        } else {
            continue;
        }


        // add to plot
        plot.insert(current);

        // check neighbors
        for (const Position& direction : DIRECTIONS) {
            // calculate neighbor
            Position neighbor = {
                current.at(0) + direction.at(0),
                current.at(1) + direction.at(1)
            };

            // check if valid
            if ((map.count(neighbor) == 0) ||
                (map.at(neighbor) != match)) {
                continue;
            }

            // add to list
            list.push(neighbor);
        }
    }

    return plot;
}

vector<Plot> processPlots(const Farm& map) {
    // temporary storage
    unordered_set<Position> visited;
    vector<Plot> plots;

    // iterate over entries
    for (const pair<const Position, uint8_t>& entry : map) {
        // check if visited
        if (visited.count(entry.first) != 0) {
            continue;
        }

        // process plot
        Plot plot = floodFill(map, entry.first, visited);
        if (plot.size() > 0) {
            plots.push_back(plot);
        }
    }

    return plots;
}

size_t firstFencing(const Plot& plot) {
    // temporary storage
    size_t perimeter = 0;

    // iterate over positions
    for (const Position& position : plot) {
        // iterate over directions
        for (const Position& direction : DIRECTIONS) {
            // calculate neighbor
            Position neighbor = {
                position.at(0) + direction.at(0),
                position.at(1) + direction.at(1)
            };

            // check if valid
            if (plot.count(neighbor) == 0) {
                perimeter += 1;
            }
        }
    }

    return plot.size() * perimeter;
}

size_t checkCorner(bool first,
                   bool second,
                   bool third) {
    size_t corners = 0;

    // check for corners on this position
    if (!first && !third) {
        corners += 1;
    }

    if (first && !second && third) {
        corners += 1;
    }

    return corners;
}



size_t secondFencing(const Plot& plot) {
    // temporary storage
    size_t corners = 0;

    // iterate over positions
    for (const Position& position : plot) {
        // check for corners on this position
        array<bool, 8> neighbors;

        // iterate over directions
        for (size_t i = 0; i < 8; i++) {
            // retrieve short-hands
            const Position& direction = NEIGHBORS.at(i);

            // calculate neighbor
            Position neighbor = {
                position.at(0) + direction.at(0),
                position.at(1) + direction.at(1)
            };

            // check if valid
            if (plot.count(neighbor) != 0) {
                neighbors.at(i) = true;
            } else {
                neighbors.at(i) = false;
            }
        }

        // check left, up-left, up
        corners += checkCorner(neighbors.at(7), neighbors.at(0), neighbors.at(1));

        // check up, up-right, right
        corners += checkCorner(neighbors.at(1), neighbors.at(2), neighbors.at(3));

        // check right, down-right, down
        corners += checkCorner(neighbors.at(3), neighbors.at(4), neighbors.at(5));

        // check down, down-left, left
        corners += checkCorner(neighbors.at(5), neighbors.at(6), neighbors.at(7));
    }

    return plot.size() * corners;
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    Farm map;

    Position cursor = { 0, 0 };
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset column
        cursor.at(1) = 0;

        for (char value : line) {
            // store value
            map[cursor] = value;

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // generate distinct plots
    vector<Plot> plots = processPlots(map);

    // setup for iteration
    size_t firstPrice = 0;
    size_t secondPrice = 0;

    // iterate over plots
    for (const Plot& plot : plots) {
        // calculate fencing
        firstPrice += firstFencing(plot);
        secondPrice += secondFencing(plot);
    }

    // display results
    cout << "[first] total price: " << firstPrice << endl;
    cout << "[second] total price: " << secondPrice << endl;

    return 0;
}
