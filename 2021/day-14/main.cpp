#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <unordered_map>

using namespace std;

typedef unordered_map<string, int64_t> Polymer;
typedef unordered_map<string, char> Rules;

void addToPolymer(Polymer& polymer,
                  const string& link,
                  int64_t count = 1) {
    if (polymer.count(link) == 0) {
        // add entry if not present
        polymer[link] = 0;
    }

    // increment count
    polymer.at(link) += count;
}

int64_t evaluatePolymer(const Polymer& polymer) {
    // temporary storage
    unordered_map<char, int64_t> elements;

    // iterate over entries
    for (const pair<const string, int64_t>& entry : polymer) {
        // get element
        char element = entry.first.front();

        // add to elements
        if (elements.count(element) == 0) {
            elements[element] = 0;
        }

        // increment count
        elements.at(element) += entry.second;
    }

    // find most & least abundant elements
    int64_t mostAbundant = 0;
    int64_t leastAbundant = numeric_limits<int64_t>::max();

    for (const pair<const char, int64_t>& entry : elements) {
        // check if most abundant
        if (mostAbundant < entry.second) {
            mostAbundant = entry.second;
        }

        // check if least abundant
        if (leastAbundant > entry.second) {
            leastAbundant = entry.second;
        }
    }

    // return difference
    return mostAbundant - leastAbundant;
}

Polymer lineToPolymer(const string& line) {
    // temporary storage
    Polymer polymer;

    // split line into links
    for (size_t i = 0; i < line.size(); i++) {
        // get element
        string element = line.substr(i, 2);

        // add to polymer
        addToPolymer(polymer, element);
    }

    return polymer;
}

Polymer transfigure(const Polymer& input,
                    const Rules& rules) {
    // temporary storage
    Polymer output;

    // iterate over entries
    for (const pair<const string, int64_t>& entry : input) {
        // check if we have a rule for this
        if (rules.count(entry.first) == 0) {
            // make a copy of the link
            addToPolymer(output, entry.first, entry.second);
        } else {
            // retrieve short-hands
            char replacement = rules.at(entry.first);

            // split link into two
            string first(2, entry.first.front());
            string second(2, entry.first.back());

            // change relevant element
            first.back() = replacement;
            second.front() = replacement;

            // add to polymer
            addToPolymer(output, first, entry.second);
            addToPolymer(output, second, entry.second);
        }
    }

    return output;
}

int main() {
    // temporary storage
    string line;
    bool readPolymer = false;

    // starting polymer
    Polymer polymer;
    Rules rules;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // switch template reading
        if (line.empty()) {
            readPolymer = true;
            continue;
        }

        // read template / rules
        if (!readPolymer) {
            polymer = lineToPolymer(line);
        } else {
            // get rule
            string element = line.substr(0, 2);
            char replacement = line.back();

            // add to rules
            rules[element] = replacement;
        }
    }

    // transfigure polymer
    for (int i = 0; i < 40; i++) {
        if (i == 10) {
            // evaluate polymer
            int64_t firstResult = evaluatePolymer(polymer);

            // display result
            cout << "[first] evaluated polymer: " << firstResult << endl;
        }

        polymer = transfigure(polymer, rules);
    }

    // evaluate polymer
    int64_t secondResult = evaluatePolymer(polymer);

    // display resul
    cout << "[second] evaluated polymer: " << secondResult << endl;

    return 0;
}
