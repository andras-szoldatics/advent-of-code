// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

typedef pair<char, int64_t> Entry;
typedef unordered_map<char, int64_t> Ledger;

bool firstDecode(const Entry& rhs,
                 const Entry& lhs) {
    if (rhs.second > lhs.second) {
        return true;
    }

    if (rhs.second < lhs.second) {
        return false;
    }

    return (rhs.first < lhs.first);
}

bool secondDecode(const Entry& rhs,
                  const Entry& lhs) {
    if (rhs.second < lhs.second) {
        return true;
    }

    if (rhs.second > lhs.second) {
        return false;
    }

    return (rhs.first < lhs.first);
}

class Decoder {
public:
    // storage
    vector<Ledger> ledgers;

    // helper functions
    void addLine(const string& line) {
        // resize vector, if necessary
        if (ledgers.size() < line.size()) {
            ledgers.resize(line.size());
        }

        // add line to counting
        for (size_t i = 0; i < line.size(); i++) {
            // retrieve short-hands
            Ledger& ledger = ledgers.at(i);
            char c = line.at(i);

            // add character to ledger
            if (ledger.count(c) != 0) {
                ledger.at(c) += 1;
            } else {
                ledger[c] = 1;
            }
        }
    }

    // utility functions
    string decode(const function<bool(const Entry&, const Entry&)>& algorithm) const {
        // temporary storage
        string message;

        // iterate over ledgers
        for (const Ledger& ledger : ledgers) {
            // temporary storage
            vector<Entry> entries;

            for (const pair<const char, int64_t>& entry : ledger) {
                // add entries to vector
                entries.push_back(entry);
            }

            // sort entries
            sort(entries.begin(), entries.end(), algorithm);

            // add top entry to message
            message += entries.front().first;
        }

        return message;
    }
};

int main() {
    // temporary storage
    string line;

    // decoder for message
    Decoder decoder;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // add line as-is
        decoder.addLine(line);
    }

    // calculate first part
    string firstMessage = decoder.decode(firstDecode);
    cout << "[first] decoded message : " << firstMessage << endl;

    // calculate second part
    string secondMessage = decoder.decode(secondDecode);
    cout << "[second] decoded message : " << secondMessage << endl;

    return 0;
}
