#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

typedef array<size_t, 2> Coordinate;

namespace std {

// extend hash for Coordinate
template<>
struct hash<Coordinate> {
    size_t operator()(const Coordinate& c) const {
        size_t bitfield = hash<size_t>() (c.front());
        bitfield += hash<size_t>() (c.back());
        return bitfield;
    }
};

} // namespace std

int64_t processSchematic(const vector<string>& schematic,
                         const unordered_map<Coordinate, char>& symbols,
                         unordered_map<Coordinate, vector<int64_t>>& connections) {
    // temporary storage
    int64_t result = 0;

    // iterate over each line
    for (size_t row = 0; row < schematic.size(); row++) {
        // retrieve short-hands
        const string& line = schematic[row];

        // iterate to find start of number
        for (size_t fromCol = 0; fromCol < line.size(); fromCol++) {
            // retrieve short-hands
            char f = line[fromCol];

            if (isdigit(f)) {
                // retrieve number
                string partial = { f };

                // iterate to find end of number
                for (size_t toCol = fromCol + 1; toCol < line.size(); toCol++) {
                    // retrieve short-hands
                    char t = line[toCol];

                    if (isdigit(t)) {
                        // append to number
                        partial += t;
                    } else {
                        break;
                    }
                }

                // convert to integer
                int64_t number = stoll(partial);

                // generate rectangle around number
                Coordinate topLeft = { row > 0 ? row - 1 : 0, fromCol > 0 ? fromCol - 1 : 0 };
                Coordinate bottomRight = { row + 1, fromCol + partial.size() };
                Coordinate cursor = { 0, 0 };
                bool foundConnection = false;

                // iterate over coordinates
                for (cursor.front() = topLeft.front(); cursor.front() <= bottomRight.front(); cursor.front()++) {
                    for (cursor.back() = topLeft.back(); cursor.back() <= bottomRight.back(); cursor.back()++) {
                        // check if coordinate is symbol
                        if (symbols.count(cursor) > 0) {
                            // set flag
                            foundConnection = true;

                            // mark a connection
                            connections[cursor].push_back(number);
                        }
                    }
                }

                // check if flag is set
                if (foundConnection) {
                    // add to total
                    result += number;
                }

                // shift from position
                fromCol += partial.size() - 1;
            }
        }
    }

    return result;
}

int64_t processGears(const unordered_map<Coordinate, char>& symbols,
                     const unordered_map<Coordinate, vector<int64_t>>& connections) {
    // temporary storage
    int64_t result = 0;

    // iterate over symbols
    for (const pair<const Coordinate, char>& symbol : symbols) {
        if (symbol.second == '*') {
            // check if this has exactly two connections
            if (connections.at(symbol.first).size() == 2) {
                // retrieve connections
                const vector<int64_t>& entry = connections.at(symbol.first);

                // calculate gear ratio
                int64_t ratio = entry.front() * entry.back();

                // add to total
                result += ratio;
            }
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // store original schematic
    vector<string> schematic;
    unordered_map<Coordinate, char> symbols;
    unordered_map<Coordinate, vector<int64_t>> connections;

    // setup for iteration
    size_t row = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // iterate over each character
        for (size_t col = 0; col < line.size(); col++) {
            // retrieve short-hands
            char& c = line[col];

            if (!isdigit(c)) {
                if (c != '.') {
                    // store coordinate of symbol
                    Coordinate coord = { row, col };
                    symbols[coord] = c;
                }

                // replace with space
                c = ' ';
            }
        }

        // store each line
        schematic.push_back(line);

        // increment row
        row += 1;
    }

    // setup for iteration
    int64_t firstTotal = processSchematic(schematic, symbols, connections);
    int64_t secondTotal = processGears(symbols, connections);

    // display progress
    cout << "[first] total of part numbers : " << firstTotal << endl;
    cout << "[second] total of gear ratios : " << secondTotal << endl;

    return 0;
}
