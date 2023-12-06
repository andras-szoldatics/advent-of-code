#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<int64_t> splitAndConvert(const string& group) {
    // temporary storage
    vector<int64_t> result;
    string remainder = group;

    while (!remainder.empty()) {
        // find next space
        size_t pos = remainder.find(' ');
        string part = remainder.substr(0, pos);

        if (pos != string::npos) {
            // remove part from remainder
            remainder = remainder.substr(pos + 1);
        } else {
            // remove part from remainder
            remainder.clear();
        }

        if (part.empty()) {
            continue;
        }

        // try to convert to integer
        try {
            int64_t number = stoll(part);
            result.push_back(number);
        } catch (...) {
            // NO-OP
        }
    }

    return result;
}

int64_t getWinningMoves(int64_t time,
                        int64_t target) {
    // temporary storage
    int64_t result = 0;

    // check different times
    for (int64_t t = 1; t < time; t++) {
        if (t * (time - t) > target) {
            result += 1;
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // store seed numbers & conversion
    vector<int64_t> times;
    vector<int64_t> targets;

    // read input file
    ifstream input("input.txt");

    // get times
    getline(input, line);
    times = splitAndConvert(line);

    // get targets
    getline(input, line);
    targets = splitAndConvert(line);

    // iterate over all pairs
    int64_t firstResult = 1;

    for (size_t i = 0; i < times.size(); i++) {
        firstResult *= getWinningMoves(times.at(i), targets.at(i));
    }

    // display result
    cout << "[first] extent of winning moves: " << firstResult << endl;

    // correct for bad kerning
    string stringTime;
    string stringTarget;

    for (size_t i = 0; i < times.size(); i++) {
        stringTime += to_string(times.at(i));
        stringTarget += to_string(targets.at(i));
    }

    // calculate real winning moves
    int64_t mergedTime = stoll(stringTime);
    int64_t mergedTarget = stoll(stringTarget);
    int64_t secondResult = getWinningMoves(mergedTime, mergedTarget);

    // display result
    cout << "[second] # of winning moves: " << secondResult << endl;

    return 0;
}
