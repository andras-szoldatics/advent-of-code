// C++ standard incldues
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

const unordered_set<char> vowels = { 'a', 'e', 'i', 'o', 'u' };
const vector<string> badStrings = { "ab", "cd", "pq", "xy" };

bool isNiceFirst(const std::string& line) {
    // filter for bad strings
    for (const string& badString : badStrings) {
        // find bad string
        if (line.find(badString) != string::npos) {
            return false;
        }
    }

    // keep a running tally
    size_t vowelCount = 0;
    size_t doubleCount = 0;

    // iterate over line
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char current = line.at(i);

        // count vowels
        if (vowels.count(current) != 0) {
            vowelCount += 1;
        }

        if (i >= 1) {
            // retrieve previous letter
            char previous = line.at(i - 1);

            // count double letter
            if (current == previous) {
                doubleCount += 1;
            }
        }
    }

    return ((vowelCount >= 3) && (doubleCount >= 1));
}

bool isNiceSecond(const std::string& line) {
    // keep a running tally
    size_t repeatCount = 0;
    size_t doubleCount = 0;

    // iterate over line
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char current = line.at(i);

        if (i >= 1) {
            // retrieve two letter string
            string pair = line.substr(i - 1, 2);

            // count matching pair
            if (line.find(pair, i + 1) != string::npos) {
                repeatCount += 1;
            }
        }

        if (i >= 2) {
            // retrieve letter two before
            char previous = line.at(i - 2);

            // count double letter
            if (current == previous) {
                doubleCount += 1;
            }
        }
    }

    return ((repeatCount >= 1) && (doubleCount >= 1));
}

int main() {
    // temporary storage
    string line;

    // keep a running tally
    int64_t niceFirst = 0;
    int64_t niceSecond = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check if this line is nice
        if (isNiceFirst(line)) {
            niceFirst += 1;
        }

        if (isNiceSecond(line)) {
            niceSecond += 1;
        }
    }

    // display result for first part
    cout << "first # of nice strings : " << niceFirst << endl;

    // display result for second part
    cout << "second # of nice strings : " << niceSecond << endl;

    return 0;
}
