#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef vector<int64_t> Update;

typedef unordered_map<int64_t, unordered_set<int64_t>> Rules;

bool checkOrder(const Rules& rules,
                int64_t first,
                int64_t second) {
    if ((rules.count(second) != 0) &&
        (rules.at(second).count(first) != 0)) {
        return false;
    }

    return true;
}

int main() {
    // temporary storage
    string line, s;

    // setup storage for rules and updates
    vector<Update> updates;
    Rules rules;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // terminate at first block
        if (line.empty()) {
            break;
        }

        // assign line to buffer
        stringstream buffer(line);

        // parse first number
        getline(buffer, s, '|');
        int64_t first = stoll(s);

        // parse second number
        getline(buffer, s);
        int64_t second = stoll(s);

        // store in rules
        rules[first].insert(second);
    }

    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);
        Update update;

        while (getline(buffer, s, ',')) {
            // parse numbers
            int64_t value = stoll(s);
            update.push_back(value);
        }

        // store in updates
        updates.push_back(update);
    }

    // setup for iteration
    int64_t firstTotal = 0;
    int64_t secondTotal = 0;
    function compare = [&rules] (int64_t f, int64_t s) -> bool {
            return checkOrder(rules, f, s);
        };

    // iterate over updates
    for (Update& update : updates) {
        bool correct = true;

        // check pairwise values
        for (size_t i = 0; i < update.size(); i++) {
            for (size_t j = i + 1; j < update.size(); j++) {
                if (!checkOrder(rules, update.at(i), update.at(j))) {
                    correct = false;
                    break;
                }
            }
        }

        if (correct) {
            // add to total
            firstTotal += update.at(update.size() / 2);
        } else {
            // re-sort the update
            stable_sort(update.begin(), update.end(), compare);

            // add to total
            secondTotal += update.at(update.size() / 2);
        }
    }

    cout << "[first] correct updates: " << firstTotal << endl;
    cout << "[second] fixed updates: " << secondTotal << endl;

    return 0;
}
