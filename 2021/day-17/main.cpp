#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

typedef array<int64_t, 2> Coordinate;

class Area {
public:
    // attributes
    Coordinate topLeft;
    Coordinate bottomRight;

    Area(const string& line) {
        // temporary storage
        string tmp;

        // assign to buffer
        stringstream buffer(line);

        // read X interval
        getline(buffer, tmp, '=');
        getline(buffer, tmp, '.');
        topLeft.at(0) = stoi(tmp);

        getline(buffer, tmp, '.');
        getline(buffer, tmp, ',');
        bottomRight.at(0) = stoi(tmp);

        // read Y interval
        getline(buffer, tmp, '=');
        getline(buffer, tmp, '.');
        bottomRight.at(1) = stoi(tmp);

        getline(buffer, tmp, '.');
        getline(buffer, tmp);
        topLeft.at(1) = stoi(tmp);

        // check relative values, just in case
        if (topLeft.at(0) > bottomRight.at(0)) {
            swap(topLeft.at(0), bottomRight.at(0));
        }

        if (topLeft.at(1) < bottomRight.at(1)) {
            swap(topLeft.at(1), bottomRight.at(1));
        }
    }

    // utility functions
    bool contains(const Coordinate& position) const {
        if ((position.at(0) < topLeft.at(0)) ||
            (position.at(0) > bottomRight.at(0))) {
            return false;
        }

        if ((position.at(1) > topLeft.at(1)) ||
            (position.at(1) < bottomRight.at(1))) {
            return false;
        }

        return true;
    }
};

class Launch {
public:
    // attributes
    Coordinate position;
    Coordinate delta;

    Launch(const Coordinate& _delta) :
        position({ 0, 0 }),
        delta(_delta) {}

    // utility functions
    bool canHitArea(const Area& area) {
        // track probe along the path
        while ((position.at(0) <= area.bottomRight.at(0)) &&
               (position.at(1) >= area.bottomRight.at(1))) {
            // check for hit
            if (area.contains(position)) {
                return true;
            }

            // move to next position
            position.at(0) += delta.at(0);
            position.at(1) += delta.at(1);

            // X moves towards zero
            if (delta.at(0) > 0) {
                delta.at(0) -= 1;
            } else if (delta.at(0) < 0) {
                delta.at(0) += 1;
            }

            // Y follows gravity
            delta.at(1) -= 1;
        }

        return false;
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // area storage
    Area area(line);

    // calculate highest launch, relative to the launch horizon
    int64_t depth = abs(area.bottomRight.at(1));
    int64_t highestLaunch = (depth - 1);
    highestLaunch *= depth;
    highestLaunch /= 2;

    // display result
    cout << "[first] highest Y launch: " << highestLaunch << endl;

    // setup for iteration
    Coordinate delta;
    size_t hits = 0;

    // iterate over reasonable values of delta
    for (delta.at(0) = 1; delta.at(0) <= area.bottomRight.at(0); delta.at(0)++) {
        for (delta.at(1) = -depth; delta.at(1) <= depth; delta.at(1)++) {
            // create launch
            Launch launch(delta);

            // check for hit
            if (launch.canHitArea(area)) {
                hits += 1;
            }
        }
    }

    // display result
    cout << "[second] possible hits on area: " << hits << endl;

    return 0;
}
