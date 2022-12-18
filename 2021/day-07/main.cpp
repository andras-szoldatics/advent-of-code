// C++ standard includes
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read line of crab mobiles
    getline(input, line);

    // assign line as buffer
    stringstream buffer(line);
    string stringValue;

    // list of crab mobiles
    vector<int64_t> crabMobiles;
    int64_t meanValue = 0;

    // iterate over line
    while (getline(buffer, stringValue, ',')) {
        // parse value
        int64_t crabMobile = stoll(stringValue);
        crabMobiles.push_back(crabMobile);

        // adjust mean value
        meanValue += crabMobile;
    }

    // check for early termination
    if (crabMobiles.empty()) {
        return 1;
    }

    // divide total to get mean value
    meanValue += 1;
    meanValue /= crabMobiles.size();

    // order crab mobiles for easier handling
    sort(crabMobiles.begin(), crabMobiles.end());

    int64_t medianValue = 0;
    // calculate median value
    if (crabMobiles.size() % 2 == 0) {
        medianValue += crabMobiles.at(crabMobiles.size() / 2 - 1);
        medianValue += crabMobiles.at(crabMobiles.size() / 2);
        medianValue /= 2;
    } else {
        medianValue = crabMobiles.at(crabMobiles.size() / 2 + 1);

    }

    // change between median or mean
    cout << "median position value : " << medianValue << endl;
    cout << "mean position value : " << meanValue << endl;

    int64_t firstTotalFuel = 0;
    // calculate fuel requirements
    for (const int64_t& crabMobile : crabMobiles) {
        // calculate current fuel
        int64_t delta = abs(medianValue - crabMobile);

        // add to total
        firstTotalFuel += delta;
    }

    // display results
    cout << "[first] total fuel : " << firstTotalFuel << endl;

    int64_t secondTotalFuel = 0;
    // calculate fuel requirements
    for (const int64_t& crabMobile : crabMobiles) {
        // calculate current fuel
        int64_t delta = abs(meanValue - crabMobile);
        delta *= (delta + 1);
        delta /= 2;

        // add to total
        secondTotalFuel += delta;
    }

    // display results
    cout << "[second] total fuel : " << secondTotalFuel << endl;

    return 0;
}
