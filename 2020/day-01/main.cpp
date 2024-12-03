#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // accounting numbers
    vector<int64_t> accounting;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line into accounting
        int64_t number = stoll(line);
        accounting.push_back(number);
    }

    bool firstFound = false;
    int64_t firstResult = 0;

    bool secondFound = false;
    int64_t secondResult = 0;

    // iterate over numbers
    for (const int64_t& first : accounting) {
        for (const int64_t& second : accounting) {
            for (const int64_t& third : accounting) {
                // check the sum of two inside numbers
                if ((!firstFound) &&
                    ((second + third) == 2020)) {
                    // first result found
                    firstResult = (second * third);
                    firstFound = true;
                }

                // check sum of three numbers
                if ((!secondFound) &&
                    ((first + second + third) == 2020)) {
                    // second result found
                    secondResult = (first * second * third);
                    secondFound = true;
                }

                // check for termination
                if (firstFound && secondFound) {
                    break;
                }
            }

            // check for termination
            if (firstFound && secondFound) {
                break;
            }
        }

        // check for termination
        if (firstFound && secondFound) {
            break;
        }
    }

    cout << "first result : " << firstResult << endl;
    cout << "second result : " << secondResult << endl;

    return 0;
}
