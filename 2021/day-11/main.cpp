// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>

using namespace std;

const int64_t ARRAY_SIZE = 10;

typedef array<int64_t, 2> Position;

template<>
struct std::hash<Position> {
    std::size_t operator()(const Position& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

class Octopuses {
public:
    // attributes of the octopuses
    array<array<int64_t, ARRAY_SIZE>, ARRAY_SIZE> matrix;

    // attributes of the step
    queue<Position> readyToFlash;
    unordered_set<Position> flashed;

    Octopuses() {
        // temporary storage
        array<int64_t, ARRAY_SIZE> defaultRow;

        // fill row
        fill(defaultRow.begin(), defaultRow.end(), 0);

        // fill matrix
        fill(matrix.begin(), matrix.end(), defaultRow);
    }

    // step functions
    void chargeAll() {
        // iterate over matrix
        for (size_t row = 0; row < ARRAY_SIZE; row++) {
            for (size_t column = 0; column < ARRAY_SIZE; column++) {
                // assemble position
                Position position;
                position.at(0) = row;
                position.at(1) = column;

                // charge position
                chargeOne(position);
            }
        }
    }

    void flashAll() {
        // iterate until noone can flash anymore
        while (!readyToFlash.empty()) {
            // retrieve next entry
            Position current = readyToFlash.front();
            readyToFlash.pop();

            // execute flash
            flashOne(current);
        }
    }

    void cleanup() {
        // iterate over flashed octopuses
        for (const Position& entry : flashed) {
            // zero out any flashed octopus
            zeroOne(entry);
        }

        // reset flashed flags
        flashed.clear();
    }

    int64_t executeStep() {
        // charge all octopuses
        chargeAll();

        // flash all relevant octopuses
        flashAll();

        // temporary storage
        int64_t flashes = flashed.size();

        // cleanup state for next step
        cleanup();

        return flashes;
    }

    // utility functions
    void printState() {
        // iterate over matrix
        for (const array<int64_t, 10>& row : matrix) {
            for (const int64_t& energy : row) {
                cout << energy;
            }

            cout << endl;
        }

        cout << endl;
    }

private:
    // helper functions
    bool isValid(const Position& position) {
        // check for underflow
        if ((position.at(0) < 0) ||
            (position.at(1) < 0)) {
            return false;
        }

        // check for overflow
        if ((position.at(0) >= ARRAY_SIZE) ||
            (position.at(1) >= ARRAY_SIZE)) {
            return false;
        }

        return true;
    }

    // utility functions
    void chargeOne(const Position& position) {
        // check if position is valid
        if (isValid(position)) {
            // retrieve short-hands
            int64_t& currentEnergy = matrix.at(position.at(0)).at(position.at(1));

            // increase energy
            currentEnergy++;

            // check if this octopus is ready
            if ((currentEnergy > 9) &&
                (flashed.count(position) == 0)) {
                readyToFlash.push(position);
            }
        }
    }

    void zeroOne(const Position& position) {
        // check if position is valid
        if (isValid(position)) {
            // zero energy
            matrix.at(position.at(0)).at(position.at(1)) = 0;
        }
    }

    void flashOne(const Position& position) {
        if (flashed.count(position) != 0) {
            // skip already flashed octopuses
            return;
        }

        // set flashed flag
        flashed.insert(position);

        // iterate over neighbors
        for (int64_t deltaRow = -1; deltaRow <= 1; deltaRow++) {
            for (int64_t deltaColumn = -1; deltaColumn <= 1; deltaColumn++) {
                // assemble position for neighbor
                Position neighbor = position;
                neighbor.at(0) += deltaRow;
                neighbor.at(1) += deltaColumn;

                // charge neighbors
                if (neighbor != position) {
                    chargeOne(neighbor);
                }
            }
        }
    }
};

int main() {
    // temporary storage
    string line;

    // storage for octopuses
    Octopuses octopuses;

    size_t inputRow = 0;
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        for (size_t column = 0; column < octopuses.matrix.size(); column++) {
            octopuses.matrix.at(inputRow).at(column) = (line.at(column) - '0');
        }

        // shift row
        inputRow++;
    }

    // keep a running tally for first part
    size_t NUMBER_OF_STEPS = 100;
    int64_t totalFlashes = 0;

    // keep a running tally for second part
    size_t currentStep = 0;
    size_t allFlashedAtStep = 0;

    // steup iteration
    bool stepsReached = false;
    bool allFlashed = false;

    // execute iteration
    while (!stepsReached || !allFlashed) {
        // retrieve current flashes
        int64_t currentFlashes = octopuses.executeStep();

        // check for reaching all steps
        if (!stepsReached) {
            if (currentStep < NUMBER_OF_STEPS) {
                // add flashes
                totalFlashes += currentFlashes;
            } else {
                // mark flag
                stepsReached = true;
            }
        }

        // increase current step
        currentStep++;

        // check for all flashing
        if ((!allFlashed) &&
            (currentFlashes == 100)) {
            // mark step
            allFlashedAtStep = currentStep;

            // mark flag
            allFlashed = true;
        }
    }

    // display results
    cout << "# of total flashes : " << totalFlashes << endl;
    cout << "all octopuses flashed at step #" << allFlashedAtStep << endl;

    return 0;
}
