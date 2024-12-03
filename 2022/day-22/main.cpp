#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
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

class Shape {
public:
    // attributes of this weird dimension
    unordered_map<int64_t, Interval> rowIntervals;
    unordered_map<int64_t, Interval> columnIntervals;

    unordered_set<Coordinate> walls;

    // mark the starting position
    Coordinate start;

    // space management functions
    void addSpace(Coordinate coordinate) {
        // stretch the intervals
        accomodate(coordinate);
    }
    void addWall(Coordinate coordinate) {
        // stretch the intervals
        accomodate(coordinate);

        // add coordinate to set of walls
        walls.insert(coordinate);
    }

private:
    // utility functions
    void accomodate(Coordinate coordinate) {
        if ((rowIntervals.empty()) &&
            (columnIntervals.empty())) {
            // check for start coordinate
            start = coordinate;
        }

        // check for non-existing row intervals
        if (rowIntervals.count(coordinate.front()) == 0) {
            rowIntervals[coordinate.front()] = DEFAULT_INTERVAL;
        }

        // check for non-existing column intervals
        if (columnIntervals.count(coordinate.back()) == 0) {
            columnIntervals[coordinate.back()] = DEFAULT_INTERVAL;
        }

        // retrieve short-hands
        Interval& row = rowIntervals.at(coordinate.front());
        Interval& column = columnIntervals.at(coordinate.back());

        // shift intervals
        if (row.front() > coordinate.back()) {
            row.front() = coordinate.back();
        }

        if (row.back() < coordinate.back()) {
            row.back() = coordinate.back();
        }

        if (column.front() > coordinate.front()) {
            column.front() = coordinate.front();
        }

        if (column.back() < coordinate.front()) {
            column.back() = coordinate.front();
        }
    }
};

vector<Coordinate> DIRECTIONS = {
    { 0, 1 },
    { 1, 0 },
    { 0, -1 },
    { -1, 0 }
};

class Decoder {
public:
    // linked dimension
    shared_ptr<Shape> dimension;
    bool secondMoveActive;

    // current state
    Coordinate position;
    int64_t direction;

    Decoder(const shared_ptr<Shape>& shape,
                 bool isCubed = false) :
        dimension(shape),
        secondMoveActive(isCubed),
        position(dimension->start),
        direction(0) {}

    // movement functions
    void moveLength(size_t length) {
        // execute multiple moves
        for (size_t i = 0; i < length; i++) {
            if (secondMoveActive) {
                moveSecond();
            } else {
                moveFirst();
            }
        }
    }

    void turnLeft() {
        // shift index
        direction--;

        // circular wraparound
        if (direction == -1) {
            direction = 3;
        }
    }

    void turnRight() {
        // shift index
        direction++;

        // circular wraparound
        if (direction == 4) {
            direction = 0;
        }
    }

    // utility functions
    void executeCommand(const string& command) {
        // temporary storage
        vector<string> moves;
        moves.push_back({});

        // iterate over command
        for (char c : command) {
            // check for specific values
            if ((c == 'L') ||
                (c == 'R')) {
                // add turn move
                moves.push_back(string(1, c));

                // add empty move
                moves.push_back({});
            } else {
                moves.back().push_back(c);
            }
        }

        // iterate over moves
        for (const string& move : moves) {
            // check for turns
            if (move == "L") {
                // turn to the left
                turnLeft();
            } else if (move == "R") {
                // turn to the right
                turnRight();
            } else {
                // convert to number
                size_t length = stoull(move);

                // move forward
                moveLength(length);
            }
        }
    }

    int64_t getPassword() {
        // calculate password
        return ((position.front() * 1000) + (position.back() * 4) + (direction));
    }

private:
    // movement functions
    void moveFirst() {
        // retrieve relevant intervals
        const Interval& row = dimension->rowIntervals.at(position.front());
        const Interval& column = dimension->columnIntervals.at(position.back());

        // generate new position
        Coordinate nextPosition = position;
        nextPosition.front() += DIRECTIONS.at(direction).front();
        nextPosition.back() += DIRECTIONS.at(direction).back();

        // check for wraparound
        if (nextPosition.front() > column.back()) {
            nextPosition.front() = column.front();
        }

        if (nextPosition.front() < column.front()) {
            nextPosition.front() = column.back();
        }

        if (nextPosition.back() > row.back()) {
            nextPosition.back() = row.front();
        }

        if (nextPosition.back() < row.front()) {
            nextPosition.back() = row.back();
        }

        // check if next position is a wall
        if (dimension->walls.count(nextPosition) == 0) {
            position = nextPosition;
        }
    }

    void moveSecond() {
        // retrieve relevant intervals
        const Interval& row = dimension->rowIntervals.at(position.front());
        const Interval& column = dimension->columnIntervals.at(position.back());

        // generate new position & direction
        Coordinate nextPosition = position;
        nextPosition.front() += DIRECTIONS.at(direction).front();
        nextPosition.back() += DIRECTIONS.at(direction).back();
        int64_t nextDirection = direction;

        // check for cube walking

        // check if next position is a wall
        if (dimension->walls.count(nextPosition)) {
            position = nextPosition;
            direction = nextDirection;
        }
    }
};

int main() {
    // temporary storage
    string line;
    string command;

    // shape of this dimension
    shared_ptr<Shape> dimension = make_shared<Shape>();

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
            if (c == '.') {
                // handle space value
                dimension->addSpace(cursor);
            } else if (c == '#') {
                // handle wall value
                dimension->addWall(cursor);
            }

            // move cursor
            cursor.back()++;
        }
    }

    // setup for first decoding
    Decoder firstDecoder(dimension);
    firstDecoder.executeCommand(command);

    // display first result
    cout << "[first] password : " << firstDecoder.getPassword() << endl;

    // setup for first decoding
    Decoder secondDecoder(dimension);
    secondDecoder.executeCommand(command);

    // display first result
    cout << "[second] password : " << firstDecoder.getPassword() << endl;

    return 0;
}
