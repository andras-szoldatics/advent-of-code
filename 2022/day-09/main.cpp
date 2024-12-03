#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

class Knot {
public:
    int64_t row;
    int64_t column;

    Knot(int64_t _row = 0,
         int64_t _column = 0) :
        row(_row),
        column(_column) {}

    // operator functions
    bool operator==(const Knot& rhs) const {
        return ((row == rhs.row) && (column == rhs.column));
    }

    // utility functions
    bool isTouching(const Knot& other) const {
        if ((abs(row - other.row) <= 1) &&
            (abs(column - other.column) <= 1)) {
            return true;
        }

        return false;
    }

    // movement functions
    void moveUp() {
        row--;
    }

    void moveDown() {
        row++;
    }

    void moveLeft() {
        column--;
    }
    void moveRight() {
        column++;
    }

};

// define hash function

namespace std {

template<>
struct hash<Knot> {
    size_t operator()(const Knot& object) const {
        size_t bitfield = hash<int64_t>() (object.row);
        bitfield += hash<int64_t>() (object.column);

        return bitfield;
    }
};

} // end of namespace std

int main() {
    // temporary storage
    string line;

    const size_t NUMBER_OF_KNOTS = 10;
    // positions for the ends of the rope
    vector<Knot> knots;
    knots.resize(NUMBER_OF_KNOTS, Knot(0, 0));

    // track tail's movements
    unordered_set<Knot> secondPositions;
    secondPositions.insert(knots.at(1));

    unordered_set<Knot> tailPositions;
    tailPositions.insert(knots.back());

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // read instruction from line
        char instruction = line.front();
        string iterationString = line.substr(2);
        size_t iteration = stoull(iterationString);

        // execute instruction
        for (size_t i = 0; i < iteration; i++) {
            // retrieve short-hands
            Knot& head = knots.front();

            // check type
            switch (instruction) {
            case 'U':
                head.moveUp();
                break;
            case 'D':
                head.moveDown();
                break;
            case 'L':
                head.moveLeft();
                break;
            case 'R':
                head.moveRight();
                break;
            default:
                // NO-OP
                (void) nullptr;
            }

            // iterate over the other knots
            for (size_t i = 1; i < NUMBER_OF_KNOTS; i++) {
                // retrieve short-hands
                const Knot& front = knots.at(i - 1);
                Knot& back = knots.at(i);

                // check if back needs to move as well
                if (!back.isTouching(front)) {
                    // it needs to move, calculate delta
                    int64_t rowDelta = (front.row - back.row);
                    int64_t columnDelta = (front.column - back.column);

                    // execute back movement
                    if (rowDelta < 0) {
                        back.moveUp();
                    } else if (rowDelta > 0) {
                        back.moveDown();
                    }

                    if (columnDelta < 0) {
                        back.moveLeft();
                    } else if (columnDelta > 0) {
                        back.moveRight();
                    }

                    if (i == 1) {
                        // mark new position of second
                        secondPositions.insert(back);
                    }

                    if (i + 1 == NUMBER_OF_KNOTS) {
                        // mark new position of tail
                        tailPositions.insert(back);
                    }
                }
            }
        }
    }

    cout << "# of positions for 2nd knot : " << secondPositions.size() << endl;
    cout << "# of positions for tail knot : " << tailPositions.size() << endl;

    return 0;
}
