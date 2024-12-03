#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

enum class Version {
    FIRST = 0,
    SECOND
};

class KeyPad {
public:
    // version
    Version version;

    // simple attributes
    unordered_map<char, Coordinate> moves;
    unordered_map<Coordinate, char> keys;
    Coordinate cursor;

    KeyPad(Version _version) :
        version(_version) {
        // moves are the same regardless
        moves['U'] = { -1, 0 };
        moves['D'] = { 1, 0 };
        moves['L'] = { 0, -1 };
        moves['R'] = { 0, 1 };

        if (version == Version::FIRST) {
            // setup keys
            keys[{ 1, 1 }] = '1';
            keys[{ 1, 2 }] = '2';
            keys[{ 1, 3 }] = '3';
            keys[{ 2, 1 }] = '4';
            keys[{ 2, 2 }] = '5';
            keys[{ 2, 3 }] = '6';
            keys[{ 3, 1 }] = '7';
            keys[{ 3, 2 }] = '8';
            keys[{ 3, 3 }] = '9';

            // setup cursor
            cursor = { 2, 2 };
        } else if (version == Version::SECOND) {
            // setup keys
            keys[{ 1, 3 }] = '1';
            keys[{ 2, 2 }] = '2';
            keys[{ 2, 3 }] = '3';
            keys[{ 2, 4 }] = '4';
            keys[{ 3, 1 }] = '5';
            keys[{ 3, 2 }] = '6';
            keys[{ 3, 3 }] = '7';
            keys[{ 3, 4 }] = '8';
            keys[{ 3, 5 }] = '9';
            keys[{ 4, 2 }] = 'A';
            keys[{ 4, 3 }] = 'B';
            keys[{ 4, 4 }] = 'C';
            keys[{ 5, 3 }] = 'D';

            // setup cursor
            cursor = { 3, 3 };
        }
    }

    // helper functions
    char keyUnderCursor() {
        return keys.at(cursor);
    }

    void moveCursor(const Coordinate& delta) {
        // calculate movement
        Coordinate future = cursor;
        future.front() += delta.front();
        future.back() += delta.back();

        // check new key
        if (keys.count(future) != 0) {
            cursor = future;
        }
    }

    // utility functions
    char decodeLine(const string& line) {
        // iterate over line
        for (char c : line) {
            // check for compatible move
            if (moves.count(c) != 0) {
                moveCursor(moves.at(c));
            }
        }

        // return final key
        return keyUnderCursor();
    }
};

int main() {
    // temporary storage
    string line;

    // setup keypad
    KeyPad firstPad(Version::FIRST);
    KeyPad secondPad(Version::SECOND);

    // temporary storage
    string firstResult;
    string secondResult;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assemble first result
        char firstKey = firstPad.decodeLine(line);
        firstResult.push_back(firstKey);

        // assemble second result
        char secondKey = secondPad.decodeLine(line);
        secondResult.push_back(secondKey);
    }

    // display result
    cout << "[first] bathroom code is : " << firstResult << endl;
    cout << "[second] bathroom code is : " << secondResult << endl;

    return 0;
}
