#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

typedef array<int64_t, 2> Position;

// extend hash functions

namespace std {
template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) + hash<int64_t>() (p.at(1));
    }
};

}

typedef unordered_map<Position, char> Lookup;

char getChar(const Lookup& lookup,
             const Position& position) {
    if (lookup.count(position) == 0) {
        return '\0';
    }

    return lookup.at(position);
}

bool checkForWord(const Lookup& lookup,
                  const Position& start,
                  const Position& direction,
                  const string& word) {
    // setup for iteration
    Position cursor = start;

    for (char c : word) {
        if (getChar(lookup, cursor) != c) {
            return false;
        }

        // shift cursor
        cursor.at(0) += direction.at(0);
        cursor.at(1) += direction.at(1);
    }

    return true;
}

int64_t countWords(const Lookup& lookup,
                   const Position& start,
                   const string& word) {
    int64_t total = 0;

    // iterate over directions
    for (int64_t row = -1; row <= 1; row++) {
        for (int64_t col = -1; col <= 1; col++) {
            // skip the center
            if ((row == 0) && (col == 0)) {
                continue;
            }

            // check for word
            if (checkForWord(lookup, start, { row, col }, word)) {
                total += 1;
            }
        }
    }

    return total;
}

int64_t countX(const Lookup& lookup,
               const Position& start) {
    if (getChar(lookup, start) != 'A') {
        return 0;
    }

    int64_t total = 0;
    char upLeft = getChar(lookup, { start.at(0) - 1, start.at(1) - 1 });
    char upRight = getChar(lookup, { start.at(0) - 1, start.at(1) + 1 });

    char downLeft = getChar(lookup, { start.at(0) + 1, start.at(1) - 1 });
    char downRight = getChar(lookup, { start.at(0) + 1, start.at(1) + 1 });

    // check for different chars crosswise
    if (((upLeft == 'M') != (downRight == 'M')) &&
        ((downLeft == 'M') != (upRight == 'M')) &&
        ((upLeft == 'S') != (downRight == 'S')) &&
        ((downLeft == 'S') != (upRight == 'S'))) {
        total += 1;
    }

    return total;
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    Lookup lookup;

    Position cursor = { 0, 0 };
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reset column
        cursor.at(1) = 0;

        for (char c : line) {
            lookup[cursor] = c;

            // shift column
            cursor.at(1) += 1;
        }

        // shift row
        cursor.at(0) += 1;
    }

    // setup for iteration
    int64_t firstTotal = 0;
    int64_t secondTotal = 0;

    // iterate over lookup
    for (const pair<Position, char>& entry : lookup) {
        firstTotal += countWords(lookup, entry.first, string("XMAS"));
        secondTotal += countX(lookup, entry.first);
    }

    cout << "[first] total number of matches: " << firstTotal << endl;
    cout << "[second] total number of matches: " << secondTotal << endl;

    return 0;
}
