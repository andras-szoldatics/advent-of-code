#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class CompressedFile {
public:
    // different stages
    string content;

    CompressedFile(const string& line) :
        content(line) {}

    // utility functions functions
    size_t decompressFirst() const {
        return decompressEstimate(content, false);
    }

    size_t decompressSecond() const {
        return decompressEstimate(content, true);
    }

private:
    // utility functions
    size_t decompressEstimate(const string& input,
                              bool recursion = false) const {
        // temporary storage
        size_t estimatedSize = 0;

        // setup for iteration
        size_t cursor = 0;

        // iterate over input
        while (cursor < input.size()) {
            // check for marker
            if (input[cursor] == '(') {
                // parse marker
                size_t markerEnd = input.find(')', cursor);
                size_t markerLength = markerEnd - cursor - 1;
                string marker = input.substr(cursor + 1, markerLength);

                // parse length and repeat
                size_t lengthEnd = marker.find('x');
                size_t length = stoi(marker.substr(0, lengthEnd));
                size_t repeat = stoi(marker.substr(lengthEnd + 1));

                // calculate estimated size
                if (recursion) {
                    // recurse
                    string section = input.substr(markerEnd + 1, length);
                    estimatedSize += repeat * decompressEstimate(section, true);
                } else {
                    // no recursion
                    estimatedSize += repeat * length;
                }

                // update cursor
                cursor = markerEnd + length + 1;
            } else {
                // no marker
                estimatedSize += 1;
                cursor += 1;
            }
        }

        return estimatedSize;
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // parse line as compressed file
    CompressedFile file(line);

    // display results
    cout << "[first] decompressed size: " << file.decompressFirst() << endl;
    cout << "[second] decompressed size: " << file.decompressSecond() << endl;

    return 0;
}
