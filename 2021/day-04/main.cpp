#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

class Table {
public:
    // values for table
    array<array<int64_t, 5>, 5> values;

    // status of table
    array<array<bool, 5>, 5> drawn;
    size_t drawnCount;

    Table() :
        drawnCount(0) {
        // temporary vector
        array<bool, 5> row;
        fill(row.begin(), row.end(), false);

        // fill drawn matrix
        drawn.fill(row);
    }

    void drawValue(const int64_t& value) {
        // iterate over array
        for (size_t row = 0; row < 5; row++) {
            for (size_t column = 0; column < 5; column++) {
                if (values.at(row).at(column) == value) {
                    // set tite as drawn
                    drawn.at(row).at(column) = true;
                }
            }
        }
    }

    int64_t totalNotDrawn() {
        // temporary storage
        int64_t total = 0;

        // iterate over array
        for (size_t row = 0; row < 5; row++) {
            for (size_t column = 0; column < 5; column++) {
                // check if this is drawn
                if (!drawn.at(row).at(column)) {
                    total += values.at(row).at(column);
                }
            }
        }

        return total;
    }

    bool isWinning() {
        // iterate over rows & columns
        for (size_t i = 0; i < 5; i++) {
            // check for full row
            if ((drawn.at(i).at(0)) &&
                (drawn.at(i).at(1)) &&
                (drawn.at(i).at(2)) &&
                (drawn.at(i).at(3)) &&
                (drawn.at(i).at(4))) {
                return true;
            }

            // check for full column
            if ((drawn.at(0).at(i)) &&
                (drawn.at(1).at(i)) &&
                (drawn.at(2).at(i)) &&
                (drawn.at(3).at(i)) &&
                (drawn.at(4).at(i))) {
                return true;
            }
        }

        // iterate over columns

        return false;
    }

};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read first line
    getline(input, line);

    vector<int64_t> bingoPulls;
    if (!line.empty()) {
        // put line into a buffer
        stringstream buffer(line);
        string stringPull;

        // parse bingo pulls in order
        while (getline(buffer, stringPull, ',')) {
            // convert string into int
            int64_t pull = stoll(stringPull);
            bingoPulls.push_back(pull);
        }
    }

    // list of tables
    vector<Table> tables;

    size_t row = 0;
    // iterate over bingo tables
    while (getline(input, line)) {
        // check for empty line
        if (line.empty()) {
            // create new table
            Table table;

            // push back new table
            tables.push_back(table);

            // reset row index
            row = 0;

            continue;
        }

        // put line into a buffer
        stringstream buffer(line);
        string stringValue;

        size_t column = 0;
        // add values to bingo table
        while (getline(buffer, stringValue, ' ')) {
            if (!stringValue.empty()) {
                // parse value
                int64_t value = stoll(stringValue);

                // set value on array
                tables.back().values.at(row).at(column++) = value;
            }
        }

        // shift rows down
        row++;
    }

    unordered_set<size_t> winners;
    // iterate over bingo pulls
    for (int64_t bingoPull : bingoPulls) {
        // iterate over tables
        for (size_t i = 0; i < tables.size(); i++) {
            // skip already won tables
            if (winners.count(i) != 0) {
                continue;
            }

            // retrieve short-hands
            Table& table = tables.at(i);

            // draw number
            table.drawValue(bingoPull);

            // check for win
            if (table.isWinning()) {
                // mark as winner
                winners.insert(i);

                if (winners.size() == 1) {
                    cout << "[first] winning table : " << i << ", with final score : " << (bingoPull * table.totalNotDrawn()) << endl;
                } else if (winners.size() == tables.size()) {
                    cout << "[last] winning table : " << i << ", with final score : " << (bingoPull * table.totalNotDrawn()) << endl;
                }
            }
        }
    }

    return 0;
}
