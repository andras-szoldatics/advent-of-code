#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_set>
#include <vector>

using namespace std;

vector<string> splitLine(const string& line,
                         char delimiter,
                         const string& filter = string()) {
    // temporary storage
    vector<string> splits;

    // assign line to buffer
    stringstream buffer(line);

    string split;
    // read splits from buffer
    while (getline(buffer, split, delimiter)) {
        // check for filtered value
        if ((split.empty()) ||
            (split == filter)) {
            continue;
        }

        // add split to splits
        splits.push_back(split);
    }

    return splits;
}

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

void printState(int64_t fromColumn,
                int64_t toColumn,
                int64_t fromRow,
                int64_t toRow,
                const unordered_set<Coordinate>& cavernWalls,
                const unordered_set<Coordinate>& cavernSand) {
    // iterate over rows
    for (int64_t row = fromRow; row <= toRow; row++) {
        for (int64_t column = fromColumn; column <= toColumn; column++) {
            // assemble test coordinate
            Coordinate cursor = { column, row };
            if (cavernWalls.count(cursor) != 0) {
                cout << "#";
            } else if (cavernSand.count(cursor) != 0) {
                cout << "O";
            } else {
                cout << ".";
            }
        }

        cout << endl;
    }

    cout << endl;
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // define sand source
    Coordinate sandOrigin = { 500, 0 };

    // current state of the cavern
    int64_t lowestWall = 0;
    unordered_set<Coordinate> cavernWalls;
    unordered_set<Coordinate> cavernSand;

    // read all lines of the file
    while (getline(input, line)) {
        // temporary storage
        vector<string> lineParts = splitLine(line, ' ', string("->"));
        vector<Coordinate> lineCoordinates;

        // iterate over parts
        for (const string& linePart : lineParts) {
            // temporary storage
            Coordinate coordinate;

            // split line part into coordinates
            vector<string> parts = splitLine(linePart, ',');

            // parse as coordinate
            coordinate.at(0) = stoll(parts.at(0));
            coordinate.at(1) = stoll(parts.at(1));

            // add coordinate to line
            lineCoordinates.push_back(coordinate);

            // check for lowest wall
            if (lowestWall < coordinate.at(1)) {
                lowestWall = coordinate.at(1);
            }
        }

        // iterate over coordinate pairs
        for (size_t i = 1; i < lineCoordinates.size(); i++) {
            // from / to coordinates
            Coordinate from = lineCoordinates.at(i - 1);
            Coordinate to = lineCoordinates.at(i);

            // setup a linewise iteration
            cavernWalls.insert(from);
            Coordinate current = from;

            // iterate over the line
            while (current != to) {
                // shift current towards end
                if (current.at(0) > to.at(0)) {
                    current.at(0)--;
                } else if (current.at(0) < to.at(0)) {
                    current.at(0)++;
                }

                if (current.at(1) > to.at(1)) {
                    current.at(1)--;
                } else if (current.at(1) < to.at(1)) {
                    current.at(1)++;
                }

                // draw the wall on the cavern
                cavernWalls.insert(current);
            }
        }
    }

    // add floor to cavern
    for (int64_t i = 0; i <= 1000; i++) {
        // temporary storage
        Coordinate floor = { i, lowestWall + 2 };

        // add floor to cavern
        cavernWalls.insert(floor);
    }

    bool sandFellOver = false;
    size_t droppedSand = 0;
    int64_t lastSandDepth = 0;

    // cache sand falling path
    stack<Coordinate> fallingCache;
    fallingCache.push(sandOrigin);

    // iterate while sand is stopped
    while (lastSandDepth < (lowestWall + 2)) {
        // drop some sand
        droppedSand++;

        // check for alternative termination
        if (fallingCache.empty()) {
            break;
        }

        // initialize sand position
        Coordinate sand = fallingCache.top();

        bool falling = true;
        while (falling) {
            // generate next coordinate
            Coordinate down = sand;
            down.at(1)++;

            // check down position
            if ((cavernWalls.count(down) == 0) &&
                (cavernSand.count(down) == 0)) {
                sand = down;
            } else {
                // generate down and left
                down.at(0) -= 1;

                // check down and left
                if ((cavernWalls.count(down) == 0) &&
                    (cavernSand.count(down) == 0)) {
                    sand = down;
                } else {
                    // generate down and right
                    down.at(0) += 2;

                    // check down and right
                    if ((cavernWalls.count(down) == 0) &&
                        (cavernSand.count(down) == 0)) {
                        sand = down;
                    } else {
                        falling = false;
                    }
                }
            }

            if (fallingCache.top() != sand) {
                // build up falling cache
                fallingCache.push(sand);
            }
        }

        // add sand to cavern
        cavernSand.insert(sand);

        // get last sand depth
        lastSandDepth = sand.at(1);

        while (cavernSand.count(fallingCache.top()) != 0) {
            // clean up falling cache
            fallingCache.pop();

            // check for empty cache
            if (fallingCache.empty()) {
                break;
            }
        }

        // check for first part
        if (lastSandDepth >= lowestWall) {
            if (!sandFellOver) {
                cout << "sand fell over, after sand #" << (droppedSand - 1) << endl;
                sandFellOver = true;
            }
        }
    }

    cout << "cavern filled up, after sand #" << (droppedSand - 1) << endl;

    return 0;
}
