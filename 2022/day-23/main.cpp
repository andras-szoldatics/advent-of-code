// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

typedef array<int64_t, 2> Interval;
const Interval DEFAULT_INTERVAL = { numeric_limits<int64_t>::max(), numeric_limits<int64_t>::min() };

vector<Coordinate> DIRECTIONS = {
    { -1, 0 },
    { 1, 0 },
    { 0, -1 },
    { 0, 1 }
};

class Field {
public:
    // elves on the field
    unordered_set<Coordinate> elves;

    // step attributes
    int64_t currentDirection;

    Field() :
        currentDirection(0) {}

    // utility functions
    vector<int64_t> getDirections(const Coordinate& elf) {
        // temporary storage
        vector<int64_t> directions;

        // get neigboring positions
        for (int64_t shift = 0; shift < 4; shift++) {
            // select direction
            int64_t index = ((currentDirection + shift) % 4);
            Coordinate delta = DIRECTIONS.at(index);

            // setup for iteration
            bool foundNeighbor = false;

            // separate checks
            if (index <= 1) {
                // iterate over column deltas
                for (delta.back() = -1; delta.back() <= 1; delta.back()++) {
                    // assemble position for check
                    Coordinate position = elf;
                    position.front() += delta.front();
                    position.back() += delta.back();

                    // check for this elf
                    if (elves.count(position) != 0) {
                        // found a neighbor in this direction
                        foundNeighbor = true;
                        break;
                    }
                }
            } else {
                // iterate over row deltas
                for (delta.front() = -1; delta.front() <= 1; delta.front()++) {
                    // assemble position for check
                    Coordinate position = elf;
                    position.front() += delta.front();
                    position.back() += delta.back();

                    // check for this elf
                    if (elves.count(position) != 0) {
                        // found a neighbor in this direction
                        foundNeighbor = true;
                        break;
                    }
                }
            }

            // add direction, if neighbored
            if (!foundNeighbor) {
                directions.push_back(index);
            }
        }

        if (directions.size() == DIRECTIONS.size()) {
            // if all directions are available, don't move
            directions.clear();
        }

        return directions;
    }

    int64_t getEmptyTiles() {
        // setup for iteration
        Interval rowInterval = DEFAULT_INTERVAL;
        Interval columnInterval = DEFAULT_INTERVAL;

        // iterate over elves
        for (const Coordinate& elf : elves) {
            // shift intervals
            if (rowInterval.front() > elf.back()) {
                rowInterval.front() = elf.back();
            }

            if (rowInterval.back() < elf.back()) {
                rowInterval.back() = elf.back();
            }

            if (columnInterval.front() > elf.front()) {
                columnInterval.front() = elf.front();
            }

            if (columnInterval.back() < elf.front()) {
                columnInterval.back() = elf.front();
            }
        }

        // calculate field size
        int64_t fieldSize = ((rowInterval.back() - rowInterval.front() + 1) *
                             (columnInterval.back() - columnInterval.front() + 1));

        // calculate empty fields
        return (fieldSize - static_cast<int64_t>(elves.size()));
    }

    // iteration functions
    int64_t spreadTheElves() {
        // temporary storage
        unordered_map<Coordinate, vector<Coordinate>> movements;
        int64_t movedElves = 0;

        // iterate over elves
        for (const Coordinate& elf : elves) {
            // generate directions for elf
            vector<int64_t> directions = getDirections(elf);

            // check for no direction
            if (!directions.empty()) {
                // retrieve direction
                const Coordinate& direction = DIRECTIONS.at(directions.front());

                // generate new position
                Coordinate position = elf;
                position.front() += direction.front();
                position.back() += direction.back();

                // mark possible movements
                movements[position].push_back(elf);
            }
        }

        // iterate over movements
        for (const pair<const Coordinate, vector<Coordinate>>& movement : movements) {
            // execute only movements that are single candidates
            if (movement.second.size() == 1) {
                // remove original position
                elves.erase(movement.second.front());

                // add new position
                elves.insert(movement.first);

                // mark movement
                movedElves++;
            }
        }

        // shift current direction
        currentDirection++;
        currentDirection %= 4;

        return movedElves;
    }
};

int main() {
    // temporary storage
    string line;
    string command;

    // shape of this dimension
    Field field;

    // running tally of coordinates
    Coordinate cursor = { 0, 0 };

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check for end of the dimension
        if (line.empty()) {
            // read command line
            getline(input, command);
        }

        // move cursor to the new line
        cursor.front()++;
        cursor.back() = 1;

        // parse line's characters, one-by-one
        for (char c : line) {
            if (c == '#') {
                // handle elf value
                field.elves.insert(cursor);
            }

            // move cursor
            cursor.back()++;
        }
    }

    int64_t elvesMoved = 0;
    // iterate over ten steps
    for (size_t i = 0; i < 10; i++) {
        // execute the step
        elvesMoved = field.spreadTheElves();
    }

    // display first result
    cout << "[first] empty ground tiles : " << field.getEmptyTiles() << endl;

    // setup for iteration
    int64_t round = 10;

    // iterate until no movement
    while (elvesMoved > 0) {
        // make the elf dance
        elvesMoved = field.spreadTheElves();

        // increase round count
        round++;
    }

    // display second result
    cout << "[second] no movement after round #" << round << endl;

    return 0;
}
