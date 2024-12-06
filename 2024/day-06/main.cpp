#include <array>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

typedef array<bool, 4> Entry;
typedef vector<Entry> Row;
typedef vector<Row> Matrix;

// extend hash functions

namespace std {
template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) +
               hash<int64_t>() (p.at(1));
    }
};

}

array<Position, 4> Directions = { {
    { -1, 0 },
    { 0, +1 },
    { +1, 0 },
    { 0, -1 }
} };

bool inBounds(const Position& widthAndHeight,
              int64_t row,
              int64_t col) {
    if ((row < 0) ||
        (row >= widthAndHeight.at(0))) {
        return false;
    }

    if ((col < 0) ||
        (col >= widthAndHeight.at(1))) {
        return false;
    }

    return true;
}

Matrix generateMatrix(const Position& widthAndHeight) {
    // setup empty visitation
    Entry entry = { false, false, false, false };
    Row row(widthAndHeight.at(1), entry);
    Matrix visited(widthAndHeight.at(0), row);

    return visited;
}

vector<Position> generatePatrol(const unordered_set<Position>& obstacles,
                                const Position& widthAndHeight,
                                const Position& start) {
    // setup for iteration
    Matrix visited = generateMatrix(widthAndHeight);

    int64_t row = start.at(0);
    int64_t col = start.at(1);
    size_t dir = 0;

    while (inBounds(widthAndHeight, row, col)) {
        // retrieve short-hands
        bool& visit = visited.at(row).at(col).at(dir);
        if (visit) {
            return {};
        } else {
            visit = true;
        }

        // generate next position
        Position next = { row, col };
        next.at(0) += Directions.at(dir).at(0);
        next.at(1) += Directions.at(dir).at(1);

        // check various directions
        while (obstacles.count(next) != 0) {
            // rotate right
            dir = (dir + 1) % 4;

            // generate next position
            next = { row, col };
            next.at(0) += Directions.at(dir).at(0);
            next.at(1) += Directions.at(dir).at(1);
        }

        // update guard position
        row = next.at(0);
        col = next.at(1);
    }

    // setup for iteration
    vector<Position> path;
    Position cursor = { 0, 0 };

    // iterate over visitation matrix
    for (const Row& r : visited) {
        // reset cursor
        cursor.at(1) = 0;

        for (const Entry& e : r) {
            if (e.at(0) ||
                e.at(1) ||
                e.at(2) ||
                e.at(3)) {
                path.push_back(cursor);
            }

            // shift cursor
            cursor.at(1) += 1;
        }

        // shift cursor
        cursor.at(0) += 1;
    }

    return path;
}

int main() {
    // temporary storage
    string line, s;

    // store map & patrol data
    unordered_set<Position> obstacles;
    Position guard;

    // read input file
    Position cursor = { 0, 0 };
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset cursor
        cursor.at(1) = 0;

        // iterate over line
        for (char c : line) {
            if (c == '#') {
                // obstacle found
                obstacles.insert(cursor);
            } else if (c == '^') {
                // guard found
                guard = cursor;
            }

            // shift cursor
            cursor.at(1) += 1;
        }

        // shift cursor
        cursor.at(0) += 1;
    }

    vector<Position> visited = generatePatrol(obstacles, cursor, guard);
    cout << "[first] total positions visited: " << visited.size() << endl;

    // setup for iteration
    int64_t secondTotal = 0;

    // iterate over possible obstructions
    for (const Position& obstruction : visited) {
        // skip initial guard position
        if (obstruction == guard) {
            continue;
        }

        // add obstruction
        obstacles.insert(obstruction);

        // check patrol for a loop
        if (generatePatrol(obstacles, cursor, guard).empty()) {
            secondTotal += 1;
        }

        // remove obstruction
        obstacles.erase(obstruction);
    }

    cout << "[second] total loops found: " << secondTotal << endl;

    return 0;
}
