// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

typedef vector<int64_t> Row;
typedef vector<Row> Matrix;

enum class Operation : int8_t {
    UNDEFINED = 0,
    TURN_ON,
    TURN_OFF,
    TOGGLE
};

void executeFirst(Matrix& lights,
                  Operation operation,
                  Position from,
                  Position to) {
    // iterate along rows
    for (int64_t row = from.at(1); row <= to.at(1); row++) {
        // iterate over columns
        for (int64_t column = from.at(0); column <= to.at(0); column++) {
            // retrieve short-hands
            int64_t& state = lights.at(row).at(column);

            // execute operation
            switch (operation) {
            case Operation::TURN_ON:
                state = 1;
                break;
            case Operation::TURN_OFF:
                state = 0;
                break;
            case Operation::TOGGLE:
                if (state == 0) {
                    state = 1;
                } else {
                    state = 0;
                }
                break;
            default:
                (void) nullptr;
            }
        }
    }
}

void executeSecond(Matrix& lights,
                   Operation operation,
                   Position from,
                   Position to) {
    // iterate along rows
    for (int64_t row = from.at(1); row <= to.at(1); row++) {
        // iterate over columns
        for (int64_t column = from.at(0); column <= to.at(0); column++) {
            // retrieve short-hands
            int64_t& state = lights.at(row).at(column);

            // execute operation
            switch (operation) {
            case Operation::TURN_ON:
                state += 1;
                break;
            case Operation::TURN_OFF:
                if (state >= 1) {
                    state -= 1;
                }
                break;
            case Operation::TOGGLE:
                state += 2;
                break;
            default:
                (void) nullptr;
            }
        }
    }
}

int main() {
    // temporary storage
    string line;

    // matrix of lights
    Row templateRow(1000, 0);

    Matrix lightsFirst(1000, templateRow);
    Matrix lightsSecond(1000, templateRow);

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Operation operation = Operation::UNDEFINED;
        Position from, to;
        from.fill(0);
        to.fill(0);

        // assign line into buffer
        stringstream buffer(line);
        string tmp;

        // parse next part of line
        getline(buffer, tmp, ' ');
        if (tmp == string("turn")) {
            // check out next word in line
            getline(buffer, tmp, ' ');

            if (tmp == string("on")) {
                operation = Operation::TURN_ON;
            } else if (tmp == string("off")) {
                operation = Operation::TURN_OFF;
            }
        } else if (tmp == string("toggle")) {
            operation = Operation::TOGGLE;
        }

        // parse next part of line
        getline(buffer, tmp, ',');
        from.at(0) = stoll(tmp);

        // parse next part of line
        getline(buffer, tmp, ' ');
        from.at(1) = stoll(tmp);

        // skip next part
        getline(buffer, tmp, ' ');

        // parse next part of line
        getline(buffer, tmp, ',');
        to.at(0) = stoll(tmp);

        // parse next part of line
        getline(buffer, tmp);
        to.at(1) = stoll(tmp);

        // execute operation
        executeFirst(lightsFirst, operation, from, to);
        executeSecond(lightsSecond, operation, from, to);
    }

    // calculate light count
    size_t lightsLit = 0;
    for (const Row& row : lightsFirst) {
        for (const int64_t& entry : row) {
            if (entry != 0) {
                lightsLit += 1;
            }
        }
    }

    // display first part result
    cout << "# of lights lit : " << lightsLit << endl;

    // calculate light total
    size_t totalBrightness = 0;

    for (const Row& row : lightsSecond) {
        for (const int64_t& entry : row) {
            if (entry) {
                totalBrightness += entry;
            }
        }
    }

    // display second part result
    cout << "total brightness of lights : " << totalBrightness << endl;

    return 0;
}
