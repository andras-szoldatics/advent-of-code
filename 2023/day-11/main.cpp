#include <array>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<size_t, 2> Point;

// extend hash function to Point

namespace std {

template<>
struct hash<Point> {
    size_t operator()(const Point& p) const {
        size_t bitfield = hash<size_t>() (p.front());
        bitfield += hash<size_t>() (p.back());
        return bitfield;
    }
};

} // namespace std

class StarMap {
public:
    // store galaxies
    vector<Point> galaxies;

    // store empty spaces
    unordered_set<size_t> emptyRows;
    unordered_set<size_t> emptyCols;

    // store extent of map
    size_t rows;
    size_t cols;

    StarMap() :
        rows(0),
        cols(0) {}

    // utility functions
    void findEmptiness() {
        // fill empty rows
        for (size_t r = 0; r < rows; r++) {
            emptyRows.insert(r);
        }

        // fill empty cols
        for (size_t c = 0; c < cols; c++) {
            emptyCols.insert(c);
        }

        // iterate over galaxies
        for (const Point& galaxy : galaxies) {
            emptyRows.erase(galaxy.front());
            emptyCols.erase(galaxy.back());
        }
    }

    size_t calculateShortDistances() {
        // temporary storage
        size_t result = 0;

        // iterate over pairs of galaxies
        for (size_t from = 0; from < galaxies.size(); from++) {
            for (size_t to = from + 1; to < galaxies.size(); to++) {
                // find distance between galaxies
                size_t distance = findDistance(galaxies[from], galaxies[to], 1);

                // update result
                result += distance;
            }
        }

        return result;
    }

    size_t calculateLongDistances() {
        // temporary storage
        size_t result = 0;

        // iterate over pairs of galaxies
        for (size_t from = 0; from < galaxies.size(); from++) {
            for (size_t to = from + 1; to < galaxies.size(); to++) {
                // find distance between galaxies
                size_t distance = findDistance(galaxies[from], galaxies[to], 999'999);

                // update result
                result += distance;
            }
        }

        return result;
    }

    // helper functions
    void parseLine(string& line) {
        // setup cursor
        Point cursor = { rows, 0 };

        // parse galaxies
        for (cursor.back() = 0; cursor.back() < line.size(); cursor.back()++) {
            if (line.at(cursor.back()) == '#') {
                galaxies.push_back(cursor);
            }
        }

        // adjust extent
        rows += 1;
        cols = max(cols, line.size());
    }

private:
    // utility functions
    size_t findDistance(const Point& p1,
                        const Point& p2,
                        size_t extraSpace) {
        // find min / max of each dimension
        size_t minRow = min(p1.front(), p2.front());
        size_t maxRow = max(p1.front(), p2.front());

        size_t minCol = min(p1.back(), p2.back());
        size_t maxCol = max(p1.back(), p2.back());

        // account for extra distance on empty rows or cols
        size_t extraDistance = 0;
        for (size_t r = minRow; r <= maxRow; r++) {
            if (emptyRows.count(r)) {
                extraDistance += extraSpace;
            }
        }

        for (size_t c = minCol; c <= maxCol; c++) {
            if (emptyCols.count(c)) {
                extraDistance += extraSpace;
            }
        }

        // calculate cartesian distance
        size_t rowDistance = maxRow - minRow;
        size_t colDistance = maxCol - minCol;

        return rowDistance + colDistance + extraDistance;
    }
};

int main() {
    // temporary storage
    string line;

    // store the galaxies
    StarMap map;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        map.parseLine(line);
    }

    // process map
    map.findEmptiness();

    // calculate distance
    size_t firstResult = map.calculateShortDistances();

    // display result
    cout << "[first] total distance: " << firstResult << endl;

    // calculate distance
    size_t secondResult = map.calculateLongDistances();

    // display result
    cout << "[second] total distance: " << secondResult << endl;

    return 0;
}
