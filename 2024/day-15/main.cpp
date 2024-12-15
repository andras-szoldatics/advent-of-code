#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

array<Position, 4> DIRECTIONS_FIRST = { {
    { -1, 0 }, // UP
    { 0, 2 }, // RIGHT
    { 1, 0 }, // DOWN
    { 0, -2 } // LEFT
} };

array<Position, 4> DIRECTIONS_SECOND = { {
    { -1, 0 }, // UP
    { 0, 1 }, // RIGHT
    { 1, 0 }, // DOWN
    { 0, -1 } // LEFT
} };

class Box {
public:
    size_t index;

    Position left;
    Position right;

    Box(size_t index,
        Position start) :
        index(index),
        left(start),
        right({ start.at(0), start.at(1) + 1 }) {}

    // utility functions
    unordered_set<size_t> checkMove(const unordered_set<Position>& walls,
                   const unordered_map<Position, size_t>& lookup,
                   const vector<Box>& boxes,
                   const Position& direction) const {
        // calculate next position
        Position nextLeft = {
            left.at(0) + direction.at(0),
            left.at(1) + direction.at(1)
        };

        Position nextRight = {
            right.at(0) + direction.at(0),
            right.at(1) + direction.at(1)
        };

        // check for walls
        if ((walls.count(nextLeft) != 0) ||
            (walls.count(nextRight) != 0)) {
            return {};
        }

        // check for boxes
        unordered_set<size_t> others;
        if (lookup.count(nextLeft) != 0) {
            others.insert(lookup.at(nextLeft));
        }

        if (lookup.count(nextRight) != 0) {
            others.insert(lookup.at(nextRight));
        }

        // setup for iteration
        unordered_set<size_t> result = { index };

        for (size_t i : others) {
            // skip self
            if (i == index) {
                continue;
            }

            // retrieve short-hands
            const Box& other = boxes.at(i);

            // check for collision
            unordered_set children = other.checkMove(walls, lookup, boxes, direction);
            if (children.empty()) {
                return {};
            } else {
                result.insert(children.begin(), children.end());
            }
        }

        return result;
    }
};

vector<Box> runProgram(const array<Position, 4>& directions,
                       const unordered_set<Position>& walls,
                       const vector<size_t>& program,
                       const vector<Box>& boxes,
                       Position start) {
    // temporary storage
    Position robot = start;

    // store box state and lookup
    vector<Box> state = boxes;
    unordered_map<Position, size_t> lookup;
    for (size_t i = 0; i < state.size(); i++) {
        // retrieve short-hands
        const Box& box = state.at(i);

        // fill out lookup
        lookup[box.left] = i;
        lookup[box.right] = i;
    }

    // iterate over program
    for (size_t i = 0; i < program.size(); i++) {
        // retrieve short-hands
        const Position& direction = directions.at(program.at(i));

        // calculate next position
        Position next = {
            robot.at(0) + direction.at(0),
            robot.at(1) + direction.at(1)
        };

        // check if next position is valid
        if (walls.count(next) != 0) {
            continue;
        }

        // check if next position is a box
        if (lookup.count(next) != 0) {
            // retrieve short-hands
            size_t index = lookup.at(next);
            Box& box = state.at(index);

            // check for box movement
            unordered_set<size_t> canMove = box.checkMove(walls, lookup, state, direction);
            if (!canMove.empty()) {
                // remove boxes from lookup
                for (size_t j : canMove) {
                    // retrieve short-hands
                    Box& other = state.at(j);

                    // remove from lookup
                    lookup.erase(other.left);
                    lookup.erase(other.right);
                }

                // do the actual movement
                for (size_t j : canMove) {
                    // retrieve short-hands
                    Box& other = state.at(j);

                    // move the box
                    other.left.at(0) += direction.at(0);
                    other.left.at(1) += direction.at(1);

                    other.right.at(0) += direction.at(0);
                    other.right.at(1) += direction.at(1);

                    // update lookup
                    lookup[other.left] = j;
                    lookup[other.right] = j;
                }
            } else {
                // skip robot movement
                continue;
            }
        }

        // update robot position
        robot = next;
    }

    return state;
}

int64_t sumGPS(const vector<Box>& boxes,
               bool wide) {
    // temporary storage
    int64_t sum = 0;

    // iterate over boxes
    for (const Box& box : boxes) {
        sum += box.left.at(0) * 100 + (wide ? box.left.at(1) : box.left.at(1) / 2);
    }

    return sum;
}

int main() {
    // temporary storage
    string line;

    // warehouse storage
    unordered_set<Position> walls;
    vector<Box> boxes;
    Position robot;

    // read input file
    ifstream input("input.txt");

    // read map data
    Position cursor = { 0, 0 };
    while (getline(input, line)) {
        if (line.empty()) {
            break;
        }

        // reset column
        cursor.at(1) = 0;

        for (char c : line) {
            // collect relevant info
            if (c == '@') {
                // add robot
                robot = cursor;
                cursor.at(1) += 1;
            } else if (c == '#') {
                // add walls
                walls.insert(cursor);
                cursor.at(1) += 1;
                walls.insert(cursor);
            } else if (c == 'O') {
                // add boxes
                Box box(boxes.size(), cursor);
                cursor.at(1) += 1;
                boxes.push_back(box);
            } else {
                // shift cursor
                cursor.at(1) += 1;
            }

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // robot program
    vector<size_t> program;

    // read program
    while (getline(input, line)) {
        for (char c : line) {
            switch (c) {
            case '^':
                program.push_back(0);
                break;
            case '>':
                program.push_back(1);
                break;
            case 'v':
                program.push_back(2);
                break;
            case '<':
                program.push_back(3);
                break;
            }
        }
    }

    // run the first program
    vector<Box> firstBoxes = runProgram(DIRECTIONS_FIRST, walls, program, boxes, robot);

    // display the result
    cout << "[first] sum of GPS coordinates: " << sumGPS(firstBoxes, false) << endl;

    // run the second program
    vector<Box> secondBoxes = runProgram(DIRECTIONS_SECOND, walls, program, boxes, robot);

    // display the result
    cout << "[second] sum of GPS coordinates: " << sumGPS(secondBoxes, true) << endl;

    // run the second program
    return 0;
}
