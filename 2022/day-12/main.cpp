// C++ standard includes
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>

using namespace std;

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

typedef vector<vector<int64_t>> HeightMap;
typedef unordered_map<Coordinate, int64_t> MappedValues;

MappedValues climb(const HeightMap& heightMap,
                   const vector<Coordinate>& startPositions) {
    // temporary storage
    MappedValues visitedMap;

    // routing state
    list<pair<Coordinate, Coordinate>> queue;

    // setup iteration with possible starts
    for (const Coordinate& startPosition : startPositions) {
        // mark as already visited at start
        visitedMap[startPosition] = -1;

        // add as a self-pair to queue
        queue.push_back({ startPosition, startPosition });
    }

    // iterate over queue
    while (!queue.empty()) {
        // retrieve elements FIFO style
        pair<Coordinate, Coordinate> element = queue.front();
        queue.pop_front();

        // retrieve short-hands
        const Coordinate& current = element.first;
        const Coordinate& previous = element.second;

        // check if current is already climbed
        if ((visitedMap.count(current) != 0) &&
            (visitedMap.at(current) >= 0)) {
            // already climbed, skip processing
            continue;
        }

        // mark visitation
        int64_t previousIteration = visitedMap.at(previous);
        visitedMap[current] = (previousIteration + 1);

        vector<Coordinate> deltas = {
            { +1, 0 },
            { 0, +1 },
            { -1, 0 },
            { 0, -1 }
        };
        // generate branching paths
        for (const Coordinate& delta : deltas) {
            // generate new coordinate
            Coordinate next = current;
            next.at(0) += delta.at(0);
            next.at(1) += delta.at(1);

            // validate new coordinate
            if ((next.at(0) < 0) ||
                (next.at(1) < 0) ||
                (next.at(0) >= static_cast<int64_t>(heightMap.size())) ||
                (next.at(1) >= static_cast<int64_t>(heightMap.at(next.at(0)).size()))) {
                // this is off the map, skip it
                continue;
            }

            // retrieve short-hands
            char currentHeight = heightMap.at(current.at(0)).at(current.at(1));
            char nextHeight = heightMap.at(next.at(0)).at(next.at(1));

            // check height requirements
            if (nextHeight > currentHeight + 1) {
                // this is too much of a slope, skip it
                continue;
            }

            // if everything checks out, add new coordinate to queue
            queue.push_back({ next, current });
        }
    }

    if (heightMap.size() < 30) {
        // display visited map, for debugging
        for (size_t row = 0; row < heightMap.size(); row++) {
            for (size_t column = 0; column < heightMap.at(row).size(); column++) {
                // temporary storage
                Coordinate position = { static_cast<int64_t>(row), static_cast<int64_t>(column) };

                // print visited number
                if (visitedMap.count(position) == 0) {
                    cout << "    " << endl;
                } else {
                    cout << setw(4) << visitedMap.at(position);
                }
            }

            cout << endl;
        }
    }

    return visitedMap;
}

int main() {
    // temporary storage
    string line;

    // height map of the area
    HeightMap heightMap;

    // coordinates of things
    Coordinate firstStart = { -1, -1 };
    Coordinate bothEnd = { -1, -1 };

    // input state
    size_t inputRow = 0;

    // read input file
    ifstream input("input.txt");

    // list of possible starting positions
    vector<Coordinate> secondStart;

    // parse map line by line
    while (getline(input, line)) {
        // temporary storage
        vector<int64_t> heightRow;

        // iterate over line
        for (size_t column = 0; column < line.size(); column++) {
            // retrieve short-hands
            char c = line.at(column);

            // check for special height markers
            if (c == 'S') {
                // this is a start coordinate
                firstStart.at(0) = inputRow;
                firstStart.at(1) = column;

                // swap with proper height value
                c = 'a';
            }

            if (c == 'E') {
                // this is an end coordinate
                bothEnd.at(0) = inputRow;
                bothEnd.at(1) = column;

                // swap with proper height value
                c = 'z';
            }

            // fill out temporary vector
            int64_t height = (c - 'a');
            heightRow.push_back(height);

            // second start is every low place
            if (height == 0) {
                Coordinate lowPlace;
                lowPlace.at(0) = inputRow;
                lowPlace.at(1) = column;

                secondStart.push_back(lowPlace);
            }
        }

        // add vectors to maps
        heightMap.push_back(heightRow);

        // shift rows down
        inputRow++;
    }

    // check for early termination
    if (heightMap.empty()) {
        return 1;
    }

    // climb with first method
    MappedValues firstVisited = climb(heightMap, { firstStart });

    // display number at end
    cout << "[first] climb finished after move #" << firstVisited.at(bothEnd) << endl;

    // climb with second method
    MappedValues secondVisited = climb(heightMap, secondStart);

    // display number at end
    cout << "[second] climb finished after move #" << secondVisited.at(bothEnd) << endl;

    return 0;
}
