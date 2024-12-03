#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef vector<vector<int64_t>> Island;
typedef array<int64_t, 2> Position;

bool isOffIsland(const Island& island,
                 Position& position) {
    // retrieve short-hands
    int64_t rows = island.size();
    int64_t columns = island.front().size();

    if (position.at(0) < 0) {
        return true;
    }

    if (position.at(1) < 0) {
        return true;
    }

    if (position.at(0) >= rows) {
        return true;
    }

    if (position.at(1) >= columns) {
        return true;
    }

    return false;
}

array<Position, 4> DIRECTIONS = {
    { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } }
};

bool isBlocking(const Island& island,
                const Position& lookFrom,
                const Position& lookAt) {
    // retrieve short-hands
    const int64_t& fromHeight = island.at(lookFrom.at(0)).at(lookFrom.at(1));
    const int64_t& atHeight = island.at(lookAt.at(0)).at(lookAt.at(1));

    return (atHeight >= fromHeight);
}

int64_t getViewingDistance(const Island& island,
                           const Position& lookFrom,
                           const Position& direction) {
    // temporary storage
    Position current = lookFrom;
    int64_t distance = 0;

    bool blocked = false;
    // iterate until blocked
    while (!blocked) {
        // shift position
        current.at(0) += direction.at(0);
        current.at(1) += direction.at(1);

        // check for termination
        if (isOffIsland(island, current)) {
            blocked = true;
        } else {
            // increase distance
            distance++;

            if (isBlocking(island, lookFrom, current)) {
                blocked = true;
            }
        }
    }

    return distance;
}

int main() {
    // temporary storage
    string line;

    // storage for trees on the island
    Island island;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create new row
        island.push_back({});

        // iterate over numbers
        for (char c : line) {
            // convert char to height
            int64_t height = static_cast<int64_t>(c - '0');

            // set height in appropriate entry in map
            island.back().push_back(height);
        }
    }

    // keep running tally
    int64_t visibleFromBeach = 0;
    int64_t bestScenicScore = 0;

    // iterate over trees
    for (size_t row = 0; row < island.size(); row++) {
        for (size_t column = 0; column < island.front().size(); column++) {
            // assemble position
            Position lookFrom;
            lookFrom.at(0) = row;
            lookFrom.at(1) = column;

            int64_t visible = 0;
            int64_t scenicScore = 1;
            // iterate over directions
            for (const Position& direction : DIRECTIONS) {
                // retrieve viewing distance
                int64_t distance = getViewingDistance(island, lookFrom, direction);

                // adjust scenic score
                scenicScore *= distance;

                // calculate blocking tree
                Position blockingTree = lookFrom;
                blockingTree.at(0) += (distance * direction.at(0));
                blockingTree.at(1) += (distance * direction.at(1));

                if (distance == 0) {
                    // handle border trees
                    visible++;
                } else {
                    // check if blocking tree is actually blocking
                    if (!isBlocking(island, lookFrom, blockingTree)) {
                        // this can only happen on a border tree
                        visible++;
                    }
                }
            }

            // check all four sides
            if (visible >= 1) {
                visibleFromBeach++;
            }

            // adjust best score
            if (bestScenicScore < scenicScore) {
                bestScenicScore = scenicScore;
            }
        }
    }

    // display results
    cout << "tress visible from beach : " << visibleFromBeach << endl;
    cout << "higest scenic score : " << bestScenicScore << endl;

    return 0;
}
