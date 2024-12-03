#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

using namespace std;

int64_t findBestProduct(const vector<int64_t>& packages,
                        const int64_t& targetNumber,
                        const int64_t& width,
                        const vector<int64_t>& selected = {}) {
    // calculate sum of selection
    int64_t sumOfSelection = accumulate(selected.begin(), selected.end(), 0l);
    if (sumOfSelection > targetNumber) {
        return numeric_limits<int64_t>::max();
    }

    if (width == static_cast<int64_t>(selected.size())) {
        if (sumOfSelection != targetNumber) {
            return numeric_limits<int64_t>::max();
        } else {
            // temporary storage
            int64_t product = 1;

            // iterate over selection
            for (int64_t entry : selected) {
                product *= entry;
            }

            return product;
        }
    }

    // keep a running tally
    int64_t bestProduct = numeric_limits<int64_t>::max();

    // iterate over possible packages
    for (int64_t package : packages) {
        // check for invalid choices
        if ((!selected.empty()) &&
            (selected.back() >= package)) {
            // skip invalid choice
            continue;
        }

        // extend selection
        vector<int64_t> extended = selected;
        extended.push_back(package);

        // determine sub-products
        int64_t localProduct = findBestProduct(packages, targetNumber, width, extended);

        // set best product
        if (bestProduct > localProduct) {
            bestProduct = localProduct;
        }
    }

    return bestProduct;
}

int64_t assignPackages(const vector<int64_t>& packages,
                       int64_t targetNumber) {
    // keep a running tally
    int64_t bestProduct = numeric_limits<int64_t>::max();
    int64_t width = 0;

    // execute iteration
    while (bestProduct == numeric_limits<int64_t>::max()) {
        // execute some tricky function
        bestProduct = findBestProduct(packages, targetNumber, width);

        // extend width
        width++;
    }

    return bestProduct;
}

int main() {
    // temporary storage
    string line;

    // list of packages
    vector<int64_t> packages;
    int64_t totalWeight = 0;

    // read input filef
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse package weight
        int64_t weight = stoll(line);

        // keep a running tally
        packages.push_back(weight);
        totalWeight += weight;
    }

    // calculate even division
    int64_t firstTarget = (totalWeight / 3);
    int64_t secondTarget = (totalWeight / 4);

    // run processing
    int64_t firstResult = assignPackages(packages, firstTarget);
    int64_t secondResult = assignPackages(packages, secondTarget);

    // display results
    cout << "[first] best quantum entanglement score : " << firstResult << endl;
    cout << "[second] best quantum entanglement score : " << secondResult << endl;

    return 0;
}
