#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

typedef unordered_map<uint64_t, size_t> Stones;

void addToStones(Stones& stones,
                 uint64_t value,
                 uint64_t count) {
    if (stones.count(value) == 0) {
        stones[value] = count;
    } else {
        stones.at(value) += count;
    }
}

uint64_t getLength(uint64_t value) {
    uint64_t length = 0;
    uint64_t factor = 1;

    while (value >= factor) {
        factor *= 10;
        length += 1;
    }

    return length;
}

uint64_t getFactor(uint64_t length) {
    uint64_t factor = 1;

    while (length > 0) {
        factor *= 10;
        length -= 1;
    }

    return factor;
}

size_t countStones(const Stones& stones) {
    size_t count = 0;

    for (const pair<const uint64_t, size_t>& stone : stones) {
        count += stone.second;
    }

    return count;
}

Stones blink(const Stones& stones) {
    // temporary storage
    Stones result;

    // iterate over entries
    for (const pair<const uint64_t, size_t>& stone : stones) {
        // retrieve short-hands
        uint64_t value = stone.first;
        size_t count = stone.second;

        if (value == 0) {
            addToStones(result, 1, count);
        } else {
            // calculate length
            uint64_t length = getLength(value);

            if (length % 2 == 0) {
                // calculate factor
                uint64_t factor = getFactor(length / 2);

                // split value
                uint64_t left = value / factor;
                uint64_t right = value % factor;

                // add to result
                addToStones(result, left, count);
                addToStones(result, right, count);
            } else {
                // add to result
                addToStones(result, value * 2024, count);
            }
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // store initial state
    Stones stones;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line, ' ')) {
        uint64_t value = stoull(line);
        addToStones(stones, value, 1);
    }

    // setup for iteration
    size_t FIRST_BLINK = 25;
    size_t SECOND_BLINK = 75;

    size_t firstCount = 0;
    size_t secondCount = 0;

    for (size_t i = 1; i <= SECOND_BLINK; i++) {
        stones = blink(stones);

        if (i == FIRST_BLINK) {
            firstCount = countStones(stones);
        }

        if (i == SECOND_BLINK) {
            secondCount = countStones(stones);
        }
    }

    // display results
    cout << "[first] # of stones: " << firstCount << endl;
    cout << "[second] # of stones: " << secondCount << endl;

    return 0;
}
