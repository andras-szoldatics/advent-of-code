#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

int64_t evolve(int64_t secret) {
    int64_t result = secret;

    // slightly reordered operations
    result ^= (result * 64) % (1 << 24);
    result ^= (result / 32) % (1 << 24);
    result ^= (result * 2'048) % (1 << 24);

    return result;
}

vector<int64_t> getValues(int64_t secret) {
    vector<int64_t> values = { secret };

    // generate the required values
    for (int i = 0; i < 2'000; i++) {
        secret = evolve(secret);
        values.push_back(secret);
    }

    return values;
}

size_t MAGIC_NUMBER = 19 * 19 * 19 * 19;

size_t asIndex(int64_t d1,
               int64_t d2,
               int64_t d3,
               int64_t d4) {
    // presume delta of -9 to 9
    size_t index = (d1 + 9);

    index *= 19;
    index += (d2 + 9);

    index *= 19;
    index += (d3 + 9);

    index *= 19;
    index += (d4 + 9);

    return index;
}

void processValues(const vector<int64_t>& values,
                   vector<int64_t>& bananas) {
    // generate prices
    vector<int64_t> prices;
    for (int64_t value : values) {
        prices.push_back(value % 10);
    }

    // generate changes
    vector<int64_t> changes;
    for (size_t i = 1; i < prices.size(); i++) {
        changes.push_back(prices.at(i) - prices.at(i - 1));
    }

    // setup for iteration
    unordered_set<size_t> visited;

    // iterate over the values
    for (size_t i = 3; i < changes.size(); i++) {
        // get the deltas
        int64_t d1 = changes.at(i - 3);
        int64_t d2 = changes.at(i - 2);
        int64_t d3 = changes.at(i - 1);
        int64_t d4 = changes.at(i + 0);

        // get the index
        size_t index = asIndex(d1, d2, d3, d4);

        if (visited.count(index) == 0) {
            // increment the banana count
            bananas.at(index) += prices.at(i + 1);

            // mark as visited
            visited.insert(index);
        }
    }
}

int main() {
    // temporary storage
    string line;

    // store the secrets
    vector<int64_t> secrets;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        int64_t secret = stoll(line);
        secrets.push_back(secret);
    }

    // setup for iteration
    vector<int64_t> bananas(MAGIC_NUMBER, 0);
    int64_t sumOfValues = 0;

    // iterate over the secrets
    for (int64_t secret : secrets) {
        // get the values
        vector<int64_t> values = getValues(secret);
        sumOfValues += values.back();

        // process values into bananas
        processValues(values, bananas);
    }

    // display progress
    cout << "[first] sum of last values: " << sumOfValues << endl;

    // check for the most bananas
    int64_t maxBananas = 0;
    for (int64_t banana : bananas) {
        maxBananas = max(maxBananas, banana);
    }

    // display result
    cout << "[second] the most bananas available: " << maxBananas << endl;

    return 0;
}
