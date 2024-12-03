#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>

using namespace std;

int main() {
    // temporary storage
    string line;

    // previous measurement
    size_t firstIncreases = 0;
    size_t secondIncreases = 0;

    // sample window size
    const size_t WINDOW_SIZE = 3;
    list<int64_t> window;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        int64_t currentDepth = stoll(line);

        // check for first increase
        if ((!window.empty()) &&
            (window.back() < currentDepth)) {
            firstIncreases++;
        }

        // add new depth to window
        window.push_back(currentDepth);

        // sliding window must be 4 to evaluate
        if (window.size() > WINDOW_SIZE) {
            // check front to back change
            if (window.back() > window.front()) {
                secondIncreases++;
            }

            // remove first element
            window.pop_front();
        }
    }

    cout << "first method increases : " << firstIncreases << endl;
    cout << "second method increases : " << secondIncreases << endl;

    return 0;
}
