#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef pair<string, string> Replacement;

void extendElements(unordered_set<string>& results,
                    const Replacement& replacement,
                    const string& molecule) {
    // state for last match
    size_t last = 0;

    // iterate over string
    while (last != string::npos) {
        // find next matching part
        last = molecule.find(replacement.first, last);

        // replace matching part
        if (last != string::npos) {
            // assemble new string
            string result = molecule;
            result.replace(last, replacement.first.size(), replacement.second);

            // add result to set
            results.insert(result);

            // shift position to avoid repetition
            last += 1;
        }
    }
}

size_t reduceElements(const vector<Replacement>& replacements,
                      const string& molecule) {
    // temporary storage
    string current = molecule;
    size_t steps = 0;

    while (current != string("e")) {
        // iterate over possible replacements
        for (const Replacement& replacement : replacements) {
            // retrieve position
            size_t first = current.find(replacement.second);

            if (first != string::npos) {
                current.replace(first, replacement.second.size(), replacement.first);

                // count step
                steps += 1;
            }

            // check for early termination
            if (current == string("e")) {
                break;
            }
        }
    }

    return steps;
}

int main() {
    // temporary storage
    string line, molecule;

    // list of replacements
    vector<Replacement> replacements;

    // read input filef
    ifstream input("input.txt");
    while (getline(input, line)) {
        // seperate line types
        if (line.empty()) {
            getline(input, molecule);
        } else {
            // temporary storage
            Replacement entry;

            // assign line to buffer
            stringstream buffer(line);

            // parse replacement original
            getline(buffer, entry.first, ' ');

            // skip next word
            getline(buffer, entry.second, ' ');

            // parse replacement new
            getline(buffer, entry.second, ' ');

            // add replacement to list
            replacements.push_back(entry);
        }
    }

    // unique set of results
    unordered_set<string> firstResults;

    // iterate over replacements
    for (const Replacement& replacement : replacements) {
        // execute possible replacements
        extendElements(firstResults, replacement, molecule);
    }

    // display results
    cout << "[first] # of possible molecules produced : " << firstResults.size() << endl;
    cout << "[second] # of steps to produce molecule : " << reduceElements(replacements, molecule) << endl;

    return 0;
}
