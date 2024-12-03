#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a.at(0));
        std::size_t h2 = std::hash<int64_t>() (a.at(1));

        return (h1 + h2);
    }
};

class Position {
public:
    // attributes
    Coordinate coordinate;
    int64_t step;

    Position(const Coordinate& _coordinate,
             int64_t _step = 0) :
        coordinate(_coordinate),
        step(_step) {}

    // helper functions
    vector<Position> getNeighbours() {
        vector<Position> tmp;

        // get neighbours
        tmp.push_back(Position({ coordinate.at(0) + 1, coordinate.at(1) }, step + 1));
        tmp.push_back(Position({ coordinate.at(0), coordinate.at(1) + 1 }, step + 1));

        if (coordinate.at(0) > 0) {
            tmp.push_back(Position({ coordinate.at(0) - 1, coordinate.at(1) }, step + 1));
        }
        if (coordinate.at(1) > 0) {
            tmp.push_back(Position({ coordinate.at(0), coordinate.at(1) - 1 }, step + 1));
        }

        return tmp;
    }

    bool isWall(int64_t input) {
        int64_t tmp = input;
        tmp += coordinate.at(0) * coordinate.at(0);
        tmp += 3 * coordinate.at(0);
        tmp += 2 * coordinate.at(0) * coordinate.at(1);
        tmp += coordinate.at(1);
        tmp += coordinate.at(1) * coordinate.at(1);

        // assign into bitset
        bitset<64> b(tmp);

        return b.count() % 2 == 1;
    }
};

int64_t exploreMaze(Position start,
                    int64_t input,
                    variant<Coordinate, int64_t> termination) {
    // setup queue
    queue<Position> queue;
    queue.push(start);

    // setup visited
    unordered_set<Coordinate> excluded;
    unordered_set<Coordinate> visited;

    while (!queue.empty()) {
        // get current position
        Position current = queue.front();
        queue.pop();

        // check if already visited
        if (excluded.count(current.coordinate) != 0) {
            continue;
        }

        // check for termination
        Coordinate* tmp1 = get_if<Coordinate>(&termination);
        if ((tmp1) &&
            (current.coordinate == *tmp1)) {
            return current.step;
        }

        int64_t* tmp2 = get_if<int64_t>(&termination);
        if ((tmp2) &&
            (current.step > *tmp2)) {
            continue;
        }

        // add to visited
        excluded.insert(current.coordinate);
        visited.insert(current.coordinate);

        // get neighbours
        vector<Position> neighbours = current.getNeighbours();

        // check if wall
        for (Position neighbour : neighbours) {
            if (neighbour.isWall(input)) {
                // add to excluded
                excluded.insert(neighbour.coordinate);
                continue;
            }

            // add to queue
            queue.push(neighbour);
        }
    }

    return (termination.index() == 0 ? -1 : static_cast<int64_t>(visited.size()));
}

int main() {
    // unique inputs
    int64_t favoriteNumber = 1350;
    int64_t maximumSteps = 50;

    // setup iteration
    Position firstStart({ 1, 1 });
    Coordinate firstEnd({ 31, 39 });

    // generate results
    int64_t firstResult = exploreMaze(firstStart, favoriteNumber, firstEnd);
    cout << "[first] # of steps: " << firstResult << endl;

    int64_t secondResult = exploreMaze(firstStart, favoriteNumber, maximumSteps);
    cout << "[second] # of locations: " << secondResult << endl;

    return 0;
}
