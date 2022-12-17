// C++ standard includes
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // strength at specific cycles
    vector<int64_t> registerValues;
    registerValues.push_back(0);
    registerValues.push_back(1);

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (line.empty()) {
            break;
        }

        // copy previous signal strength
        int64_t currentStrength = registerValues.back();

        // split line in two
        size_t firstSpace = line.find(' ');
        string command = line.substr(0, firstSpace);
        string tmp = line.substr(firstSpace + 1);

        // execute command
        if (command == "addx") {
            // parse parameter
            int64_t number = stoll(tmp);

            // no change at first
            registerValues.push_back(currentStrength);

            // change current strength
            currentStrength += number;

            // add changed strength
            registerValues.push_back(currentStrength);
        } else if (command == "noop") {
            // no change at first
            registerValues.push_back(currentStrength);
        }
    }

    int64_t totalSignalStrength = 0;

    vector<int64_t> interestingTimes = { 20, 60, 100, 140, 180, 220 };
    // iterate ove the times defined
    for (const int64_t& time : interestingTimes) {
        // retrieve short-hands
        const int64_t& value = registerValues.at(time);

        // calculate signal strength
        int64_t signalStrength = (time * value);

        // display interesting values
        cout << "@ cycle #" << time << ", register value : " << value << ", signal strength : " << signalStrength << endl;

        // add signal strength to total
        totalSignalStrength += signalStrength;
    }

    cout << "total signal strength : " << totalSignalStrength << endl << endl;

    // display parameters
    size_t numberOfRows = 6;
    size_t numberOfColumns = 40;

    cout << "signal analysis on display" << endl;
    // excute signal analysis row by row
    for (size_t row = 0; row < numberOfRows; row++) {
        // temporary storage
        string pixelsInRow;

        // execute signal anaylsis column by column
        for (size_t column = 0; column < numberOfColumns; column++) {
            // calculate corresponding time for screen position
            size_t timeOfPosition = (row * numberOfColumns) + column + 1;

            // query register at time
            int64_t registerAtTime = registerValues.at(timeOfPosition);

            // calculate corresponding register value for position
            int64_t registerOfPosition = (column);

            // check for sprite delta
            int64_t registerDelta = (registerAtTime - registerOfPosition);
            if ((registerDelta >= -1) &&
                (registerDelta <= 1)) {
                // pixel is lit, add it as so
                pixelsInRow.push_back('#');
            } else {
                // pixel is unlit
                pixelsInRow.push_back(' ');
            }
        }

        // display finished row
        cout << "row #" << row << " : " << pixelsInRow << endl;
    }

    return 0;
}
