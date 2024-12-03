#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

string decodeLine(const string& line) {
    // temporary storage
    string tmp = line;

    // strip encompassing " characters
    tmp = tmp.substr(1);
    tmp.pop_back();

    // iterate over the string
    for (size_t i = 0; i < tmp.size(); i++) {
        // retrieve short-hands
        char current = tmp.at(i);

        // check for escape character
        if ((current == '\\') &&
            ((i + 1) < tmp.size())) {
            // check for next character
            char next = tmp.at(i + 1);

            if ((next == '\\') ||
                (next == '"')) {
                // wrap into single character
                tmp.erase(i, 1);
            } else if (next == 'x') {
                // substitute character
                tmp.at(i) = '?';

                // wrap into single character
                tmp.erase(i + 1, 3);
            }
        }
    }

    return tmp;
}

string encodeLine(const string& line) {
    // temporary storage
    string tmp;

    // add starting character
    tmp.push_back('"');

    // iterate over the string
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char current = line.at(i);

        if ((current == '"') ||
            (current == '\\')) {
            // add escape character
            tmp.push_back('\\');
        }

        // add original character
        tmp.push_back(current);
    }

    // add final character
    tmp.push_back('"');

    return tmp;
}

int main() {
    // temporary storage
    string line;

    // keep a running tally
    size_t totalOriginal = 0;
    size_t totalDecoded = 0;
    size_t totalEncoded = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        size_t originalSize = line.size();

        // decode line
        string decoded = decodeLine(line);
        size_t decodedSize = decoded.size();

        // encode line
        string encoded = encodeLine(line);
        size_t encodedSize = encoded.size();

        // add to totals
        totalOriginal += originalSize;
        totalDecoded += decodedSize;
        totalEncoded += encodedSize;
    }

    // display first result
    int64_t firstResult = static_cast<int64_t>(totalOriginal);
    firstResult -= static_cast<int64_t>(totalDecoded);

    cout << "[first] result of strings : " << firstResult << endl;

    // display second result
    int64_t secondResult = static_cast<int64_t>(totalEncoded);
    secondResult -= static_cast<int64_t>(totalOriginal);

    cout << "[second] result of strings : " << secondResult << endl;

    return 0;
}
