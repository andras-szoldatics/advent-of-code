#include <cstdint>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

using namespace std;

typedef array<int64_t, 3> Triangle;

bool isPossible(const Triangle& triangle) {
    // calculate helper values
    int64_t totalSides = accumulate(triangle.begin(), triangle.end(), 0l);
    int64_t largestSide = *max_element(triangle.begin(), triangle.end());

    return (totalSides > (largestSide * 2));
}

int main() {
    // temporary storage
    string line, tmp;

    // all the values in order
    vector<int64_t> values;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        while (getline(buffer, tmp, ' ')) {
            // filter parts
            if (!tmp.empty()) {
                // parse as int
                int64_t value = stoll(tmp);

                // add to list
                values.push_back(value);
            }
        }

    }

    // keep a running tally
    int64_t firstTriangles = 0;
    int64_t secondTriangles = 0;

    // iterate by lines
    for (size_t i = 0; i < values.size(); i += 3) {
        // formulate triangle
        Triangle first = { values.at(i), values.at(i + 1), values.at(i + 2) };

        // check triangle
        if (isPossible(first)) {
            firstTriangles += 1;
        }
    }

    // iterate by columns
    for (size_t i = 0; i < values.size(); i += 1) {
        // formulate triangle
        Triangle second = { values.at(i), values.at(i + 3), values.at(i + 6) };

        // check triangle
        if (isPossible(second)) {
            secondTriangles += 1;
        }

        // big jump at the end of block
        if (i % 3 == 2) {
            i += 6;
        }
    }

    // display results
    cout << "[first] possible triangles : " << firstTriangles << endl;
    cout << "[second] possible triangles : " << secondTriangles << endl;

    return 0;
}
