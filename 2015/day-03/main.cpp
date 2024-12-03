#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

template<>
struct std::hash<Position> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read first & only relevant line
    getline(input, line);

    // define directions as deltas
    vector<Position> deltas = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

    // current position
    Position firstSanta = { 0, 0 };
    Position secondSanta = { 0, 0 };
    Position secondRoboSanta = { 0, 0 };

    // set of already visited positions
    unordered_set<Position> firstVisited;
    firstVisited.insert(firstSanta);

    unordered_set<Position> secondVisited;
    firstVisited.insert(firstSanta);

    // process line in a chunk
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        const char c = line.at(i);

        // temporary storage
        Position delta = { 0, 0 };

        // select appropriate delta
        if (c == '^') {
            delta = deltas.at(0);
        } else if (c == 'v') {
            delta = deltas.at(1);
        } else if (c == '>') {
            delta = deltas.at(2);
        } else if (c == '<') {
            delta = deltas.at(3);
        }

        // move first santa
        firstSanta.at(0) += delta.at(0);
        firstSanta.at(1) += delta.at(1);
        firstVisited.insert(firstSanta);

        if (i % 2 == 0) {
            // move second santa
            secondSanta.at(0) += delta.at(0);
            secondSanta.at(1) += delta.at(1);
            secondVisited.insert(secondSanta);
        } else {
            // move second robo-santa
            secondRoboSanta.at(0) += delta.at(0);
            secondRoboSanta.at(1) += delta.at(1);
            secondVisited.insert(secondRoboSanta);
        }
    }

    // display result for first part
    cout << "houses visited by Santa : " << firstVisited.size() << endl;
    cout << "houses visited by Santa & RoboSanta : " << secondVisited.size() << endl;

    return 0;
}
