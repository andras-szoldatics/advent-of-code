#include <cstdint>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // vector of fishies
    array<size_t, 9> fishies;
    fill(fishies.begin(), fishies.end(), 0);

    // read line of fishies
    getline(input, line);

    // assign line as buffer
    stringstream buffer(line);
    string stringValue;

    // iterate over line
    while (getline(buffer, stringValue, ',')) {
        // parse value
        int64_t fishy = stoll(stringValue);
        fishies.at(fishy)++;
    }

    // define the number of days
    size_t firstNumberOfDays = 80;
    size_t secondNumberOfDays = 256;

    // iterate over the day
    for (size_t i = 1; i <= secondNumberOfDays; i++) {
        // number of new fishies
        array<size_t, 9> newFishies;
        fill(newFishies.begin(), newFishies.end(), 0);

        // iterate over fishies
        for (size_t i = 0; i < fishies.size(); i++) {
            if (i == 0) {
                // new fishies are needed
                newFishies.at(6) += fishies.at(0);
                newFishies.at(8) += fishies.at(0);
            } else {
                // shift the fishies down
                newFishies.at(i - 1) += fishies.at(i);
            }
        }

        // apply new fishies
        fishies = newFishies;

        if ((i == firstNumberOfDays) ||
            (i == secondNumberOfDays)) {
            size_t totalFishies = 0;

            // count the fishies
            for (size_t fishy : fishies) {
                totalFishies += fishy;
            }

            cout << "after day #" << i << ", total # of fishies : " << totalFishies << endl;
        }
    }

    return 0;
}
