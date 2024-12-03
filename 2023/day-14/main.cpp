#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

using namespace std;

class Reflector {
public:
    // store data as columns
    vector<string> rocks;

    // helper functions
    int64_t getTotalLoad() const {
        // temporary storage
        int64_t totalLoad = 0;

        // iterate over each column
        for (const string& line : rocks) {
            // temporary storage
            int64_t currentLoad = static_cast<int64_t>(line.size());

            // iterate over each space
            for (char c : line) {
                if (c == 'O') {
                    // count load if rock
                    totalLoad += currentLoad;
                }

                // decrement load
                currentLoad -= 1;
            }
        }

        return totalLoad;
    }

    string getHash() const {
        // temporary storage
        string hash;
        string buffer;

        // iterate over each column
        for (const string& line : rocks) {
            // iterate over each space
            for (char c : line) {
                if (c == '#') {
                    buffer.push_back('2');
                } else if (c == 'O') {
                    buffer.push_back('1');
                } else {
                    buffer.push_back('0');
                }

                if (buffer.size() == 5) {
                    // convert to hex
                    size_t value = stoul(buffer, nullptr, 3);
                    char hex = static_cast<char>(value);
                    hash.push_back(hex);

                    // clear buffer
                    buffer.clear();
                }
            }

            // convert remainder
            if (!buffer.empty()) {
                // convert to hex
                size_t value = stoul(buffer, nullptr, 3);
                char hex = static_cast<char>(value);
                hash.push_back(hex);
            }

            // add buffer to hash
            hash.append(buffer);
            buffer.clear();
        }

        return hash;
    }

    void display() {
        // retrieve short-hands
        size_t size = rocks.size();

        // iterate over each column
        for (size_t r = 0; r < size; r++) {
            for (size_t c = 0; c < size; c++) {
                cout << rocks.at(c).at(r);
            }

            cout << endl;
        }

        cout << endl;
    }

    // utility functions
    void slideUp() {
        // iterate over each column
        for (string& line : rocks) {
            // temporary storage
            optional<size_t> emptySpace;

            for (size_t i = 0; i < line.size(); i++) {
                // retrieve short-hands
                char cursor = line.at(i);

                // mark first empty space
                if ((cursor == '.') &&
                    !emptySpace) {
                    emptySpace = i;
                } else if (cursor == 'O') {
                    // move each rock up, unless obstructed
                    if (emptySpace) {
                        line.at(emptySpace.value()) = 'O';
                        line.at(i) = '.';
                        emptySpace = emptySpace.value() + 1;
                    }
                } else if (cursor == '#') {
                    // reset empty space
                    emptySpace.reset();
                }
            }
        }
    }

    void rotateClockwise() {
        // allocate all spaces
        string emptyLine(rocks.size(), '.');
        vector<string> tmp(rocks.size(), emptyLine);
        size_t sum = rocks.size() - 1;

        // iterate over each column
        for (size_t c = 0; c < rocks.size(); c++) {
            // retrieve short-hands
            string& line = rocks.at(c);

            // iterate over each row
            for (size_t r = 0; r < line.size(); r++) {
                // set character to new column
                tmp.at(sum - r).at(c) = line.at(r);
            }
        }

        // update data
        rocks = tmp;
    }

    void spinCycle() {
        for (size_t i = 0; i < 4; i++) {
            // slide up
            slideUp();

            // rotate clockwise
            rotateClockwise();
        }
    }
};

int64_t shakeItBaby(Reflector& dish) {
    // temporary storage
    unordered_map<string, size_t> hashDetected;
    unordered_map<string, int64_t> hashLoad;

    // cycle the dish
    for (size_t i = 1; i <= 1'000'000'000; i++) {
        // spin it baby
        dish.spinCycle();

        // retrieve hash
        string hash = dish.getHash();

        // retrieve load
        if (hashDetected.count(hash) == 0) {
            hashDetected[hash] = i;
            hashLoad[hash] = dish.getTotalLoad();
        } else {
            size_t cycleLength = i - hashDetected.at(hash);
            cout << "found repeating hash, with cyclic length: " << cycleLength << endl;

            // skip to the end
            size_t cycles = (1'000'000'000 - i) / cycleLength;
            i += cycles * cycleLength;

            // clear hash
            hashDetected.clear();
        }
    }

    return 0;
}

int main() {
    // temporary storage
    string line;

    // patch storage
    Reflector firstDish;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // resize cols if required
        if (firstDish.rocks.size() < line.size()) {
            firstDish.rocks.resize(line.size());
        }

        // add character to each column
        for (size_t i = 0; i < line.size(); i++) {
            firstDish.rocks.at(i).push_back(line.at(i));
        }
    }

    // make a copy
    Reflector secondDish = firstDish;

    // slide up the dish
    firstDish.slideUp();

    // display resulting load
    int64_t firstLoad = firstDish.getTotalLoad();
    cout << "[first] total load: " << firstLoad << endl;

    // shake the dish
    shakeItBaby(secondDish);

    // display resulting load
    int64_t secondLoad = secondDish.getTotalLoad();
    cout << "[second] total load: " << secondLoad << endl;

    return 0;
}
