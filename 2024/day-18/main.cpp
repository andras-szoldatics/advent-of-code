#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_map>

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
    int64_t score;

    bool operator<(const Entry& rhs) const {
        // handle as priority order
        return score > rhs.score;
    }
};

optional<int64_t> findPath(const unordered_map<Position, size_t>& walls,
                           const Position& start,
                           const Position& end,
                           int64_t fallenBytes) {
    // setup for iteration
    unordered_map<Position, int64_t> visited;

    priority_queue<Entry> queue;
    queue.push({ start, 0 });

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
        if (visited.count(current.position) == 0) {
            visited[current.position] = current.score;
        } else {
            continue;
        }

        // do the move
        for (const Position& direction : DIRECTIONS) {
            Position next = { current.position.at(0) + direction.at(0),
                              current.position.at(1) + direction.at(1) };

            // check for bounds
            if ((next.at(0) < 0) || (next.at(1) < 0)) {
                continue;
            }

            if ((next.at(0) > end.at(0)) || (next.at(1) > end.at(1))) {
                continue;
            }

            // check for walls
            if ((walls.count(next) == 0) ||
                (walls.at(next) > fallenBytes)) {
                queue.push({ next, current.score + 1 });
            }
        }
    }

    return nullopt;
}

const Position START = { 0, 0 };
const Position END = { 70, 70 };
const int64_t FIRST_DROP = 1'024;

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    unordered_map<Position, size_t> walls;
    vector<Position> lookup;
    int64_t lines = 1;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign to buffer
        stringstream buffer(line);
        string s;

        // read first value
        getline(buffer, s, ',');
        int64_t c = stoll(s);

        // read second value
        getline(buffer, s);
        int64_t r = stoll(s);

        // store the wall
        Position p = { r, c };
        lookup.push_back(p);

        if (walls.count(p) == 0) {
            walls[p] = lines;
        }

        // increment line counter
        lines++;
    }

    // setup for search

    // find the first path
    int64_t firstScore = findPath(walls, START, END, FIRST_DROP).value_or(0);

    // display results
    cout << "[first] # of minimum steps: " << firstScore << endl;

    // setup for binary search
    int64_t minimum = FIRST_DROP + 1;
    int64_t maximum = lookup.size();

    // perform binary search
    while (minimum != maximum) {
        // generate new check point
        int64_t check = (minimum + maximum) / 2;

        // find the path
        optional<int64_t> result = findPath(walls, START, END, check);

        // check for first failure
        if (result.has_value()) {
            // update minimum
            minimum = check + 1;
        } else {
            // update maximum
            maximum = check;
        }
    }

    // display results
    Position block = lookup.at(minimum - 1);
    cout << "[second] block position: " << block.at(1) << "," << block.at(0) << endl;

    return 0;
}
