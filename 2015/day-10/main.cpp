// C++ standard includes
#include <fstream>
#include <iostream>

using namespace std;

string lookAndSay(const string& input) {
    // temporary storage
    string output;

    // iterate over input
    for (size_t i = 0; i < input.size(); i++) {
        // retrieve short-hands
        char current = input.at(i);

        // setup for iteration
        int64_t count = 1;

        // iterate until matching characters are found
        while ((i + 1) < input.size()) {
            // check for break in pattern
            if (current != input.at(i + 1)) {
                break;
            }

            // increase counters
            count++;
            i++;
        }

        // extend output with current pattern
        output += to_string(count);
        output += current;
    }

    return output;
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // setup for iteration
    string result = line;

    for (size_t i = 1; i <= 50; i++) {
        // execute transformation
        result = lookAndSay(result);

        // display results
        if (i == 40) {
            cout << "[first] length of line after 40 iterations : " << result.size() << endl;
        } else if (i == 50) {
            cout << "[second] length of line after 50 iterations : " << result.size() << endl;
        }
    }

    return 0;
}
