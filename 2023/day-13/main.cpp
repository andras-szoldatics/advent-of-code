#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// utility functions
vector<size_t> findSymmetries(const vector<string>& entries) {
    // temporary storage
    vector<size_t> symmetries;

    // check each row
    for (size_t r = 1; r < entries.size(); r++) {
        // temporary storage
        bool symmetric = true;

        // check if pairwise symmetry is present
        size_t up = r - 1;
        size_t down = r;

        // check across all deltas
        for (size_t delta = 0; delta <= up; delta++) {
            // check for valid indices
            if (delta > up) {
                continue;
            }

            if (entries.size() <= down + delta) {
                continue;
            }

            // shift indices
            size_t upCursor = up - delta;
            size_t downCursor = down + delta;

            // check if symmetry is missing
            if (entries.at(upCursor) != entries.at(downCursor)) {
                // broken symmetry
                symmetric = false;
            }
        }

        if (symmetric) {
            // symmetry found
            symmetries.push_back(down);
        }
    }

    return symmetries;
}

class Patch {
public:
    vector<string> cols;
    vector<string> rows;

    // helper functions
    size_t size() const {
        return cols.size() * rows.size();
    }

    // utility functions
    void smudgeIndex(size_t i) {
        // calculate exact position
        size_t row = i / cols.size();
        size_t col = i % cols.size();

        // retrieve short-hands
        char& colEntry = cols.at(col).at(row);
        char& rowEntry = rows.at(row).at(col);

        // flip between . and #
        if (colEntry == '.') {
            colEntry = '#';
            rowEntry = '#';
        } else {
            colEntry = '.';
            rowEntry = '.';
        }
    }
};

int main() {
    // temporary storage
    string line;

    // patch storage
    vector<Patch> patches;
    patches.emplace_back();

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check for empty line
        if (line.empty()) {
            // create new patch
            patches.emplace_back();

            // skip to next line
            continue;
        }

        // add line as new row
        patches.back().rows.push_back(line);

        // resize cols if required
        if (patches.back().cols.size() < line.size()) {
            patches.back().cols.resize(line.size());
        }

        // add character to each column
        for (size_t i = 0; i < line.size(); i++) {
            patches.back().cols.at(i).push_back(line.at(i));
        }
    }

    // calculate total value of patches
    size_t totalValue = 0;
    size_t smudgeValue = 0;

    // smudge all patches
    for (Patch& patch : patches) {
        // retrieve short-hands
        size_t size = patch.size();
        vector<size_t> originalColValue = findSymmetries(patch.cols);
        vector<size_t> originalRowValue = findSymmetries(patch.rows);

        // calculate value
        for (size_t value : originalColValue) {
            totalValue += value;
        }

        for (size_t value : originalRowValue) {
            totalValue += value * 100;
        }

        // temporary storage
        bool smudgeFound = false;

        // iterate over all possible entries
        for (size_t i = 0; i < size; i++) {
            // smudge index
            patch.smudgeIndex(i);

            // retrieve values
            vector<size_t> newColValue = findSymmetries(patch.cols);
            for (size_t value : newColValue) {
                if (originalColValue.empty() ||
                    originalColValue.front() != value) {
                    smudgeValue += value;
                    smudgeFound = true;
                }
            }

            vector<size_t> newRowValue = findSymmetries(patch.rows);
            for (size_t value : newRowValue) {
                if (originalRowValue.empty() ||
                    originalRowValue.front() != value) {
                    smudgeValue += value * 100;
                    smudgeFound = true;
                }
            }

            // stop if smudge was found
            if (smudgeFound) {
                break;
            }

            // restore index
            patch.smudgeIndex(i);
        }
    }

    // print results
    cout << "[first] total value: " << totalValue << endl;
    cout << "[second] smudge value: " << smudgeValue << endl;

    return 0;
}
