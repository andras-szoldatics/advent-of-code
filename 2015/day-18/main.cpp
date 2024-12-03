#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

typedef array<int64_t, 2> Position;

template<>
struct std::hash<Position> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

class Lights {
public:
    // state storage
    int64_t matrixSize;
    unordered_set<Position> shining;

    Lights() :
        matrixSize(0) {}

    // helper functions
    void setSize(int64_t size) {
        if (matrixSize < size) {
            matrixSize = size;
        }
    }

    void flipOn(const Position& position) {
        // filter position
        if ((position.front() >= 1) &&
            (position.front() <= matrixSize) &&
            (position.back() >= 1) &&
            (position.back() <= matrixSize)) {
            // turn on light
            shining.insert(position);
        }
    }

    // utility functions
    void step() {
        // temporary storage
        unordered_set<Position> past = shining;
        unordered_map<Position, size_t> neighbours;

        // reset shining state
        shining.clear();

        // iterate over past positions
        for (const Position& position : past) {
            // iterate over neighbours
            for (int64_t row = -1; row <= 1; row++) {
                for (int64_t column = -1; column <= 1; column++) {
                    // filter original position
                    if ((row != 0) ||
                        (column != 0)) {
                        // temporary storage
                        Position shifted = position;
                        shifted.front() += row;
                        shifted.back() += column;

                        // add to neighbour map
                        if (neighbours.count(shifted) != 0) {
                            neighbours.at(shifted) += 1;
                        } else {
                            neighbours[shifted] = 1;
                        }
                    }
                }
            }
        }

        // iterate over neigbours
        for (const pair<const Position, size_t>& entry : neighbours) {
            // check for shining state
            if ((entry.second == 3) ||
                (((entry.second == 2) &&
                  (past.count(entry.first) != 0)))) {
                flipOn(entry.first);
            }
        }
    }

    void flipOnCorners() {
        // handle corner cases
        flipOn({ 1, 1 });
        flipOn({ 1, matrixSize });
        flipOn({ matrixSize, 1 });
        flipOn({ matrixSize, matrixSize });
    }
};

int main() {
    // temporary storage
    string line;

    // lights state
    Position current = { 1, 1 };
    Lights firstLights;
    Lights secondLights;

    // read input filef
    ifstream input("input.txt");
    while (getline(input, line)) {
        // set matrix width
        int64_t matrixSize = static_cast<int64_t>(line.size());
        firstLights.setSize(matrixSize);
        secondLights.setSize(matrixSize);

        // process line
        for (char c : line) {
            if (c == '#') {
                firstLights.flipOn(current);
                secondLights.flipOn(current);
            }

            // shift column
            current.back() += 1;
        }

        // shift row
        current.front() += 1;
        current.back() = 1;
    }

    // setup for iteration
    size_t NUMBER_OF_STEPS = 100;

    // flip on corners
    secondLights.flipOnCorners();

    // iterate step-by-step
    for (size_t i = 1; i <= NUMBER_OF_STEPS; i++) {
        // process first matrix
        firstLights.step();

        // process second matrix
        secondLights.step();
        secondLights.flipOnCorners();
    }

    // display results
    cout << "[first] lights are on after hundred steps : " << firstLights.shining.size() << endl;
    cout << "[second] lights are on after hundred steps : " << secondLights.shining.size() << endl;

    return 0;
}
