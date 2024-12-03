#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

size_t getCombinations(const string& p,
                       const vector<size_t>& s,
                       size_t extension = 0) {
    // allocate final pattern
    string pattern = p;
    for (size_t i = 0; i < extension; i++) {
        pattern += '?';
        pattern += p;
    }

    // add one more space, for easier handling
    pattern += '.';

    // allocate final springs
    vector<size_t> springs = s;
    for (size_t i = 0; i < extension; i++) {
        springs.insert(springs.end(), s.begin(), s.end());
    }

    // calculate prefix sum
    vector<size_t> prefixSum(pattern.size() + 1, 0);
    for (size_t i = 0; i < pattern.size(); i++) {
        size_t delta = (pattern.at(i) != '.' ? 1 : 0);
        prefixSum.at(i + 1) = prefixSum.at(i) + delta;
    }

    // calculate wiggle room
    size_t wiggleRoom = pattern.size();
    wiggleRoom -= springs.size();
    wiggleRoom += 1;
    for (size_t spring : springs) {
        wiggleRoom -= spring;
    }

    // temporary storage
    vector<size_t> emptyRow(pattern.size() + 1, 0);
    vector<vector<size_t>> combinations(1, emptyRow);
    size_t size = springs.front();
    size_t sum = 0;
    bool valid = true;

    // handle first row of things
    for (size_t i = 0; i < wiggleRoom; i++) {
        // check interval validity
        if (pattern.at(i + size) == '#') {
            sum = 0;
        } else if ((valid) &&
                   (prefixSum.at(i + size) - prefixSum.at(i) == size)) {
            sum += 1;
        }

        // fill out table
        combinations.at(0).at(i + size) = sum;

        // check if this is the first pattern
        if (pattern.at(i) == '#') {
            valid = false;
        }
    }

    // handle remaining rows
    size_t startIndex = size + 1;

    for (size_t rowIndex = 1; rowIndex < springs.size(); rowIndex++) {
        // retrieve size
        size = springs.at(rowIndex);

        // allocate new row
        const vector<size_t>& previous = combinations.back();
        vector<size_t> current(pattern.size() + 1, 0);

        // reset running sum
        sum = 0;

        for (size_t i = startIndex; i < startIndex + wiggleRoom; i++) {
            // check pattern validity
            if (pattern.at(i + size) == '#') {
                sum = 0;
            } else if ((previous.at(i - 1) > 0) &&
                       (pattern.at(i - 1) != '#') &&
                       (prefixSum.at(i + size) - prefixSum.at(i) == size)) {
                sum += previous.at(i - 1);
            }

            // mark sum
            current.at(i + size) = sum;
        }

        // update table
        combinations.push_back(current);

        // update start index
        startIndex += size + 1;
    }

    return sum;
}

int main() {
    // temporary storage line
    string line;

    // setup for iteration
    size_t firstResult = 0;
    size_t secondResult = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line into buffer
        stringstream buffer(line);
        string pattern;
        vector<size_t> springs;

        // retrieve relevant information
        getline(buffer, pattern, ' ');

        // temporary storage
        string tmp;
        while (getline(buffer, tmp, ',')) {
            size_t spring = stoull(tmp);
            springs.push_back(spring);
        }

        // calculate combinations
        firstResult += getCombinations(pattern, springs);
        secondResult += getCombinations(pattern, springs, 4);
    }

    // display result
    cout << "[first] number of results: " << firstResult << endl;
    cout << "[second] number of results: " << secondResult << endl;

    return 0;
}
