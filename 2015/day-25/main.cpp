#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main() {
    // temporary storage
    string line, tmp;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // parse parameters
    int64_t targetRow = 0;
    int64_t targetColumn = 0;

    // assign line to buffer
    stringstream buffer(line);

    // skip to relevant part
    getline(buffer, tmp, 'w');

    // parse row
    getline(buffer, tmp, ',');
    targetRow = stoll(tmp);

    // skip to relevant part
    getline(buffer, tmp, 'n');

    // parse column
    getline(buffer, tmp, '.');
    targetColumn = stoll(tmp);

    // setup for iteration
    int64_t row = 1;
    int64_t column = 1;
    int64_t value = 20151125;
    bool doIteration = true;

    // example iteration
    while (doIteration) {
        // shift to next position
        row--;
        column++;

        if (row == 0) {
            // handle wrap-around
            row = column;
            column = 1;
        }

        // generate new value
        value *= 252533;
        value %= 33554393;

        // check for target position
        if ((row == targetRow) &&
            (column == targetColumn)) {
            doIteration = false;
        }
    }

    cout << "[first] final value : " << value << endl;

    return 0;
}
