#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>

using namespace std;

string PATTERN_1 = R"(do\(\)|don't\(\)|mul\(([0-9]+),([0-9]+)\))";

int main() {
    // temporary storage
    string line;
    sregex_iterator end;

    // setup for iteration
    int64_t firstTotal = 0;
    int64_t secondTotal = 0;
    bool doing = true;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // matching whole line
        regex matcher1(PATTERN_1);
        sregex_iterator it(line.begin(), line.end(), matcher1);

        while (it != end) {
            if (it->str() == string("do()")) {
                doing = true;
            } else if (it->str() == string("don't()")) {
                doing = false;
            } else {
                // the numbers, what do they mean?
                int64_t a = stoll(it->str(1));
                int64_t b = stoll(it->str(2));

                firstTotal += (a * b);
                if (doing) {
                    secondTotal += (a * b);
                }
            }

            // shift
            it++;
        }
    }

    // display results
    cout << "[first] total: " << firstTotal << endl;
    cout << "[second] total: " << secondTotal << endl;

    return 0;
}
