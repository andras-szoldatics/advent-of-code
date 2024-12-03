#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // keep a running tally of calories
    vector<int64_t> elves(1, 0);

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line into vector
        if (line.empty()) {
            // begin new entry
            elves.push_back(0);
        } else {
            // add line into previous entry
            int64_t calories = stoll(line);
            elves.back() += calories;
        }
    }

    // order the vector according to values
    sort(elves.begin(), elves.end(), greater<int64_t>());

    cout << "top elf : " << elves.at(0) << endl;
    if (elves.size() >= 3) {
        // retrieve the top 3 elves
        cout << "sum of top three elves : " << (elves.at(0) + elves.at(1) + elves.at(2)) << endl;
    }

    return 0;
}
