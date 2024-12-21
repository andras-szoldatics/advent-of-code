#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>

using namespace std;

typedef array<int64_t, 2> Position;

class Input {
public:
    size_t depth;

    char from;
    char to;

    Input(size_t d,
          char f,
          char t) :
        depth(d),
        from(f),
        to(t) {}

    // operator functions
    bool operator==(const Input& other) const {
        return depth == other.depth &&
               from == other.from &&
               to == other.to;
    }
};

namespace std {
// extend hash functions

template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) +
               hash<int64_t>() (p.at(1));
    }
};

template<>
struct hash<Input> {
    size_t operator()(const Input& t) const {
        return hash<size_t>() (t.depth) +
               hash<char>() (t.from) +
               hash<char>() (t.to);
    }
};

} // namespace std

class Keypad {
public:
    // id storage
    string id;

    // key storage
    unordered_map<Position, char> keys;
    Position defaultStart;

    // cache storage
    unordered_map<char, Position> lookup;
    unordered_map<Input, size_t> costs;

    Keypad(const string& i,
           const unordered_map<Position, char>& k) :
        id(i),
        keys(k),
        defaultStart({ 0, 0 }) {
        // calculate lookup
        for (const pair<const Position, char>& key : keys) {
            lookup[key.second] = key.first;
        }

        // determine default start
        if (lookup.count('A') != 0) {
            defaultStart = lookup.at('A');
        }
    }
};

// define different keypads

Keypad NUMERIC = {
    string("NUMERIC"),
    {
        { { 1, 1 }, '7' },
        { { 1, 2 }, '8' },
        { { 1, 3 }, '9' },
        { { 2, 1 }, '4' },
        { { 2, 2 }, '5' },
        { { 2, 3 }, '6' },
        { { 3, 1 }, '1' },
        { { 3, 2 }, '2' },
        { { 3, 3 }, '3' },
        { { 4, 2 }, '0' },
        { { 4, 3 }, 'A' }
    }
};

Keypad DIRECTIONAL = {
    string("DIRECTIONAL"),
    {
        { { 1, 2 }, '^' },
        { { 1, 3 }, 'A' },
        { { 2, 1 }, '<' },
        { { 2, 2 }, 'v' },
        { { 2, 3 }, '>' }
    }
};

const array<Position, 4> MOVES = { {
    { 0, 1 }, // RIGHT
    { 1, 0 }, // DOWN
    { 0, -1 }, // LEFT
    { -1, 0 }, // UP
} };

const array<char, 4> CHARS = { {
    '>',
    'v',
    '<',
    '^'
} };

class PathEntry {
public:
    Position position;
    size_t cost;

    char lastMove;

    PathEntry(const Position& p,
              size_t c,
              char lm) :
        position(p),
        cost(c),
        lastMove(lm) {}
};

size_t findCost(Keypad& keypad,
                const Input& input) {
    // check the cache for step
    if (keypad.costs.count(input) != 0) {
        return keypad.costs.at(input);
    }

    // retrieve positions
    Position from = keypad.lookup.at(input.from);
    Position to = keypad.lookup.at(input.to);

    // check for first layer
    if (input.depth == 0) {
        // calculate manhattan cost
        size_t manhattan = abs(from.at(0) - to.at(0)) + abs(from.at(1) - to.at(1));

        return manhattan + 1;
    }

    // setup for iteration
    queue<PathEntry> queue;
    queue.push({ from, 0, 'A' });

    // search for path
    size_t limit = numeric_limits<size_t>::max();
    while (!queue.empty()) {
        // get next entry
        PathEntry entry = queue.front();
        queue.pop();

        // check for limit
        if (entry.cost > limit) {
            continue;
        }

        // check for destination
        if (entry.position == to) {
            // generate final button press
            size_t depth = input.depth - 1;
            char fc = entry.lastMove;

            size_t cost = findCost(DIRECTIONAL, { depth, fc, 'A' });
            size_t final_cost = entry.cost + cost;

            if (limit > final_cost) {
                limit = final_cost;
            }
        } else {
            // split entry for next entry
            for (size_t i = 0; i < MOVES.size(); ++i) {
                // get next position
                Position next = {
                    entry.position.at(0) + MOVES.at(i).at(0),
                    entry.position.at(1) + MOVES.at(i).at(1)
                };

                // check for valid position
                if (keypad.keys.count(next) != 0) {
                    // add to queue with pre-computed cost
                    size_t depth = input.depth - 1;
                    char fc = entry.lastMove;
                    char tc = CHARS.at(i);

                    size_t cost = findCost(DIRECTIONAL, { depth, fc, tc });
                    queue.push({ next, entry.cost + cost, tc });
                }
            }
        }
    }

    // set cache value
    keypad.costs[input] = limit;

    return limit;
}

size_t computeComplexity(const vector<string>& targets,
                         size_t robotChain) {
    // temporary storage
    size_t complexity = 0;

    // iterate over targets
    for (const string& target : targets) {
        // temporary storage
        size_t value= stoll(target);
        size_t totalCost = 0;

        // iterate over pairs of characters
        for (size_t i = 0; i < target.size(); ++i) {
            // create input
            size_t depth = robotChain;
            char from = (i != 0 ? target.at(i - 1) : 'A');
            char to = target.at(i);

            // calculate cost
            size_t cost = findCost(NUMERIC, { depth, from, to });
            totalCost += cost;
        }

        complexity += totalCost * value;
    }

    return complexity;
}

int main() {
    // temporary storage
    string line;

    // store the target codes
    vector<string> targets;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        targets.push_back(line);
    }

    // display progress
    size_t firstComplexity = computeComplexity(targets, 2);
    cout << "[first] total complexity: " << firstComplexity << endl;

    // setup for iteration
    size_t secondComplexity = computeComplexity(targets, 25);
    cout << "[second] total complexity: " << secondComplexity << endl;

    return 0;
}
