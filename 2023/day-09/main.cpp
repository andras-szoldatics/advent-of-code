#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Sequence {
public:
    vector<vector<int64_t>> numbers;

    Sequence(const vector<int64_t>& n) :
        numbers({ n }) {
        // generate deltas
        generateDeltas();
    }

    // utility functions
    void generateDeltas() {
        // setup for iterations
        bool onlyZeroes = false;

        // iterate over deltas
        while (!onlyZeroes) {
            // retrieve short-hands
            const vector<int64_t> latest = numbers.back();

            // temporary storage
            vector<int64_t> delta;

            // iterate over numbers
            onlyZeroes = true;
            for (size_t i = 1; i < latest.size(); ++i) {
                // calculate delta
                int64_t d = latest.at(i) - latest.at(i - 1);

                if (d != 0) {
                    onlyZeroes = false;
                }

                // store delta
                delta.push_back(d);
            }

            // store delta
            numbers.push_back(delta);
        }
    }

    void extend(bool plus) {
        // add 0 to latest sequence's end
        numbers.back().push_back(0);

        // iterate backwards on sequences and calculate from deltas
        for (size_t i = numbers.size() - 1; i > 0; --i) {
            // retrieve short-hands
            const vector<int64_t>& deltas = numbers.at(i);
            vector<int64_t>& values = numbers.at(i - 1);

            // calculate new value
            int64_t value = values.back();
            if (plus) {
                value += deltas.back();
            } else {
                value -= deltas.back();
            }
            values.push_back(value);
        }
    }


    void extendFuture() {
        // extend future
        extend(true);
    }

    void extendPast() {
        // reverse numbers
        reverseSequence();

        // extend future
        extend(false);

        // reverse numbers
        reverseSequence();
    }

    // helper functions
    void reverseSequence() {
        // reverse numbers
        for (vector<int64_t>& n : numbers) {
            reverse(n.begin(), n.end());
        }
    }
};

Sequence parseSequence(const string& line) {
    // temporary storage
    vector<string> tokens = { {} };
    vector<int64_t> numbers;

    // split line along spaces
    for (char c : line) {
        if (c == ' ') {
            tokens.push_back({});
        } else {
            tokens.back().push_back(c);
        }
    }

    // convert tokens to numbers
    for (const string& token : tokens) {
        try {
            int64_t number = stoll(token);
            numbers.push_back(number);
        } catch (...) {
            // NO-OP
        }
    }

    // create sequence
    Sequence sequence(numbers);

    return sequence;
}

int main() {
    // temporary storage
    string line;

    // store stuff
    vector<Sequence> sequences;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        Sequence sequence = parseSequence(line);
        sequences.push_back(sequence);
    }

    // setup for iteration
    int64_t firstSum = 0;
    int64_t secondSum = 0;

    // extend sequences
    for (Sequence& sequence : sequences) {
        // extend this sequence
        sequence.extendFuture();
        sequence.extendPast();

        // retrieve first value
        firstSum += sequence.numbers.front().back();
        secondSum += sequence.numbers.front().front();
    }

    // display result
    cout << "[first] sum of generated values: " << firstSum << endl;
    cout << "[second] sum of generated values: " << secondSum << endl;

    return 0;
}
