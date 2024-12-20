#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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

const array<Position, 4> DIRECTIONS = { {
    { 0, 1 }, // RIGHT
    { 1, 0 }, // DOWN
    { 0, -1 }, // LEFT
    { -1, 0 }, // UP
} };

int64_t getDistance(const Position& a,
                    const Position& b) {
    return abs(a.at(0) - b.at(0)) + abs(a.at(1) - b.at(1));
}

class Entry {
public:
    Position position;
    int64_t time;

    bool operator<(const Entry& rhs) const {
        // handle as priority order
        return time > rhs.time;
    }
};

vector<Entry> findPath(const unordered_set<Position>& walls,
                       const Position& start,
                       const Position& end) {
    // setup for iteration
    unordered_map<Position, int64_t> visited;
    vector<Entry> path;

    priority_queue<Entry> queue;
    queue.push({ start, 0 });

    // iterate over the queue
    while (!queue.empty()) {
        // get the current position
        Entry current = queue.top();
        queue.pop();

        // check for duplicated visits
        if (visited.count(current.position) == 0) {
            visited[current.position] = current.time;

            // extend the path
            path.push_back(current);
        } else {
            continue;
        }

        // check if the position is the end
        if (current.position == end) {
            break;
        }

        // check for moves
        for (const Position& direction : DIRECTIONS) {
            Position next = { current.position.at(0) + direction.at(0),
                              current.position.at(1) + direction.at(1) };

            // check for walls
            if (walls.count(next) != 0) {
                continue;
            }

            // add to the queue
            queue.push({ next, current.time + 1 });
        }
    }

    return path;
}

int64_t findShortcuts(const vector<Entry>& path,
                      int64_t maxDistance,
                      int64_t minCheat) {
    // setup for iteration
    int64_t shortcuts = 0;

    // iterate over the pairs of positions
    for (size_t f = 0; f < path.size(); f++) {
        // retrieve short-hands
        const Entry& from = path.at(f);

        for (size_t t = f + 1; t < path.size(); t++) {
            // retrieve short-hands
            const Entry& to = path.at(t);

            // check for distance
            int64_t distance = getDistance(path.at(f).position, path.at(t).position);

            if (distance > maxDistance) {
                continue;
            }

            // calculate final savings
            int64_t savings = to.time - from.time;
            savings -= distance;

            // mark shortcuts
            if (savings >= minCheat) {
                shortcuts += 1;
            }
        }
    }

    return shortcuts;
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    unordered_set<Position> walls;
    Position start = { 0, 0 };
    Position end = { 0, 0 };

    Position cursor = { 0, 0 };
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset column
        cursor.at(1) = 0;

        for (char c : line) {
            if (c == '#') {
                walls.insert(cursor);
            } else if (c == 'S') {
                start = cursor;
            } else if (c == 'E') {
                end = cursor;
            }

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // find the path
    vector<Entry> path = findPath(walls, start, end);

    // check for shortcuts
    int64_t firstShortcuts = findShortcuts(path, 2, 100);

    // display progress
    cout <<  "[first] # of shortcuts: " << firstShortcuts << endl;

    // check for shortcuts
    int64_t secondShortcuts = findShortcuts(path, 20, 100);

    // display progress
    cout <<  "[second] # of shortcuts: " << secondShortcuts << endl;

    return 0;
}
