#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

size_t calculate(unordered_map<string, size_t>& possibilities,
                 const vector<string>& towels,
                 const string& target) {
    // check for end
    if (target.empty()) {
        return 1;
    }

    // check the cache
    if (possibilities.count(target) != 0) {
        return possibilities.at(target);
    }

    // setup for iteration
    size_t total = 0;

    // check designs
    for (const string& towel : towels) {
        string check = target.substr(0, towel.size());

        // check if design is possible
        if (check == towel) {
            // calculate the variations
            string next = target.substr(towel.size());
            size_t result = calculate(possibilities, towels, next);

            // store the result
            total += result;
        }
    }

    // store the result
    possibilities[target] = total;

    return total;
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    unordered_map<string, size_t> possibilities;
    vector<string> towels;
    vector<string> designs;

    // read input file
    ifstream input("input.txt");
    size_t mode = 0;

    while (getline(input, line)) {
        // switch modes
        if (line.empty()) {
            mode += 1;
            continue;
        }

        if (mode == 0) {
            // assign buffer
            stringstream buffer(line);
            string s;

            // read each word
            while (getline(buffer, s, ',')) {
                // store towels
                towels.push_back(s);

                // skip to next word
                getline(buffer, s, ' ');
            }
        } else if (mode == 1) {
            // store designs
            designs.push_back(line);
        }
    }

    // setup for iteration
    size_t valid = 0;

    // calculate all possibilities
    for (const string& design : designs) {
        size_t result = calculate(possibilities, towels, design);

        if (result != 0) {
            valid += 1;
        }
    }

    // display results
    cout << "[first] # of valid designs: " << valid << endl;

    // setup for iteration
    size_t numberOfWays = 0;

    // count valid designs
    for (const string& design : designs) {
        if (possibilities.count(design) != 0) {
            numberOfWays += possibilities.at(design);
        }
    }

    // display results
    cout << "[second] # of ways to make a valid design: " << numberOfWays << endl;

    return 0;
}
