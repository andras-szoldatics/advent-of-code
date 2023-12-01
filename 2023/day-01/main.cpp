#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int64_t getNumbers(const string& line) {
    // temporary storage
    vector<int64_t> numbers;

    // iterate forward
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char c = line.at(i);

        // check for numeric value
        if (isdigit(c)) {
            // convert to int64
            int64_t value = c - '0';
            numbers.push_back(value);
        }
    }

    // check for empty
    if (numbers.empty()) {
        return 0;
    }

    return numbers.front() * 10 + numbers.back();
}

const vector<string> DIGITS = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

int64_t getCalibration(const string& line) {
    // temporary storage
    vector<int64_t> numbers;

    // iterate forward
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char c = line.at(i);

        // check for numeric value
        if (isdigit(c)) {
            // convert to int64
            int64_t value = c - '0';
            numbers.push_back(value);
        } else {
            // check against text digits
            for (size_t d = 0; d < DIGITS.size(); d++) {
                // retrieve short-hands
                const string& digit = DIGITS.at(d);

                // get partial string
                string partial = line.substr(i, digit.size());

                // check for match
                if (partial == digit) {
                    // convert to int64
                    int64_t value = static_cast<int64_t>(d + 1);
                    numbers.push_back(value);
                }
            }
        }
    }

    return numbers.front() * 10 + numbers.back();
}

int main() {
    // temporary storage
    string line;

    // setup for iteration
    int64_t firstResult = 0;
    int64_t secondResult = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // get numbers
        int64_t numbers = getNumbers(line);
        firstResult += numbers;

        // get calibration
        int64_t calibration = getCalibration(line);
        secondResult += calibration;
    }

    // display result
    cout << "[first] calibration value: " << firstResult << endl;
    cout << "[second] calibration value: " << secondResult << endl;

    return 0;
}
