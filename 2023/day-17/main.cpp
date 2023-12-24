#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

// extend hash function to Position, Ray

namespace std {

template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        size_t bitfield = hash<int64_t>() (p.front());
        bitfield += hash<int64_t>() (p.back());
        return bitfield;
    }
};

} // namespace std

enum class Direction : uint8_t {
    NONE = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Crucible {
public:
    // current position
    Position position;

    // crucible settings
    int64_t minimumMoves;
    int64_t maximumMoves;

    // history data
    Direction direction;
    int64_t count;

    // Dijkstra data
    int64_t heatLoss;

    Crucible(Position p,
             int64_t minMoves,
             int64_t maxMoves) :
        position(p),
        minimumMoves(minMoves),
        maximumMoves(maxMoves),
        direction(Direction::NONE),
        count(minMoves),
        heatLoss(0) {}

    // helper functions
    string getHashKey() const {
        // convert position to string
        string key = to_string(position.front());
        key += ",";
        key += to_string(position.back());

        // convert direction to string
        key += "@";
        key += to_string(static_cast<uint8_t>(direction));
        key += ",";
        key += to_string(count);

        return key;
    }

    // operator functions
    bool operator<(const Crucible& other) const {
        // compare heat loss for priority queue
        return (heatLoss > other.heatLoss);
    }
};

class HeatMap {
public:
    // store heat at a given position
    unordered_map<Position, int64_t> entries;

    // helper functions
    optional<Crucible> moveOne(const Crucible& state,
                               Direction direction) const {
        // filter out invalid moves - MINIMUM
        if ((state.direction != direction) &&
            (state.count < state.minimumMoves)) {
            return nullopt;
        }

        // filter out invalid moves - MAXIMUM
        if ((state.direction == direction) &&
            (state.count >= state.maximumMoves)) {
            return nullopt;
        }

        // calculate new position
        Position nextPosition = state.position;
        switch (direction) {
        case Direction::UP:
            // cannot directly reverse
            if (state.direction == Direction::DOWN) {
                return nullopt;
            }

            nextPosition.front() -= 1;
            break;
        case Direction::DOWN:
            // cannot directly reverse
            if (state.direction == Direction::UP) {
                return nullopt;
            }

            nextPosition.front() += 1;
            break;
        case Direction::LEFT:
            // cannot directly reverse
            if (state.direction == Direction::RIGHT) {
                return nullopt;
            }

            nextPosition.back() -= 1;
            break;
        case Direction::RIGHT:
            // cannot directly reverse
            if (state.direction == Direction::LEFT) {
                return nullopt;
            }

            nextPosition.back() += 1;
            break;
        default:
            return nullopt;
        }

        // check position
        if (entries.count(nextPosition) == 0) {
            return nullopt;
        }

        // create new state
        Crucible nextState = state;
        nextState.position = nextPosition;
        if (state.direction == direction) {
            nextState.count += 1;
        } else {
            nextState.direction = direction;
            nextState.count = 1;
        }

        // account for heat loss
        nextState.heatLoss += entries.at(nextPosition);

        return nextState;
    }

    // utility functions
    vector<Crucible> splitStates(const Crucible& state) const {
        // temporary storage
        vector<Crucible> states;

        // generate all possible states
        for (uint8_t i = 1; i <= 4; i++) {
            // temporary storage
            Direction direction = static_cast<Direction>(i);
            optional<Crucible> nextState = moveOne(state, direction);

            if (nextState.has_value()) {
                states.push_back(nextState.value());
            }
        }

        return states;
    }
};

int64_t findBestPath(const HeatMap& heatMap,
                     int64_t minMoves,
                     int64_t maxMoves,
                     const Position& start,
                     const Position& end) {
    // setup for iteration
    priority_queue<Crucible> queue;
    unordered_map<string, int64_t> bestVisit;

    // create initial state
    Crucible initialState(start, minMoves, maxMoves);
    queue.push(initialState);

    // search for end position
    while (!queue.empty()) {
        // pop next state
        Crucible state = queue.top();
        queue.pop();

        // mark visited tiles
        string key = state.getHashKey();
        if (bestVisit.count(key) == 0) {
            bestVisit[key] = state.heatLoss;
        } else {
            continue;
        }

        // check for end position
        if ((state.position == end) &&
            (state.count >= state.minimumMoves)) {
            return state.heatLoss;
        }

        // split state into multiple states
        vector<Crucible> states = heatMap.splitStates(state);
        for (const Crucible& nextState : states) {
            queue.push(nextState);
        }
    }

    return -1;
}

int main() {
    // temporary storage
    string line;
    Position cursor = { 0, 0 };

    // cave storage
    HeatMap heatMap;
    Position start = { 0, 0 };
    Position end = { 0, 0 };

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (heatMap.entries.empty()) {
            // pre-allocate map
            heatMap.entries.reserve(line.size() * line.size());
        }

        // parse line
        for (size_t i = 0; i < line.size(); i++) {
            // store heat
            heatMap.entries[cursor] = line[i] - '0';
            end = cursor;

            // update cursor
            cursor.back() += 1;
        }

        // update cursor
        cursor.front() += 1;
        cursor.back() = 0;
    }

    // find best path
    int64_t firstBest = findBestPath(heatMap, 1, 3, start, end);

    // display result
    cout << "[first] best path: " << firstBest << endl;

    // find best path
    int64_t secondBest = findBestPath(heatMap, 4, 10, start, end);

    // display result
    cout << "[second] best path: " << secondBest << endl;

    return 0;
}
