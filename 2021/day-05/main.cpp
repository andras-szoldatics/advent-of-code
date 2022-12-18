// C++ standard includes
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
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

class VentLine {
public:
    // attributes
    Coordinate first;
    Coordinate last;

    VentLine() {
        // assign empty values
        first = { 0, 0 };
        last = { 0, 0 };
    }

    void parseFromString(const string& line) {
        // split line in two
        size_t firstSpace = line.find(' ');
        string firstPart = line.substr(0, firstSpace);

        string tmp = line.substr(firstSpace + 1);
        size_t secondSpace = tmp.find(' ');
        string secondPart = tmp.substr(secondSpace + 1);

        // assign buffer for first part
        stringstream firstBuffer(firstPart);

        // retrive first row
        getline(firstBuffer, tmp, ',');
        first.at(0) = stoll(tmp);

        // retrive first column
        getline(firstBuffer, tmp, ',');
        first.at(1) = stoll(tmp);

        // assign buffer for second part
        stringstream secondBuffer(secondPart);

        // retrive first row
        getline(secondBuffer, tmp, ',');
        last.at(0) = stoll(tmp);

        // retrive first column
        getline(secondBuffer, tmp, ',');
        last.at(1) = stoll(tmp);
    }

    vector<Coordinate> getCoordinates() const {
        // temporary storage
        vector<Coordinate> coordinates = { first };

        // check for simple case - 1 tile
        if (first == last) {
            return coordinates;
        }

        // iterate until reaching end
        while (coordinates.back() != last) {
            // temporary storage
            Coordinate currentPosition = coordinates.back();

            // move towards end
            if (currentPosition.at(0) != last.at(0)) {
                // need to adjust rows
                if (currentPosition.at(0) < last.at(0)) {
                    currentPosition.at(0)++;
                } else {
                    currentPosition.at(0)--;
                }
            }

            if (currentPosition.at(1) != last.at(1)) {
                // need to adjust columns
                if (currentPosition.at(1) < last.at(1)) {
                    currentPosition.at(1)++;
                } else {
                    currentPosition.at(1)--;
                }
            }

            // add modified position to vector
            coordinates.push_back(currentPosition);
        }

        return coordinates;
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // vector of lines
    vector<VentLine> firstVentLines;
    vector<VentLine> secondVentLines;

    // set of dangerous tiles
    unordered_set<Coordinate> firstDangerousTiles;
    unordered_set<Coordinate> firstVeryDangerousTiles;

    unordered_set<Coordinate> secondDangerousTiles;
    unordered_set<Coordinate> secondVeryDangerousTiles;

    // iterate over bingo tables
    while (getline(input, line)) {
        // create new line
        VentLine tmp;

        // parse class
        tmp.parseFromString(line);

        // filter lines according to type
        if ((tmp.first.at(0) == tmp.last.at(0)) ||
            (tmp.first.at(1) == tmp.last.at(1))) {
            // add line to vector
            firstVentLines.push_back(tmp);
        }

        // add line to vector
        secondVentLines.push_back(tmp);
    }

    // iterate over first lines
    for (const VentLine& ventLine : firstVentLines) {
        // generate vector of tiles
        vector<Coordinate> coordinates = ventLine.getCoordinates();

        // check coordinates one-by-one
        for (const Coordinate& coordinate : coordinates) {
            // evaluate tile of the coordinate
            if (firstDangerousTiles.count(coordinate)) {
                firstVeryDangerousTiles.insert(coordinate);
            } else {
                firstDangerousTiles.insert(coordinate);
            }
        }
    }

    cout << "[first] very dangerous tiles : " << firstVeryDangerousTiles.size() << endl;

    // iterate over second lines
    for (const VentLine& ventLine : secondVentLines) {
        // generate vector of tiles
        vector<Coordinate> coordinates = ventLine.getCoordinates();

        // check coordinates one-by-one
        for (const Coordinate& coordinate : coordinates) {
            // evaluate tile of the coordinate
            if (secondDangerousTiles.count(coordinate)) {
                secondVeryDangerousTiles.insert(coordinate);
            } else {
                secondDangerousTiles.insert(coordinate);
            }
        }
    }

    cout << "[second] very dangerous tiles : " << secondVeryDangerousTiles.size() << endl;

    return 0;
}
