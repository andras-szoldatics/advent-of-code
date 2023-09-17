#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

// AOC-2021 @ day 20

typedef array<int64_t, 2> Coordinate;
typedef array<bool, 512> Algorithm;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a.at(0));
        std::size_t h2 = std::hash<int64_t>() (a.at(1));

        return (h1 + h2);
    }
};

class Image {
public:
    // stored pixels
    vector<vector<bool>> pixels;

    // current state
    bool currentBackground;
    size_t shift;

    Image() :
        currentBackground(false),
        shift(0) {}

    // helper functions
    void allocate(size_t size,
                  bool value) {
        // allocate pixels
        pixels.resize(size, vector<bool>(size, value));
    }

    void addPixel(const Coordinate& coordinate) {
        // add pixel
        pixels.at(coordinate.at(1)).at(coordinate.at(0)) = true;
    }

    bool getValue(const Coordinate& coordinate) {
        // check for out of bounds
        if ((coordinate.at(0) < 0) ||
            (coordinate.at(1) < 0) ||
            (coordinate.at(0) >= static_cast<int64_t>(pixels.size())) ||
            (coordinate.at(1) >= static_cast<int64_t>(pixels.size()))) {
            return currentBackground;
        }

        return pixels.at(coordinate.at(1)).at(coordinate.at(0));
    }

    size_t getEntry(const Coordinate& coordinate) {
        // temporary storage
        Coordinate cursor;
        size_t tmp = 0;

        // check surround entries
        for (cursor.at(1) = coordinate.at(1) - 1; cursor.at(1) <= coordinate.at(1) + 1; cursor.at(1)++) {
            for (cursor.at(0) = coordinate.at(0) - 1; cursor.at(0) <= coordinate.at(0) + 1; cursor.at(0)++) {
                // shift left
                tmp <<= 1;

                // check for active pixel
                bool activePixel = getValue(cursor);
                if (activePixel) {
                    tmp |= 1;
                }
            }
        }

        return tmp;
    }

    size_t getActiveSize() {
        // temporary storage
        size_t tmp = 0;

        // iterate over pixels
        for (const vector<bool>& row : pixels) {
            for (bool pixel : row) {
                tmp += (pixel ? 1 : 0);
            }
        }

        return tmp;
    }

    // utility functions
    Image enhance(const Algorithm& algorithm) {
        // temporary storage
        Image enhanced;
        size_t newSize = pixels.size() + 2;
        enhanced.allocate(newSize, false);

        // switch background
        enhanced.currentBackground = (currentBackground ? algorithm.back() : algorithm.front());

        // setup for iteration
        Coordinate coordinate;

        // iterate over pixels
        for (coordinate.at(1) = 0; coordinate.at(1) < static_cast<int64_t>(newSize); coordinate.at(1)++) {
            for (coordinate.at(0) = 0; coordinate.at(0) < static_cast<int64_t>(newSize); coordinate.at(0)++) {
                // shift lookup coordinate
                Coordinate cursor = { coordinate.at(0) - 1, coordinate.at(1) - 1 };

                // check for algorithm
                size_t entry = getEntry(cursor);

                // store pixel
                if (algorithm.at(entry)) {
                    enhanced.addPixel(coordinate);
                }
            }
        }


        return enhanced;
    }
};

int main() {
    // temporary storage
    string line;

    // CSI data set
    Algorithm algorithm;
    Image image;

    // setup for iteration
    bool onAlgorithm = true;
    int64_t row = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check for empty line
        if (line.empty()) {
            onAlgorithm = false;
            continue;
        }

        if (onAlgorithm) {
            // convert to algorithm
            for (size_t i = 0; i < line.size(); i++) {
                algorithm.at(i) = (line.at(i) == '#');
            }

            // set background
            image.currentBackground = false;
        } else {
            // check for first row
            if (image.pixels.empty()) {
                // allocate pixels
                image.allocate(line.size(), false);
            }

            // iterate over line
            for (size_t i = 0; i < line.size(); i++) {
                // check for active pixel
                bool value = (line.at(i) == '#');

                if (value) {
                    // store pixel
                    Coordinate pixel = { static_cast<int64_t>(i), row };
                    image.addPixel(pixel);
                }
            }

            // update row
            row += 1;
        }
    }

    // iterate over pixels twice
    for (size_t i = 0; i < 50; i++) {
        if (i == 2) {
            // print result
            cout << "[first] # of pixels: " << image.getActiveSize() << endl;
        }

        // enhance image
        image = image.enhance(algorithm);
    }

    // print result
    cout << "[second] # of pixels: " << image.getActiveSize() << endl;

    return 0;
}
