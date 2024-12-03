#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 3> Position;
typedef array<Position, 2> Interval;

template<>
struct std::hash<Position> {
    std::size_t operator()(const Position& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);
        std::size_t h3 = std::hash<int64_t>() (a[2]);

        return (h1 + h2 + h3);
    }
};

const array<int64_t, 3> DELTAS = { -1, 0, 1 };

class Voxel {
public:
    Position position;

    Voxel() {
        fill(position.begin(), position.end(), 0);
    }
    Voxel(const Position& _position) :
        position(_position) {}

    // parsing function
    void parse(const string& line) {
        // temporary storage
        string tmp;

        // assign line to buffer
        stringstream buffer(line);

        // read first value
        getline(buffer, tmp, ',');
        position.at(0) = stoll(tmp);

        // read second value
        getline(buffer, tmp, ',');
        position.at(1) = stoll(tmp);

        // read third value
        getline(buffer, tmp, ',');
        position.at(2) = stoll(tmp);
    }

    // utility functions
    vector<Position> getNeighbors() const {
        // temporary storage
        vector<Position> tmp;

        // generate deltas
        for (const int64_t& delta : DELTAS) {
            if (delta != 0) {
                // generate X delta
                Position deltaX = position;
                deltaX.at(0) += delta;

                // generate X delta
                Position deltaY = position;
                deltaY.at(1) += delta;

                // generate X delta
                Position deltaZ = position;
                deltaZ.at(2) += delta;

                // add deltas to vector
                tmp.push_back(deltaX);
                tmp.push_back(deltaY);
                tmp.push_back(deltaZ);
            }
        }

        return tmp;
    }

    int64_t freeSides(const unordered_set<Position>& positions) const {
        // temporary storage
        int64_t freeSides = 6;

        // retrieve possible neighbors
        vector<Position> possibleNeighbors = getNeighbors();

        // iterate over negihbors
        for (const Position& possibleNeighbor : possibleNeighbors) {
            // check for existing position
            if (positions.count(possibleNeighbor) != 0) {
                freeSides--;
            }
        }

        return freeSides;
    }
};

unordered_set<Position> expandPosition(const unordered_set<Position>& occupied,
                                       const unordered_set<Position>& inside,
                                       const Interval& interval,
                                       const Position& position) {
    // temporary storage
    unordered_set<Position> expansion;

    // initialize queue
    queue<Position> queue;
    queue.push(position);

    // execute iteration
    while (!queue.empty()) {
        // retrieve first entry
        Position current = queue.front();
        queue.pop();

        // check for overlap
        if ((occupied.count(current) != 0) ||
            (expansion.count(current) != 0)) {
            continue;
        }

        // check if went over the interval on the small side
        if ((current.at(0) <= interval.front().at(0)) ||
            (current.at(1) <= interval.front().at(1)) ||
            (current.at(2) <= interval.front().at(2))) {
            // over the line, this is outside
            return {};
        }

        // check if we went over the interval on the large side
        if ((current.at(0) >= interval.back().at(0)) ||
            (current.at(1) >= interval.back().at(1)) ||
            (current.at(2) >= interval.back().at(2))) {
            // over the line, this is outside
            return {};
        }

        // add current to expansion
        expansion.insert(current);

        // generate neighbors for voxel
        Voxel currentVoxel(current);
        vector<Position> neighbors = currentVoxel.getNeighbors();

        // iterate over neighbors
        for (const Position& next : neighbors) {
            // check for termination
            if ((occupied.count(next) != 0) ||
                (inside.count(next) != 0) ||
                (expansion.count(next) != 0)) {
                continue;
            }

            // add voxel
            queue.push(next);
        }
    }

    return expansion;
}

unordered_set<Position> findInsidePositions(const unordered_set<Position>& occupied) {
    // temporary storage
    unordered_set<Position> inside;

    // setup interval
    int64_t maximum = numeric_limits<int64_t>::max();
    int64_t minimum = numeric_limits<int64_t>::min();
    Interval interval;
    interval.front() = { maximum, maximum, maximum };
    interval.back() = { minimum, minimum, minimum };

    // iterate over occupied positions
    for (const Position& entry : occupied) {
        // minimize starting interval
        if (interval.front().at(0) > entry.at(0)) {
            interval.front().at(0) = entry.at(0);
        }

        if (interval.front().at(1) > entry.at(1)) {
            interval.front().at(1) = entry.at(1);
        }

        if (interval.front().at(2) > entry.at(2)) {
            interval.front().at(2) = entry.at(2);
        }

        // maximize ending interval
        if (interval.back().at(0) < entry.at(0)) {
            interval.back().at(0) = entry.at(0);
        }

        if (interval.back().at(1) < entry.at(1)) {
            interval.back().at(1) = entry.at(1);
        }

        if (interval.back().at(2) < entry.at(2)) {
            interval.back().at(2) = entry.at(2);
        }
    }

    // iterate over reasonable positions that could be inside
    for (int64_t x = (interval.front().at(0) + 1); x < interval.back().at(0); x++) {
        for (int64_t y = (interval.front().at(1) + 1); y < interval.back().at(1); y++) {
            for (int64_t z = (interval.front().at(2) + 1); z < interval.back().at(2); z++) {
                // assemble position
                Position position = { x, y, z };

                // check if this position is unclassified
                if ((occupied.count(position) == 0) ||
                    (inside.count(position) == 0)) {
                    // execute expansion
                    unordered_set<Position> expansion = expandPosition(occupied, inside, interval, position);

                    // check if this expansion is valuable
                    if (!expansion.empty()) {
                        inside.merge(expansion);
                    }
                }
            }
        }
    }

    return inside;
}

int main() {
    // temporary storage
    string line;

    // store droplet information
    vector<Voxel> droplets;

    // store occupied space
    unordered_set<Position> occupied;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Voxel droplet;

        // parse droplet
        droplet.parse(line);

        if (occupied.count(droplet.position) == 0) {
            // store droplet
            droplets.push_back(droplet);
            occupied.insert(droplet.position);
        }
    }

    // keep a running tally
    int64_t totalSides = 0;

    // iterate over droplets
    for (const Voxel& droplet : droplets) {
        // retrieve free sides
        totalSides += droplet.freeSides(occupied);
    }

    // display results
    cout << "approximate surface area of shape : " << totalSides << endl;

    // find inside position
    unordered_set<Position> insidePositions = findInsidePositions(occupied);

    // assemble merged position
    unordered_set<Position> mergedPositions;
    mergedPositions.merge(occupied);
    mergedPositions.merge(insidePositions);

    // keep a running tally
    int64_t outsideSides = 0;

    // iterate over droplets
    for (const Voxel& droplet : droplets) {
        // retrieve free sides
        outsideSides += droplet.freeSides(mergedPositions);
    }

    // display results
    cout << "outside surface area of shape : " << outsideSides << endl;

    return 0;
}
