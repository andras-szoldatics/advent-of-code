// C++ standard includes
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<int64_t> firstIteration(int64_t numberOfElves) {
    // setup for iteration
    vector<int64_t> presents(numberOfElves + 1, 0);

    // iterate over all the possible elves
    for (int64_t elf = 1; elf <= numberOfElves; elf++) {
        // send the elf down the line
        for (int64_t atHouse = elf; atHouse <= numberOfElves; atHouse += elf) {
            // deliver some presents
            presents.at(atHouse) += (elf * 10);
        }
    }

    return presents;
}

vector<int64_t> secondIteration(int64_t numberOfElves) {
    // setup for iteration
    vector<int64_t> presents(numberOfElves + 1, 0);

    // iterate over all the possible elves
    for (int64_t elf = 1; elf <= numberOfElves; elf++) {
        // send the elf down the line
        for (int64_t i = 1; i <= 50; i++) {
            // temporary storage
            int64_t atHouse = (elf * i);

            if (atHouse <= numberOfElves) {
                // deliver some presents
                presents.at(atHouse) += (elf * 11);
            } else {
                break;
            }
        }
    }

    return presents;
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // parse target number
    int64_t target = stoll(line);
    int64_t numberOfElves = (target / 10);

    // execute iterations
    vector<int64_t> firstPresents = firstIteration(numberOfElves);
    vector<int64_t> secondPresents = secondIteration(numberOfElves);

    // setup for iteration
    int64_t firstFound = 0;
    int64_t secondFound = 0;

    // iterate over houses
    for (int64_t atHouse = 1; atHouse <= numberOfElves; atHouse++) {
        if (firstFound == 0) {
            if (firstPresents.at(atHouse) >= target) {
                firstFound = atHouse;
            }
        }

        if (secondFound == 0) {
            if (secondPresents.at(atHouse) >= target) {
                secondFound = atHouse;
            }
        }

        if ((firstFound != 0) &&
            (secondFound != 0)) {
            break;
        }
    }

    // display results
    cout << "[first] house has loads of presents : " << firstFound << endl;
    cout << "[second] house has loads of presents : " << secondFound << endl;

    return 0;
}
