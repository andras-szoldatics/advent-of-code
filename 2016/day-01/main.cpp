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
    vector<Position> deltas = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
    int64_t deltaIndex = 0;

    // current position
    Position current = { 0, 0 };

    // set of already visited positions
    unordered_set<Position> firstVisited;
    firstVisited.insert(current);

    // list of second visits
    vector<Position> multiVisited;

    // assign line to a buffer
    stringstream buffer(line);

    string turn;
    // process line in a chunk
    while (getline(buffer, turn, ' ')) {
        // process chunk's start
        if (turn.front() == 'R') {
            deltaIndex++;

            // wrap-around indexing
            if (deltaIndex > 3) {
                deltaIndex = 0;
            }
        } else if (turn.front() == 'L') {
            deltaIndex--;

            // wrap-around indexing
            if (deltaIndex < 0) {
                deltaIndex = 3;
            }
        }

        // retrieve number
        turn.erase(0, 1);
        if (turn.back() == ',') {
            turn.pop_back();
        }
        size_t blocks = stoull(turn);

        // move along the grid
        for (size_t i = 0; i < blocks; i++) {
            // shift position by current direction
            current.at(0) += deltas.at(deltaIndex).at(0);
            current.at(1) += deltas.at(deltaIndex).at(1);

            // classify current position
            if (!firstVisited.count(current)) {
                // this is visit #1
                firstVisited.insert(current);
            } else {
                // this is visit #2+
                multiVisited.push_back(current);
            }
        }
    }

    // display result for first part
    cout << "final position : " << current.at(0) << " " << current.at(1) << endl;
    cout << "manhattan distance of this : " << (abs(current.at(0)) + abs(current.at(1))) << endl;

    if (!multiVisited.empty()) {
        // retrieve short-hands
        Position firstDoubleVisit = multiVisited.front();

        // display result for second part
        cout << endl;
        cout << "1st multi-visited position : " << firstDoubleVisit.at(0) << " " << firstDoubleVisit.at(1) << endl;
        cout << "manhattan distance of this : " << (abs(firstDoubleVisit.at(0)) + abs(firstDoubleVisit.at(1))) << endl;
    }

    return 0;
}
