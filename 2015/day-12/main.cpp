// C++ standard includes
#include <fstream>
#include <iostream>

using namespace std;

int64_t parse(const string& line,
              size_t& position,
              bool skipRed) {
    // temporary storage
    int64_t result = 0;
    bool zeroOut = false;

    while (position < line.size()) {
        // retrieve short-hands
        char current = line.at(position);

        // seperate parsing for elements
        switch (current) {
        case '{':
        case '[':
            // move cursor
            position++;

            // retrieve sub-result
            result += parse(line, position, skipRed);
            break;
        case '}':
            // move cursor
            position++;

            // return sub-result
            return (zeroOut ? 0 : result);
            break;
        case ']':
            // move cursor
            position++;

            // return sub-result
            return result;
            break;
        case '"':
            // move cursor
            position++;

            if (position >= 0) {
                // temporary storage
                size_t endOfValue = line.find('"', position);
                string value = line.substr(position, endOfValue - position);

                if ((skipRed) &&
                    (value == string("red"))) {
                    zeroOut = true;
                }

                // move cursor
                position = (endOfValue + 1);
            }

            break;
        default:
            if ((isdigit(current)) ||
                (current == '-')) {
                // temporary storage
                size_t length = 0;

                // accumulate result
                string subLine = line.substr(position);
                result += stoi(subLine, &length);

                // move cursor
                position += length;
            } else {
                // other value
                position++;
            }
            break;
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // temporary storage
    size_t firstPosition = 0;
    size_t secondPosition = 0;

    // display results
    cout << "[first] sum of all numbers : " << parse(line, firstPosition, false) << endl;
    cout << "[second] sum of all numbers : " << parse(line, secondPosition, true) << endl;

    return 0;
}
