#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
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

class Sea {
public:
    // attributes
    int64_t width;
    int64_t height;

    Sea() :
        width(0),
        height(0) {}
};

class Cucumber {
public:
    // attributes
    int64_t depth;
    int64_t right;

    Cucumber(int64_t _depth,
             int64_t _right) :
        depth(_depth),
        right(_right) {}

    // utility functions
    Coordinate asCoordinate() const {
        return { depth, right };
    }
};

class Herd {
public:
    // pointers
    shared_ptr<Sea> sea;

    // attributes
    vector<Cucumber> cucumbers;
    array<int64_t, 2> delta;

    Herd(const shared_ptr<Sea>& _sea,
         array<int64_t, 2> _delta) :
        sea(_sea),
        delta(_delta) {}

    // utility functions
    unordered_set<Coordinate> getCoverage() const {
        // temporary storage
        unordered_set<Coordinate> tmp;

        // iterate over cucumbers
        for (const Cucumber& cucumber : cucumbers) {
            tmp.insert(cucumber.asCoordinate());
        }

        return tmp;
    }

    size_t moveCucumbers(const unordered_set<Coordinate>& previousCoverage) {
        // temporary storage
        size_t moved = 0;

        // iterate over cucumbers
        for (Cucumber& cucumber : cucumbers) {
            // add cucumber to vector, for now
            Coordinate coordinate = cucumber.asCoordinate();

            // shift coordinate to check
            coordinate.at(0) += delta.at(0);
            coordinate.at(0) %= sea->height;
            coordinate.at(1) += delta.at(1);
            coordinate.at(1) %= sea->width;

            // check if this position is covered
            if (previousCoverage.count(coordinate) == 0) {
                // move cucumber
                cucumber.depth = coordinate.at(0);
                cucumber.right = coordinate.at(1);

                // one more cucumber moved
                moved++;
            }
        }

        return moved;
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // sea of cucumbers
    shared_ptr<Sea> sea = make_shared<Sea>();
    Herd downHerd(sea, { +1, 0 });
    Herd rightHerd(sea, { 0, +1 });

    int64_t currentDepth = 0;
    // read line of display signals
    while (getline(input, line)) {
        // increase width, if necessary
        if (sea->width < static_cast<int64_t>(line.size())) {
            sea->width = line.size();
        }

        // parse line, value by value
        for (size_t currentRight = 0; currentRight < line.size(); currentRight++) {
            // retrieve short-hands
            char c = line.at(currentRight);

            // create cucumber
            Cucumber cucumber(currentDepth, currentRight);

            // check if there is a cucumber here
            if (c == 'v') {
                downHerd.cucumbers.push_back(cucumber);
            } else if (c == '>') {
                rightHerd.cucumbers.push_back(cucumber);
            }
        }

        // increase row
        currentDepth++;

        // increase height, if necessary
        if (sea->height < currentDepth) {
            sea->height = currentDepth;
        }
    }

    // enter the first cycle
    size_t cycle = 0;
    size_t cucumbersMoved = 1;

    while (cucumbersMoved > 0) {
        // reset count
        cucumbersMoved = 0;

        // generate current coverage
        unordered_set<Coordinate> firstCoverage = downHerd.getCoverage();
        unordered_set<Coordinate> firstRightCoverage = rightHerd.getCoverage();
        firstCoverage.insert(firstRightCoverage.begin(), firstRightCoverage.end());

        // try to move cucumbers
        cucumbersMoved += rightHerd.moveCucumbers(firstCoverage);

        // generate current coverage
        unordered_set<Coordinate> secondCoverage = downHerd.getCoverage();
        unordered_set<Coordinate> secondRightCoverage = rightHerd.getCoverage();
        secondCoverage.insert(secondRightCoverage.begin(), secondRightCoverage.end());

        // try to move cucumbers
        cucumbersMoved += downHerd.moveCucumbers(secondCoverage);

        // next cycle
        cycle++;

        if (cycle % 25 == 0) {
            cout << "cucumbers moved on cycle #" << cycle << " : " << cucumbersMoved << endl;
        }
    }

    cout << "no movement on cycle #" << cycle << endl;

    return 0;
}
