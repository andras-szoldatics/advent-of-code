#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class SNAFU {
public:
    // stored values
    vector<int64_t> values;

    SNAFU(const string& line = string("0")) {
        // iterate over line
        for (char c : line) {
            // seperate handling
            switch (c) {
            case '=':
                values.push_back(-2);
                break;
            case '-':
                values.push_back(-1);
                break;
            default:
                // hope for a digit
                if (isdigit(c)) {
                    int64_t v = (c - '0');
                    values.push_back(v);
                }
                break;
            }
        }

        // reverse values
        reverse(values.begin(), values.end());
    }

    // evaluation functions
    string asString() const {
        // temporary storage
        string tmp;

        // iterate over values
        for (const int64_t& value : values) {
            // separate handling
            switch (value) {
            case -2:
                tmp.push_back('=');
                break;
            case -1:
                tmp.push_back('-');
                break;
            default:
                // assemble as digit
                char c = static_cast<char>('0' + value);
                tmp.push_back(c);
            }
        }

        // reverse characters
        reverse(tmp.begin(), tmp.end());

        return tmp;
    }

    int64_t asInt64() const {
        // temporary storage
        int64_t tmp = 0;

        int64_t part = 1;
        // iterate over values
        for (const int64_t& value : values) {
            // add value * part to total
            tmp += (value * part);

            // shift part to next
            part *= 5;
        }

        return tmp;
    }

    // operator functions
    void operator+=(const SNAFU& rhs) {
        // setup for iteration
        if (values.size() < rhs.values.size()) {
            values.resize(rhs.values.size(), 0);
        }

        // iterate over right-hand values
        for (size_t i = 0; i < rhs.values.size(); i++) {
            // add to values as is
            values.at(i) += rhs.values.at(i);
        }

        // iterate over current values
        for (size_t i = 0; i < values.size(); i++) {
            // retrieve short-hands
            int64_t& currentValue = values.at(i);

            // normalize values
            while (currentValue > 2) {
                // normalize down
                currentValue -= 5;

                if (values.size() > (i + 1)) {
                    // shift next value
                    values.at(i + 1)++;
                } else {
                    // add default value
                    values.push_back(1);
                }
            }

            while (currentValue < -2) {
                // normalize up
                currentValue += 5;

                if (values.size() > (i + 1)) {
                    // shift next value
                    values.at(i + 1)--;
                } else {
                    // add default value
                    values.push_back(-1);
                }
            }
        }
    }
};

int main() {
    // temporary storage
    string line;

    // stored SNAFU values
    vector<SNAFU> snafus;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line as SNAFU
        SNAFU snafu(line);

        // add to storage
        snafus.push_back(snafu);
    }

    SNAFU total;
    // iterate over values
    for (const SNAFU& snafu : snafus) {
        // display each SNAFU
        cout << snafu.asString() << " -> " << snafu.asInt64() << endl;

        // add to total
        total += snafu;
    }

    // display result
    cout << "[first] total: " << total.asString() << " -> " << total.asInt64() << endl;

    return 0;
}
