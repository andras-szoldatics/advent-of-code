#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

string findMostCommonBits(const vector<string>& values) {
    // temporary storage
    size_t bitWidth = values.front().size();
    vector<size_t> zeroValues(bitWidth, 0);
    string tmp;

    // iterate over values
    for (const string& value : values) {
        // iterate over value
        for (size_t i = 0; i < bitWidth; i++) {
            // mark zero values
            if (value.at(i) == '0') {
                zeroValues.at(i)++;
            }
        }
    }

    // iterate over bit width
    for (size_t i = 0; i < bitWidth; i++) {
        if (zeroValues.at(i) > (values.size() / 2)) {
            tmp.push_back('0');
        } else {
            tmp.push_back('1');
        }
    }

    return tmp;
}

string findOneValue(const vector<string>& values,
                    bool findMostCommon,
                    size_t currentIndex = 0) {
    // check for termination
    if (values.size() == 1) {
        return values.front();
    }

    // temporary storage
    vector<string> oneValues;
    vector<string> zeroValues;

    // iterate over values to categorize them
    for (const string& value : values) {
        // check index bit
        if (value.at(currentIndex) == '1') {
            oneValues.push_back(value);
        } else {
            zeroValues.push_back(value);
        }
    }

    // check which is the more relevant
    if (oneValues.size() >= zeroValues.size()) {
        return findOneValue((findMostCommon ? oneValues : zeroValues), findMostCommon, currentIndex + 1);
    }

    return findOneValue((findMostCommon ? zeroValues : oneValues), findMostCommon, currentIndex + 1);
}

int64_t stringToInt(const string& value) {
    // temporary storage
    int64_t tmp = 0;

    // iterate over string
    for (char entry : value) {
        // shift value
        tmp <<= 1;

        // check for a 1 bit
        if (entry == '1') {
            tmp += 1;
        }
    }

    return tmp;
}

int main() {
    // temporary storage
    string line;

    // storing values
    vector<string> values;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // add line to storage
        values.push_back(line);
    }

    // check for termination
    if (values.empty()) {
        return 1;
    }

    // find most common bits
    string mostCommonBits = findMostCommonBits(values);
    int64_t gammaRate = stringToInt(mostCommonBits);

    // generate least common bits
    string leastCommonBits;
    for (char c : mostCommonBits) {
        if (c == '1') {
            leastCommonBits.push_back('0');
        } else {
            leastCommonBits.push_back('1');
        }
    }
    int64_t episolonRate = stringToInt(leastCommonBits);

    // display values
    cout << "power consumption : " << (gammaRate * episolonRate) << endl;

    // find most common value
    string mostCommonValue = findOneValue(values, true);
    int64_t oxygenGenerator = stringToInt(mostCommonValue);

    // find least common value
    string leastCommonValue = findOneValue(values, false);
    int64_t co2Scrubber = stringToInt(leastCommonValue);

    // display values
    cout << "life support rating : " << (oxygenGenerator * co2Scrubber) << endl;

    return 0;
}
