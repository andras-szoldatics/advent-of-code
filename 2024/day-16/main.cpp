#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
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

class Entry {
public:
    Position position;
    size_t direction;

    int64_t score;

    bool operator<(const Entry& rhs) const {
        // handle as priority order
        return score > rhs.score;
    }
};

void doMove(const unordered_set<Position>& walls,
            const Entry& current,
            priority_queue<Entry>& queue,
            bool reversed) {
    // generate next entries
    array<size_t, 3> turns = { 0, 1, 3 };

    for (size_t turn : turns) {
        // get direction, reverse if needed
        size_t direction = (current.direction + turn) + (reversed ? 2 : 0);
        direction %= 4;

        Position next = { current.position.at(0) + DIRECTIONS.at(direction).at(0),
                          current.position.at(1) + DIRECTIONS.at(direction).at(1) };

        // check for walls
        if (walls.count(next) != 0) {
            continue;
        }

        // add to the queue, reverse if needed
        Position position = (turn == 0 ? next : current.position);
        int64_t delta = (turn == 0 ? 1 : 1000);
        queue.push({ position, (direction + (reversed ? 2 : 0)) % 4, current.score + delta });
    }
}

optional<int64_t> findPath(const unordered_set<Position>& walls,
                           const Position& start,
                           const Position& end) {
    // setup for iteration
    array<unordered_set<Position>, 4> visited;

    priority_queue<Entry> queue;
    queue.push({ start, 0, 0 });

    // iterate over the queue
    while (!queue.empty()) {
        // get the current position
        Entry current = queue.top();
        queue.pop();

        // check if the position is the end
        if (current.position == end) {
            return current.score;
        }

        // check for duplicated visits
        unordered_set<Position>& v = visited.at(current.direction);
        if (v.count(current.position) == 0) {
            v.insert(current.position);
        } else {
            continue;
        }

        // do the move
        doMove(walls, current, queue, false);
    }

    return nullopt;
}

size_t scanPath(const unordered_set<Position> walls,
                const Position& start,
                const Position& end,
                int64_t target) {
    // setup for iteration
    array<unordered_map<Position, int64_t>, 4> startWeights;

    priority_queue<Entry> startQueue;
    startQueue.push({ start, 0, 0 });

    // iterate over the queue
    while (!startQueue.empty()) {
        // get the current position
        Entry current = startQueue.top();
        startQueue.pop();

        // check if we exceeded the target
        if (current.score > target) {
            break;
        }

        // check for duplicated visits
        unordered_map<Position, int64_t>& sw = startWeights.at(current.direction);
        if (sw.count(current.position) == 0) {
            sw[current.position] = current.score;
        } else {
            continue;
        }


        // do the move
        doMove(walls, current, startQueue, false);

    }

    // setup for iteration
    unordered_set<Position> tiles;

    // iterate over finish directions
    for (size_t i = 0; i < 4; i++) {
        // setup for iteration
        array<unordered_map<Position, int64_t>, 4> endWeights;

        priority_queue<Entry> endQueue;
        endQueue.push({ end, i, 0 });

        // iterate over queue
        while (!endQueue.empty()) {
            // get the current position
            Entry current = endQueue.top();
            endQueue.pop();

            // check if we exceeded the target
            if (current.score > target) {
                break;
            }

            // check for duplicated visits
            unordered_map<Position, int64_t>& ew = endWeights.at(current.direction);
            if (ew.count(current.position) == 0) {
                ew[current.position] = current.score;
            } else {
                continue;
            }

            // check for start weights
            unordered_map<Position, int64_t>& sw = startWeights.at(current.direction);
            if (sw.count(current.position) != 0) {
                // check if this tile is within bounds
                int64_t total = sw.at(current.position) + current.score;

                if (total <= target) {
                    tiles.insert(current.position);
                }
            }

            // do the move
            doMove(walls, current, endQueue, true);
        }
    }

    return tiles.size();

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
    int64_t firstScore = findPath(walls, start, end).value_or(0);

    // display results
    cout << "[first] total score: " << firstScore << endl;

    // scan the paths
    size_t secondScore = scanPath(walls, start, end, firstScore);

    // display results
    cout << "[second] total score: " << secondScore << endl;

    return 0;
}
