#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int64_t ELF_BASE = 26;
const size_t ELF_WIDTH = 8;
const char ELF_ZERO = 'a';

int64_t elfToInt(const string& input) {
    // temporary storage
    int64_t tmp = 0;

    // iterate over string
    for (char c : input) {
        // shift number
        tmp *= ELF_BASE;

        // generate partial number
        int64_t partial = (c - ELF_ZERO);

        // add partial
        tmp += partial;
    }

    return tmp;
}

string intToElf(int64_t input) {
    // temporary storage
    int64_t remainder = input;
    string tmp;

    // iterate over integer
    for (size_t i = 0; i < ELF_WIDTH; i++) {
        // generate numeral
        int64_t numeral = (remainder % ELF_BASE);
        char c = ELF_ZERO;
        while (numeral--) {
            c++;
        }

        // add numeral to string
        tmp.push_back(c);

        // reduce number
        remainder /= ELF_BASE;
    }

    // reverse string
    reverse(tmp.begin(), tmp.end());

    return tmp;
}

bool valid(int64_t password) {
    // convert back to elf
    string elf = intToElf(password);

    // setup for iteration
    bool increasing = false;
    vector<size_t> pairs;

    // iterate over string
    for (size_t i = 0; i < elf.size(); i++) {
        // retrieve short-hands
        char current = elf.at(i);

        if (i >= 1) {
            // retrieve short-hands
            char pastOne = elf.at(i - 1);

            // check for pair
            if (current == pastOne) {
                if ((pairs.empty()) ||
                    (pairs.back() + 1 < i)) {
                    pairs.push_back(i);
                }
            }

            if ((i >= 2) &&
                (!increasing)) {
                // retrieve short-hands
                char pastTwo = elf.at(i - 2);

                // check for tri-value
                if ((pastTwo + 2 == current) &&
                    (pastOne + 1 == current)) {
                    increasing = true;
                }
            }
        }

        // filter for unallowed letters
        if ((current == 'i') ||
            (current == 'l') ||
            (current == 'o')) {
            return false;
        }
    }

    // evaluate pairs
    bool paired = (pairs.size() >= 2);

    return (increasing && paired);
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // convert to integer
    int64_t original = elfToInt(line);

    // setup for iteration
    int64_t first = (original + 1);

    while (!valid(first)) {
        // try next password
        first += 1;
    }

    // display first result
    cout << "[first] next password : " << intToElf(first) << endl;

    // setup for iteration
    int64_t second = (first + 1);

    while (!valid(second)) {
        // try next password
        second += 1;
    }

    // display second result
    cout << "[second] next password : " << intToElf(second) << endl;

    return 0;
}
