#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cassert>

using namespace std;

typedef unordered_map<string, unordered_map<string, int64_t>> Changes;

int64_t findBestSeating(const unordered_set<string>& guests,
                        const Changes& changes) {
    // keep a running tally
    int64_t bestTotal = numeric_limits<int64_t>::min();

    // setup for iteration
    vector<string> currentOrder(guests.begin(), guests.end());
    sort(currentOrder.begin(), currentOrder.end());
    bool hasNext = true;

    // remove last element as fix point
    string fixGuest = currentOrder.back();
    currentOrder.pop_back();

    while (hasNext) {
        // temporary storage
        int64_t totalChange = 0;

        for (size_t i = 0; i <= currentOrder.size(); i++) {
            // retrieve short-hands
            const string& past = (i == 0 ? fixGuest : currentOrder.at(i - 1));
            const string& present = (i == currentOrder.size() ? fixGuest : currentOrder.at(i));

            // add pair of changes
            totalChange += changes.at(past).at(present);
            totalChange += changes.at(present).at(past);
        }

        if (bestTotal < totalChange) {
            bestTotal = totalChange;
        }

        // retrieve next permutation
        hasNext = next_permutation(currentOrder.begin(), currentOrder.end());
    }

    return bestTotal;
}

int main() {
    // temporary storage
    string line, tmp;

    // distance matrix
    unordered_map<string, unordered_map<string, int64_t>> changes;
    unordered_set<string> guests;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // parse first guest
        string from;
        getline(buffer, from, ' ');

        // skip next word
        getline(buffer, tmp, ' ');

        // parse second place
        string gainOrLoss;
        getline(buffer, gainOrLoss, ' ');

        // parse distance
        getline(buffer, tmp, ' ');
        int64_t change = stoll(tmp);
        if (gainOrLoss != string("gain")) {
            change *= -1;
        }

        // skip next six words
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse second guest
        string to;
        getline(buffer, to, '.');

        if (guests.count(from) == 0) {
            // fill out vector of guests
            guests.insert(from);
        }

        if (guests.count(to) == 0) {
            // fill out vector of guests
            guests.insert(to);
        }

        // assign defined value forward & backward
        changes[from][to] = change;
    }

    // calculate first results
    int64_t firstBest = findBestSeating(guests, changes);

    // setup for second part
    const string myself("MYSELF");

    // iterate over current guests
    for (const string& guest : guests) {
        // add change pair
        changes[guest][myself] = 0;
        changes[myself][guest] = 0;
    }

    // add myself to the guest list
    guests.insert("MYSELF");

    // calculate second results
    int64_t secondBest = findBestSeating(guests, changes);

    // display results
    cout << "[first] best total change : " << firstBest << endl;
    cout << "[second] best total change : " << secondBest << endl;

    return 0;
}
