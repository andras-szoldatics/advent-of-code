// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    // temporary storage
    string line;

    // marker length const
    const size_t FIRST_LENGTH = 4;
    const size_t SECOND_LENGTH = 14;

    // read input file
    ifstream input("input.txt");

    // read the one and only relevant line
    getline(input, line);

    bool firstFound = false;
    bool secondFound = false;
    // do something with the line
    for (size_t i = 0; i < line.size(); i++) {
        if (!firstFound) {
            // retrieve a substring from the code
            string firstWindow = line.substr(i, FIRST_LENGTH);

            // check for unique values
            sort(firstWindow.begin(), firstWindow.end());
            string::iterator threshold = unique(firstWindow.begin(), firstWindow.end());
            size_t firstSize = distance(firstWindow.begin(), threshold);

            // check if unique size matches
            if (firstSize == FIRST_LENGTH) {
                cout << "found first marker : " << (i + FIRST_LENGTH) << endl;
                firstFound = true;
            }
        }

        if (!secondFound) {
            // retrieve a sbustring from the code
            string secondWindow = line.substr(i, SECOND_LENGTH);

            // check for unique values
            sort(secondWindow.begin(), secondWindow.end());
            string::iterator threshold = unique(secondWindow.begin(), secondWindow.end());
            size_t firstSize = distance(secondWindow.begin(), threshold);

            // check if unique size matches
            if (firstSize == SECOND_LENGTH) {
                cout << "found second marker : " << (i + SECOND_LENGTH) << endl;
                secondFound = true;
            }
        }

        // check for early termination
        if (firstFound && secondFound) {
            break;
        }
    }

    return 0;
}
