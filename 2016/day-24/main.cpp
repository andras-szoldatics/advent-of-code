#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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

class Maze;

class Position {
public:
    // current position
    Coordinate coordinate;

    // distance from start
    int64_t steps;

    Position(const Coordinate& _coordinate,
             const int64_t _steps = 0) :
        coordinate(_coordinate),
        steps(_steps) {}
};

class Maze {
public:
    // walls of the maze
    unordered_set<Coordinate> walls;

    // POI positions
    unordered_map<int64_t, Coordinate> poi;

    // traversal matrix
    unordered_map<int64_t, unordered_map<int64_t, int64_t>> traversal;

    // utility functions
    void computeMatrix() {
        // retrieve vector of POI
        vector<int64_t> poiIds = getPOI();

        // compute traversal matrix
        for (size_t f = 0; f < poiIds.size(); f++) {
            // retrieve identifiers
            int64_t from = poiIds.at(f);

            // 0 distance to self
            traversal[from][from] = 0;

            for (size_t t = f + 1; t < poiIds.size(); t++) {
                // retrieve identifiers
                int64_t to = poiIds.at(t);

                // compute distance
                int64_t distance = calculateDistance(poi.at(from), poi.at(to));

                // store in matrix
                traversal[from][to] = distance;
                traversal[to][from] = distance;
            }
        }
    }

    int64_t calculateShortestPath(optional<int64_t> final = nullopt) {
        // temporary storage
        int64_t distance = numeric_limits<int64_t>::max();

        // retrieve default order
        vector<int64_t> poiIds = getPOI();
        int64_t current = calculateRoute(poiIds, final);
        if (distance > current) {
            distance = current;
        }

        // check all permutations
        while (next_permutation(poiIds.begin(), poiIds.end())) {
            // check if we start with the correct POI
            if (poiIds[0] != 0) {
                continue;
            }

            // calculate distance
            current = calculateRoute(poiIds, final);

            // check if better
            if (distance > current) {
                distance = current;
            }
        }

        return distance;
    }

private:
    // helper functions
    vector<int64_t> getPOI() {
        // retrieve vector of POI
        vector<int64_t> poiIds;
        poiIds.reserve(poi.size());

        for (const pair<const int64_t, Coordinate>& entry : poi) {
            poiIds.push_back(entry.first);
        }

        // sort for later use
        sort(poiIds.begin(), poiIds.end());

        return poiIds;
    }
    vector<Position> getNextSteps(const Position& position) {
        // temporary storage
        vector<Position> steps;

        // check all four directions
        // UP
        Position up = position;
        up.coordinate[0] -= 1;
        up.steps += 1;
        if (walls.count(up.coordinate) == 0) {
            steps.push_back(up);
        }

        // DOWN
        Position down = position;
        down.coordinate[0] += 1;
        down.steps += 1;
        if (walls.count(down.coordinate) == 0) {
            steps.push_back(down);
        }

        // LEFT
        Position left = position;
        left.coordinate[1] -= 1;
        left.steps += 1;
        if (walls.count(left.coordinate) == 0) {
            steps.push_back(left);
        }

        // RIGHT
        Position right = position;
        right.coordinate[1] += 1;
        right.steps += 1;
        if (walls.count(right.coordinate) == 0) {
            steps.push_back(right);
        }

        return steps;
    }

    // utility functions
    int64_t calculateDistance(const Coordinate& from,
                              const Coordinate& to) {
        // setup queue for search
        queue<Position> queue;
        queue.push(Position(from));

        // setup visited set
        unordered_set<Coordinate> visited;

        while (queue.size() > 0) {
            // get next position
            Position position = queue.front();
            queue.pop();

            // check if we are there
            if (position.coordinate == to) {
                return position.steps;
            }

            // check if we have been here
            if (visited.count(position.coordinate) != 0) {
                continue;
            }

            // mark as visited
            visited.insert(position.coordinate);

            // generate steps
            vector<Position> nextSteps = getNextSteps(position);

            // add to queue
            for (const Position& step : nextSteps) {
                queue.push(step);
            }
        }

        return -1;
    }

    int64_t calculateRoute(const vector<int64_t>& currentOrder,
                           optional<int64_t> final = nullopt) {
        // temporary storage
        int64_t distance = 0;

        // check all pairs
        for (size_t i = 0; i + 1 < currentOrder.size(); i++) {
            // retrieve identifiers
            int64_t from = currentOrder[i];
            int64_t to = currentOrder[i + 1];

            // retrieve distance
            distance += traversal.at(from).at(to);
        }

        // check if we need to return to start
        if (final) {
            // retrieve identifiers
            int64_t from = currentOrder.back();
            int64_t to = final.value_or(from);

            // retrieve distance
            distance += traversal.at(from).at(to);
        }

        return distance;
    }
};

int main() {
    // temporary storage
    string line;

    // store the map of the maze
    Maze maze;

    // setup for iteration
    int64_t row = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        for (size_t col = 0; col < line.size(); col++) {
            if (line[col] == '#') {
                // mark wall
                Coordinate tmp = { row, static_cast<int64_t>(col) };

                maze.walls.insert(tmp);
            } else if (line[col] != '.') {
                // mark POI
                Coordinate tmp = { row, static_cast<int64_t>(col) };
                int64_t poi = line[col] - '0';

                maze.poi[poi] = tmp;
            }
        }

        // next row
        row += 1;
    }

    // compute traversal matrix
    maze.computeMatrix();

    // compute results
    int64_t firstResult = maze.calculateShortestPath();
    cout << "[first] length of shortest path: " << firstResult << endl;

    int64_t secondResult = maze.calculateShortestPath(0);
    cout << "[second] length of shortest path: " << secondResult << endl;

    return 0;
}
