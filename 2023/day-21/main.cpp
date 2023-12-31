#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<size_t, 2> Position;

template<>
struct std::hash<Position> {
    size_t operator()(const array<size_t, 2>& a) const {
        size_t h1 = std::hash<size_t>() (a.front());
        size_t h2 = std::hash<size_t>() (a.back());

        return (h1 + h2);
    }
};

class Tile {
public:
    // tile data
    Position start;
    unordered_set<Position> rocks;

    // extent data
    size_t width;
    size_t height;

    Tile() :
        start({ 0, 0 }),
        width(0),
        height(0) {}

    // helper functions
    void processLine(const string& line) {
        // shift width
        if (line.size() > width) {
            width = line.size();
        }

        // process line
        Position cursor = { height, 0 };
        for (cursor.back() = 0; cursor.back() < line.size(); cursor.back()++) {
            // retrieve short-hands
            char c = line.at(cursor.back());

            switch (c) {
            case '#':
                rocks.insert(cursor);
                break;
            case 'S':
                start = cursor;
                break;
            }
        }

        // shift height
        height += 1;
    }

    // utility functions
    vector<Position> getNextPositions(const Position& pos) const {
        // initialize vector
        vector<Position> next;

        // check left
        if (pos.back() > 0) {
            Position left = { pos.front(), pos.back() - 1 };
            if (rocks.find(left) == rocks.end()) {
                next.push_back(left);
            }
        }

        // check right
        if (pos.back() < width - 1) {
            Position right = { pos.front(), pos.back() + 1 };
            if (rocks.find(right) == rocks.end()) {
                next.push_back(right);
            }
        }

        // check up
        if (pos.front() > 0) {
            Position up = { pos.front() - 1, pos.back() };
            if (rocks.find(up) == rocks.end()) {
                next.push_back(up);
            }
        }

        // check down
        if (pos.front() < height - 1) {
            Position down = { pos.front() + 1, pos.back() };
            if (rocks.find(down) == rocks.end()) {
                next.push_back(down);
            }
        }

        return next;
    }
};

class Step {
public:
    Position position;
    size_t count;

    Step(const Position& p,
         size_t c = 0) :
        position(p),
        count(c) {}

    // operator functions
    bool operator<(const Step& other) const {
        return count > other.count;
    }
};

pair<size_t, size_t> countTiles(const Tile& tile,
                                size_t firstSteps,
                                size_t secondSteps) {
    // temporary storage
    pair<size_t, size_t> result = { 0, 0 };
    unordered_set<Position> visited;

    // assuming real input, store some parity counts
    int64_t centerOnCount = 0;
    int64_t centerOffCount = 0;
    int64_t otherBothCount = 0;

    // initialize queue
    priority_queue<Step> queue;
    queue.emplace(tile.start);

    // iterate over queue
    while (!queue.empty()) {
        // retrieve next step
        Step step = queue.top();
        queue.pop();

        // check visited
        if (visited.count(step.position) == 0) {
            visited.insert(step.position);
        } else {
            continue;
        }

        // check step count
        if (step.count <= firstSteps) {
            // count reachable tiles
            if (step.count % 2 == firstSteps % 2) {
                result.first += 1;
            }
        }

        // tally up stuff for second part
        if (step.count <= tile.width / 2) {
            // count reachable tiles
            if (step.count % 2 == secondSteps % 2) {
                centerOnCount += 1;
            } else {
                centerOffCount += 1;
            }
        } else {
            otherBothCount += 1;
        }

        // generate next steps
        vector<Position> nextPositions = tile.getNextPositions(step.position);
        for (const Position& entry : nextPositions) {
            // filter to extent
            if ((entry.front() >= tile.height) ||
                (entry.back() >= tile.width)) {
                continue;
            }

            // add to queue
            queue.emplace(entry, step.count + 1);
        }
    }

    // calculate second part
    size_t stepExtent = secondSteps / tile.width;
    size_t firstRing = (stepExtent / 2) * 2 + 1;
    firstRing *= firstRing;

    size_t secondRing = (stepExtent + 1) / 2 * 2;
    secondRing *= secondRing;

    size_t weirdTiles = (firstRing + secondRing) / 2;

    // final count
    result.second = (firstRing * centerOnCount) +
        (secondRing * centerOffCount) +
        (weirdTiles * otherBothCount);

    return result;
}

const size_t FIRST_STEPS = 64;
const size_t SECOND_STEPS = 26'501'365;

int main() {
    // temporary storage line
    string line;

    // store garden
    Tile garden;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        garden.processLine(line);
    }

    // calculate result
    pair<size_t, size_t> result = countTiles(garden, FIRST_STEPS, SECOND_STEPS);

    // display result
    cout << "[first] tiles reachable in " << FIRST_STEPS << " steps: " << result.first << endl;
    cout << "[second] tiles reachable in " << SECOND_STEPS << " steps: " << result.second << endl;

    return 0;
}
