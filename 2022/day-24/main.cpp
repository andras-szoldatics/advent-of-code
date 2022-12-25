// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

typedef array<int64_t, 2> Interval;
const Interval DEFAULT_INTERVAL = { numeric_limits<int64_t>::max(), numeric_limits<int64_t>::min() };

vector<Coordinate> DIRECTIONS = {
    { -1, 0 },
    { 1, 0 },
    { 0, -1 },
    { 0, 1 }
};

class Storm {
public:
    // simple attributes
    Coordinate position;
    size_t direction;

    Storm(const Coordinate& start,
          char type) :
        position(start) {
        // seperate types
        switch (type) {
        case '^':
            direction = 0;
            break;
        case 'v':
            direction = 1;
            break;
        case '<':
            direction = 2;
            break;
        case '>':
            direction = 3;
            break;
        default:
            // NO-OP
            (void) nullptr;
        }
    }

    // utility functions
    void move() {
        // retrieve delta
        const Coordinate& delta = DIRECTIONS.at(direction);

        // move storm
        position.front() += delta.front();
        position.back() += delta.back();
    }
};

typedef unordered_set<Coordinate> StormCoverage;

class Maze {
public:
    // attributes of this weird dimension
    unordered_map<int64_t, Interval> rowIntervals;
    unordered_map<int64_t, Interval> columnIntervals;

    // storm current state
    vector<Storm> storms;

    // storm positions over time
    vector<StormCoverage> coverageOverTime;

    // maze waypoints
    Coordinate start;
    Coordinate goal;

    Maze() {
        // add empty positions
        coverageOverTime.push_back({});

        // zero waypoints
        start.fill(0);
        goal.fill(0);
    }

    // helper functions
    void addStorm(Coordinate cursor,
                  char type) {
        // assemble storm
        Storm storm(cursor, type);

        // add storm
        storms.push_back(storm);

        // mark storm position on state 0
        coverageOverTime.at(0).insert(cursor);
    }

    const StormCoverage& getStormCoverage(int64_t time) const {
        // retrieve matching coverage
        return coverageOverTime.at(time % coverageOverTime.size());
    }

    // utility functions
    void accomodate(Coordinate cursor) {
        if ((rowIntervals.empty()) &&
            (columnIntervals.empty())) {
            // check for start coordinate
            start = cursor;
        }

        // goal is the last coordinate
        goal = cursor;

        // check for non-existing row intervals
        if (rowIntervals.count(cursor.front()) == 0) {
            rowIntervals[cursor.front()] = DEFAULT_INTERVAL;
        }

        // check for non-existing column intervals
        if (columnIntervals.count(cursor.back()) == 0) {
            columnIntervals[cursor.back()] = DEFAULT_INTERVAL;
        }

        // retrieve short-hands
        Interval& row = rowIntervals.at(cursor.front());
        Interval& column = columnIntervals.at(cursor.back());

        // shift intervals
        if (row.front() > cursor.back()) {
            row.front() = cursor.back();
        }

        if (row.back() < cursor.back()) {
            row.back() = cursor.back();
        }

        if (column.front() > cursor.front()) {
            column.front() = cursor.front();
        }

        if (column.back() < cursor.front()) {
            column.back() = cursor.front();
        }
    }

    void generateStormCoverage() {
        // retrieve row interval
        const Interval& secondRow = rowIntervals.at(3);
        const Interval& secondColumn = columnIntervals.at(3);

        // calculate synchronicity
        int64_t widthInColumns = (secondRow.back() - secondRow.front() + 1);
        int64_t heightInRows = (secondColumn.back() - secondColumn.front() + 1);
        size_t period = (widthInColumns * heightInRows);

        // reduce by common factors
        period /= gcd(widthInColumns, heightInRows);

        // iterate over possible versions
        while (coverageOverTime.size() < period) {
            // move the storms
            moveStorms();
        }
    }

private:
    // utility functions
    void moveStorms() {
        // temporary storage
        StormCoverage positions;

        // iterate over storms
        for (Storm& storm : storms) {
            // retrieve relevant intervals
            const Interval& row = rowIntervals.at(storm.position.front());
            const Interval& column = columnIntervals.at(storm.position.back());

            // move storm
            storm.move();

            // check for wraparound
            if (storm.position.front() > column.back()) {
                storm.position.front() = column.front();
            }

            if (storm.position.front() < column.front()) {
                storm.position.front() = column.back();
            }

            if (storm.position.back() > row.back()) {
                storm.position.back() = row.front();
            }

            if (storm.position.back() < row.front()) {
                storm.position.back() = row.back();
            }

            // mark storm
            positions.insert(storm.position);
        }

        // add to positions
        coverageOverTime.push_back(positions);
    }
};

enum class Moves : int8_t {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    WAIT = -127
};

const vector<Moves> MOVES = {
    Moves::RIGHT,
    Moves::DOWN,
    Moves::WAIT,
    Moves::UP,
    Moves::LEFT
};

class State {
public:
    // current attributes
    Coordinate position;
    int64_t minutesElapsed;

    // moves storage
    vector<Moves> moves;

    State(const Coordinate& start,
          int64_t startMinutes = 0) :
        position(start),
        minutesElapsed(startMinutes) {}
};

int64_t searchRoute(const Maze& maze,
                    const pair<Coordinate, Coordinate>& waypoints,
                    int64_t startMinutes = 0) {
    // setup for iteration
    list<State> states;
    State firstState(waypoints.first, startMinutes);
    states.push_front(firstState);

    // add storage for
    vector<unordered_set<Coordinate>> reachedPositions;
    reachedPositions.resize(maze.coverageOverTime.size());

    // execute BFS iteration
    while (!states.empty()) {
        // retrieve next state
        State previous = states.front();
        states.pop_front();

        // check if we reached the goal
        if (previous.position == waypoints.second) {
            return previous.minutesElapsed;
        }

        size_t cycle = (previous.minutesElapsed % maze.coverageOverTime.size());
        // check if reached this position already
        if (reachedPositions.at(cycle).count(previous.position) != 0) {
            continue;
        }

        // mark position as newly reached
        reachedPositions.at(cycle).insert(previous.position);

        // make a copy of state, just in case
        State state = previous;
        state.minutesElapsed++;

        // retrieve storm coverage
        const StormCoverage& coverage = maze.getStormCoverage(state.minutesElapsed);

        // retrieve short-hands
        const Interval& row = maze.rowIntervals.at(state.position.front());
        const Interval& column = maze.columnIntervals.at(state.position.back());

        // decide on next robot
        for (const Moves& move : MOVES) {
            // check for wait move
            if (move == Moves::WAIT) {
                // assemble next state
                State nextState = state;
                nextState.moves.push_back(move);

                // check if waiting is valid here
                if (coverage.count(nextState.position) == 0) {
                    states.push_back(nextState);
                }
            } else {
                // retrieve direction delta
                const Coordinate& delta = DIRECTIONS.at(static_cast<size_t>(move));

                // assemble next state
                State nextState = state;
                nextState.position.front() += delta.front();
                nextState.position.back() += delta.back();
                nextState.moves.push_back(move);

                // check for leaving the valley
                if (nextState.position.front() > column.back()) {
                    continue;
                }

                if (nextState.position.front() < column.front()) {
                    continue;
                }

                if (nextState.position.back() > row.back()) {
                    continue;
                }

                if (nextState.position.back() < row.front()) {
                    continue;
                }

                // check if moving is valid here
                if (coverage.count(nextState.position) == 0) {
                    states.push_back(nextState);
                }
            }
        }
    }

    return -1;
}

int main() {
    // temporary storage
    string line;
    string command;

    // maze to navigate
    Maze maze;

    // running tally of coordinates
    Coordinate cursor = { 0, 0 };

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check for end of the dimension
        if (line.empty()) {
            // read command line
            getline(input, command);
        }

        // move cursor to the new line
        cursor.front()++;
        cursor.back() = 1;

        // parse line's characters, one-by-one
        for (char c : line) {
            if (c != '#') {
                // handle cavern extent
                maze.accomodate(cursor);

                if (c != '.') {
                    maze.addStorm(cursor, c);
                }
            }

            // move cursor
            cursor.back()++;
        }
    }

    // generate all possible storms
    maze.generateStormCoverage();

    // execute search for best route
    int64_t firstToGoal = searchRoute(maze, make_pair(maze.start, maze.goal));

    // display first result
    cout << "[first] fewest minutes required : " << firstToGoal << endl;

    int64_t secondToStart = searchRoute(maze, make_pair(maze.goal, maze.start), firstToGoal);
    int64_t secondToGoal = searchRoute(maze, make_pair(maze.start, maze.goal), secondToStart);

    // display second result
    cout << "[second] fewest minutes required for the one-and-a-half roundtrip : " << secondToGoal << endl;

    return 0;
}
