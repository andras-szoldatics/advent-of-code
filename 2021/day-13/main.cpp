#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a.at(0));
        std::size_t h2 = std::hash<int64_t>() (a.at(1));

        return (h1 + h2);
    }
};

class Dots {
public:
    // current coordinate space
    unordered_set<Coordinate> space;

    // note the extent
    int64_t maxX;
    int64_t maxY;

    Dots() :
        maxX(0),
        maxY(0) {}

    // helper functions
    void add(const Coordinate& dot) {
        space.insert(dot);

        // shift extent
        maxX = max(maxX, dot.at(0));
        maxY = max(maxY, dot.at(1));
    }

    void recalculate() {
        // reset extent
        maxX = 0;
        maxY = 0;

        // iterate over space
        for (const Coordinate& entry : space) {
            // shift extent
            maxX = max(maxX, entry.at(0));
            maxY = max(maxY, entry.at(1));
        }
    }

    // utility functions
    void foldUp(int64_t y) {
        // temporary storage
        vector<Coordinate> moved;

        // iterate over dots
        for (const Coordinate& entry : space) {
            if (entry.at(1) > y) {
                // add to vector
                moved.push_back(entry);
            }
        }

        // erase moved dots
        for (const Coordinate& entry : moved) {
            space.erase(entry);
        }

        // create new dots
        for (const Coordinate& entry : moved) {
            // temporary storage
            Coordinate dot = entry;
            dot.at(1) = y + y - dot.at(1);

            // add to space
            space.insert(dot);
        }

        // shift extent
        recalculate();
    }

    void foldLeft(int64_t x) {
        // temporary storage
        vector<Coordinate> moved;

        // iterate over dots
        for (const Coordinate& entry : space) {
            if (entry.at(0) > x) {
                // add to vector
                moved.push_back(entry);
            }
        }

        // erase moved dots
        for (const Coordinate& entry : moved) {
            space.erase(entry);
        }

        // create new dots
        for (const Coordinate& entry : moved) {
            // temporary storage
            Coordinate dot = entry;
            dot.at(0) = x + x - dot.at(0);

            // add to space
            space.insert(dot);
        }

        // shift extent
        recalculate();
    }

    void displaySpace() {
        // temporary storage
        Coordinate tmp;

        // iterate over extent
        for (tmp.at(1) = 0; tmp.at(1) <= maxY; tmp.at(1)++) {
            for (tmp.at(0) = 0; tmp.at(0) <= maxX; tmp.at(0)++) {
                // check if dot exists
                if (space.count(tmp) != 0) {
                    cout << '#';
                } else {
                    cout << ' ';
                }
            }

            cout << endl;
        }
    }
};

class Fold {
public:
    // simple attributes
    optional<int64_t> x;
    optional<int64_t> y;

    // utility functions
    void execute(Dots& dots) const {
        if (x.has_value()) {
            dots.foldLeft(x.value());
        }

        if (y.has_value()) {
            dots.foldUp(y.value());
        }
    }
};

typedef vector<Fold> Folds;

int main() {
    // temporary storage
    string line;

    // storage for octopuses
    Dots dots;
    Folds folds;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // skip empty lines
        if (line.empty()) {
            continue;
        }

        // assign to buffer
        stringstream buffer(line);

        // parse line
        if (line.front() == 'f') {
            // temporary storage
            Fold fold;

            // parse fold information
            string tmp, type;
            getline(buffer, tmp, ' ');
            getline(buffer, tmp, ' ');
            getline(buffer, type, '=');

            // parse fold value
            int64_t value;
            getline(buffer, tmp);
            value = stoll(tmp);

            // assign to fold
            if (type == "x") {
                fold.x = value;
            } else if (type == "y") {
                fold.y = value;
            }

            // add to folds
            folds.push_back(fold);
        } else {
            // temporary storage
            Coordinate dot;
            string tmp;

            // parse coordinate information
            getline(buffer, tmp, ',');
            dot.at(0) = stoll(tmp);

            getline(buffer, tmp);
            dot.at(1) = stoll(tmp);

            // add to dots
            dots.add(dot);
        }
    }

    // setup for iteration
    optional<size_t> firstSize;

    // execute folds
    for (const Fold& fold : folds) {
        // execute fold
        fold.execute(dots);

        // mark first solution
        if (!firstSize) {
            firstSize = dots.space.size();
        }
    }

    // display solution
    cout << "[first] # of dots: " << firstSize.value() << endl;
    cout << "[second]" << endl;
    dots.displaySpace();

    return 0;
}
