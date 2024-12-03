#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

class Cubeoid {
public:
    // state
    bool isOn;

    // from coordinates
    int64_t x1;
    int64_t y1;
    int64_t z1;

    // to coordinates
    int64_t x2;
    int64_t y2;
    int64_t z2;

    Cubeoid(int64_t _x1,
            int64_t _y1,
            int64_t _z1,
            int64_t _x2,
            int64_t _y2,
            int64_t _z2,
            bool _isOn) :
        isOn(_isOn),
        x1(_x1),
        y1(_y1),
        z1(_z1),
        x2(_x2),
        y2(_y2),
        z2(_z2) {}

    Cubeoid(const string& line) :
        isOn(false),
        x1(0),
        y1(0),
        z1(0),
        x2(0),
        y2(0),
        z2(0) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // read state
        getline(buffer, tmp, ' ');
        isOn = (tmp == "on");

        // read X interval
        getline(buffer, tmp, '=');
        getline(buffer, tmp, '.');
        x1 = stoll(tmp);

        getline(buffer, tmp, '.');
        getline(buffer, tmp, ',');
        x2 = stoll(tmp);

        // read Y interval
        getline(buffer, tmp, '=');
        getline(buffer, tmp, '.');
        y1 = stoll(tmp);

        getline(buffer, tmp, '.');
        getline(buffer, tmp, ',');
        y2 = stoll(tmp);

        // read Z interval
        getline(buffer, tmp, '=');
        getline(buffer, tmp, '.');
        z1 = stoll(tmp);

        getline(buffer, tmp, '.');
        getline(buffer, tmp);
        z2 = stoll(tmp);
    }

    // utility functions
    int64_t getVolume() const {
        return (x2 - x1 + 1) *
               (y2 - y1 + 1) *
               (z2 - z1 + 1) *
               (isOn ? 1 : -1);
    }

    optional<Cubeoid> getDeltaCubeoid(const Cubeoid& other) const {
        // generate an intersection coordinate pairs
        int64_t dx1 = max(x1, other.x1);
        int64_t dy1 = max(y1, other.y1);
        int64_t dz1 = max(z1, other.z1);
        int64_t dx2 = min(x2, other.x2);
        int64_t dy2 = min(y2, other.y2);
        int64_t dz2 = min(z2, other.z2);

        // check if the intersection is valid
        if ((dx1 > dx2) ||
            (dy1 > dy2) ||
            (dz1 > dz2)) {
            return nullopt;
        }

        // determine the on state of delta cube
        bool dIsOn = other.isOn;
        if (dIsOn && other.isOn) {
            // type overlap, correct for total volume
            dIsOn = false;
        } else if (!dIsOn && !other.isOn) {
            // type overlap, correct for total volume
            dIsOn = true;
        }

        // return delta cubeoid
        return Cubeoid(dx1, dy1, dz1, dx2, dy2, dz2, dIsOn);
    }
};

class Reactor {
public:
    // keep a list of deltas
    vector<Cubeoid> listOfDeltas;

    // helper functions
    void addCube(const Cubeoid& cube) {
        // temporary storage
        vector<Cubeoid> deltas;

        // iterate over all previous cubes
        for (const Cubeoid& other : listOfDeltas) {
            // get the delta cube
            optional<Cubeoid> delta = cube.getDeltaCubeoid(other);

            // check if the delta is valid
            if (delta.has_value()) {
                // add the delta
                deltas.emplace_back(delta.value());
            }
        }

        // add the original cube, if it is on
        if (cube.isOn) {
            deltas.push_back(cube);
        }

        // update the deltas
        for (const Cubeoid& delta : deltas) {
            listOfDeltas.push_back(delta);
        }
    }

    // utility functions
    int64_t getVolume() const {
        // temprorary storage
        int64_t volume = 0;

        // iterate over all cubes
        for (const Cubeoid& cube : listOfDeltas) {
            // add the volume
            volume += cube.getVolume();
        }

        return volume;
    }
};

int main() {
    // temporary storage
    string line;

    // list of cubes
    vector<Cubeoid> cubes;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        cubes.emplace_back(line);
    }

    // setup for iteration
    Reactor firstReactor;

    // iterate over all cubes
    for (const Cubeoid& cube : cubes) {
        // filter on first reactor
        if ((cube.x1 < 50) &&
            (cube.y1 < 50) &&
            (cube.z1 < 50) &&
            (cube.x2 > -50) &&
            (cube.y2 > -50) &&
            (cube.z2 > -50)) {
            // add the cubes
            firstReactor.addCube(cube);
        }
    }

    // display result
    cout << "[first] volume of active cubes: " << firstReactor.getVolume() << endl;

    // setup for iteration
    Reactor secondReactor;

    // iterate over all cubes
    for (const Cubeoid& cube : cubes) {
        // just add the cubes
        secondReactor.addCube(cube);
    }

    // display result
    cout << "[second] volume of active cubes: " << secondReactor.getVolume() << endl;

    return 0;
}
