#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

typedef vector<bool> Row;
typedef vector<Row> Grid;

void generateNextRow(Grid& grid) {
    // check for early return
    if (grid.empty()) {
        return;
    }

    // retrieve short-hands
    const Row& past = grid.back();

    // temporary storage
    Row next(past.size(), false);

    // iterate over past row
    for (size_t i = 0; i < past.size(); ++i) {
        // get left and right
        bool left = (i == 0) ? false : past.at(i - 1);
        bool right = (i == past.size() - 1) ? false : past.at(i + 1);

        // set next row
        next.at(i) = (left != right);
    }

    // add next row to grid
    grid.push_back(next);
}

size_t countSafeTiles(const Grid& grid) {
    // temporary storage
    size_t count = 0;

    // iterate over grid
    for (const Row& row : grid) {
        // iterate over row
        for (bool tile : row) {
            // count safe tiles
            if (!tile) {
                ++count;
            }
        }
    }

    return count;
}

int main() {
    // temporary storage
    string line;

    // storage for grid
    Grid grid;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Row row(line.size(), false);

        // process input line
        for (size_t i = 0; i < line.size(); ++i) {
            row.at(i) = (line[i] == '^');
        }

        // add as first row
        grid.push_back(row);
    }

    // setup for iteration
    size_t firstCount = 0;
    size_t FIRST_ROWS = 40;
    grid.reserve(FIRST_ROWS);

    // generate next rows
    for (size_t i = 1; i < FIRST_ROWS; ++i) {
        generateNextRow(grid);
    }

    // count safe tiles
    firstCount = countSafeTiles(grid);
    cout << "[first] # of safe tiles: " << firstCount << endl;

    // setup for iteration
    size_t secondCount = 0;
    size_t SECOND_ROWS = 400000;
    grid.reserve(SECOND_ROWS);

    // generate next rows
    for (size_t i = FIRST_ROWS; i < SECOND_ROWS; ++i) {
        generateNextRow(grid);
    }

    // count safe tiles
    secondCount = countSafeTiles(grid);
    cout << "[second] # of safe tiles: " << secondCount << endl;

    return 0;
}
