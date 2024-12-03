#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// const variables
size_t SCREEN_WIDTH = 50;
size_t SCREEN_HEIGHT = 6;

class Screen {
public:
    // state storage
    vector<bool> pixels;

    Screen(size_t width = SCREEN_WIDTH,
           size_t height = SCREEN_HEIGHT) :
        screenWidth(width),
        screenHeight(height) {
            // set default state
            pixels = vector<bool>(screenWidth * screenHeight, false);
        }

    // helper functions
    size_t getIndex(size_t row,
                    size_t column) const {
        // calculate index
        size_t index = (row * screenWidth) + column;

        return index;
    }

    // drawing functions
    void drawRectangle(size_t width,
                       size_t height) {
        // draw from the top left corner
        for (size_t row = 0; row < height; row++) {
            for (size_t column = 0; column < width; column++) {
                // calculate index
                size_t index = getIndex(row, column);

                // draw pixel
                pixels.at(index) = true;
            }
        }
    }

    // moving functions
    void shiftRow(size_t row,
                  size_t amount) {
        // retrieve pixels at the end of row
        vector<bool> endOfRow(pixels.begin() + (row * screenWidth) + screenWidth - amount,
                              pixels.begin() + (row * screenWidth) + screenWidth);

        // shift every pixel to the right
        for (size_t column = screenWidth - 1; column >= amount; column--) {
            // calculate index
            size_t index = getIndex(row, column);

            // shift pixel
            pixels.at(index) = pixels.at(index - amount);
        }

        // account for wrap-around
        for (size_t column = 0; column < amount; column++) {
            // calculate index
            size_t index = getIndex(row, column);

            // shift pixel
            pixels.at(index) = endOfRow.at(column);
        }
    }

    void shiftColumn(size_t column,
                     size_t amount) {
        //  temporary storage
        vector<bool> endOfColumn;

        // retrieve pixels at the end of column
        for (size_t row = screenHeight - amount; row < screenHeight; row++) {
            // calculate index
            size_t index = getIndex(row, column);

            // retrieve pixel
            endOfColumn.push_back(pixels.at(index));
        }

        // shift every pixel down
        for (size_t row = screenHeight - 1; row >= amount; row--) {
            // calculate index
            size_t index = getIndex(row, column);

            // shift pixel
            pixels.at(index) = pixels.at(index - (amount * screenWidth));
        }

        // account for wrap-around
        for (size_t row = 0; row < amount; row++) {
            // calculate index
            size_t index = getIndex(row, column);

            // shift pixel
            pixels.at(index) = endOfColumn.at(row);
        }
    }

    // diplay functions
    size_t litPixels() const {
        // temporary storage
        size_t count = 0;

        // iterate over all the pixels
        for (bool pixel : pixels) {
            // count lit pixels
            if (pixel) {
                count++;
            }
        }

        return count;
    }

    void display() const {
        // iterate over all the pixels
        for (size_t y = 0; y < screenHeight; ++y) {
            for (size_t x = 0; x < screenWidth; ++x) {
                // calculate index
                size_t index = (y * screenWidth) + x;

                // display pixel
                cout << (pixels.at(index) ? '#' : '.');
            }

            // new line
            cout << endl;
        }
    }

private:
    // screen dimensions
    size_t screenWidth;
    size_t screenHeight;
};

void unitTest() {
    // initialize smaller screen
    Screen screen(7, 3);

    // test draw rectangle
    screen.drawRectangle(3, 2);
    if (screen.litPixels() != 6) {
        cout << "[unit test] draw rectangle failed" << endl;
        exit(1);
    }

    // test shift column
    screen.shiftColumn(1, 1);
    if (screen.litPixels() != 6) {
        cout << "[unit test] shift column failed" << endl;
        exit(1);
    }

    // test shift row
    screen.shiftRow(0, 4);
    if (screen.litPixels() != 6) {
        cout << "[unit test] shift row failed" << endl;
        exit(1);
    }

    // test shift column
    screen.shiftColumn(1, 1);
    if (screen.litPixels() != 6) {
        cout << "[unit test] shift column failed" << endl;
        exit(1);
    }

    // test display
    screen.display();
    cout << endl;
}

int main() {
    // temporary storage
    string line;

    unitTest();

    // setup screen
    Screen screen;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line as screen instruction
        if (line.substr(0, 4) == "rect") {
            // parse width and height
            size_t width = stoull(line.substr(5, line.find('x')));
            size_t height = stoull(line.substr(line.find('x') + 1));

            // draw rectangle
            screen.drawRectangle(width, height);
        } else if (line.substr(0, 10) == "rotate row") {
            // parse row and amount
            size_t row = stoull(line.substr(13, line.find(" by")));
            size_t amount = stoull(line.substr(line.find("by ") + 3));

            // shift row
            screen.shiftRow(row, amount);
        } else if (line.substr(0, 13) == "rotate column") {
            // parse column and amount
            size_t column = stoull(line.substr(16, line.find(" by")));
            size_t amount = stoull(line.substr(line.find("by ") + 3));

            // shift column
            screen.shiftColumn(column, amount);
        }
    }

    // display results
    cout << "[first] # of lit pixels: " << screen.litPixels() << endl;
    cout << "[second] displayed pixels: " << endl << endl;
    screen.display();

    return 0;
}
