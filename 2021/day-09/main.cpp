#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

void fillBasin(vector<vector<int64_t>>& basinMap,
               size_t row,
               size_t column,
               int64_t basinIndex) {
    // retrieve short-hands
    int64_t currentIndex = basinMap.at(row).at(column);
    if (currentIndex != 0) {
        return;
    } else {
        basinMap.at(row).at(column) = basinIndex;
    }

    if (row > 0) {
        fillBasin(basinMap, row - 1, column, basinIndex);
    }

    if (row + 1 < basinMap.size()) {
        fillBasin(basinMap, row + 1, column, basinIndex);
    }

    if (column > 0) {
        fillBasin(basinMap, row, column - 1, basinIndex);
    }

    if (column + 1 < basinMap.at(row).size()) {
        fillBasin(basinMap, row, column + 1, basinIndex);
    }
}

void processBasins(vector<vector<int64_t>>& basinMap) {
    // temporary storage
    unordered_set<int64_t> processed = { -1, 0 };

    // iterate over rows
    for (size_t row = 0; row < basinMap.size(); row++) {
        // iterate over columns
        for (size_t column = 0; column < basinMap.at(row).size(); column++) {
            // retrieve short-hands
            int64_t basinIndex = basinMap.at(row).at(column);

            // find a unique value that is not already processed
            if (processed.count(basinIndex) == 0) {
                // fill basin with index
                basinMap.at(row).at(column) = 0;
                fillBasin(basinMap, row, column, basinIndex);

                // mark index as processed
                processed.insert(basinIndex);
            }
        }
    }
}

int main() {
    // temporary storage
    string line;

    // store cavern data
    vector<vector<int64_t>> heightMap;

    // store basin data
    vector<vector<int64_t>> basinMap;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        vector<int64_t> heightRow;
        vector<int64_t> basinRow;

        // process line into row
        for (char c : line) {
            int64_t height = static_cast<int64_t>(c - '0');
            heightRow.push_back(height);
            if (height == 9) {
                basinRow.push_back(-1);
            } else {
                basinRow.push_back(0);
            }
        }

        // add row to map
        heightMap.push_back(heightRow);
        basinMap.push_back(basinRow);
    }

    int64_t basinIndex = 1;
    int64_t riskLevel = 0;
    // iterate over rows
    for (size_t row = 0; row < heightMap.size(); row++) {
        // retrieve short-hands
        const vector<int64_t>& heightRow = heightMap.at(row);

        // iterate over elements
        for (size_t column = 0; column < heightRow.size(); column++) {
            // temporary storage
            bool lowPoint = true;

            // retrieve comparison values
            const int64_t& height = heightRow.at(column);

            if (row > 0) {
                if (height >= heightMap.at(row - 1).at(column)) {
                    lowPoint = false;
                }
            }

            if (row + 1 < heightMap.size()) {
                if (height >= heightMap.at(row + 1).at(column)) {
                    lowPoint = false;
                }
            }

            if (column > 0) {
                if (height >= heightRow.at(column - 1)) {
                    lowPoint = false;
                }
            }

            if (column + 1 < heightRow.size()) {
                if (height >= heightRow.at(column + 1)) {
                    lowPoint = false;
                }
            }

            // check if this is a low point
            if (lowPoint) {
                riskLevel += (height + 1);

                basinMap.at(row).at(column) = basinIndex++;
            }
        }
    }

    cout << "total risk level of low pooints : " << riskLevel << endl;

    // process basins
    processBasins(basinMap);

    // basin numbers
    vector<size_t> basins;

    // iterate over rows
    for (const vector<int64_t>& basinRow : basinMap) {
        // iterate over entries
        for (int64_t entry : basinRow) {
            if (entry <= 0) {
                continue;
            }

            // check if vector is big enough
            if (basins.size() <= static_cast<size_t>(entry)) {
                basins.resize(entry + 1, 0);
            }

            // handle entry
            basins.at(entry)++;
        }
    }

    // sort basins
    sort(basins.begin(), basins.end(), greater<size_t>());

    cout << "result of three largest basins multiplied : " << (basins.at(0) * basins.at(1) * basins.at(2)) << endl;

    return 0;
}
