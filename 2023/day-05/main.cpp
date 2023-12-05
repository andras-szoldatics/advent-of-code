#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

using namespace std;

class Range {
public:
    int64_t from;
    int64_t to;
    int64_t delta;

    Range(int64_t f,
          int64_t t,
          int64_t d) :
        from(f),
        to(t),
        delta(d) {}

    // utility functions
    bool contains(int64_t value) const {
        return (from <= value) && (value <= to);
    }

    bool hasIntersection(const Range& other) const {
        return (from <= other.to) && (other.from <= to);
    }

    Range getIntersection(const Range& other) const {
        // temporary storage
        int64_t f = max(from, other.from);
        int64_t t = min(to, other.to);
        int64_t d = delta + other.delta;

        return Range(f, t, d);
    }

    int64_t mapValue(int64_t value) const {
        return value + delta;
    }
};

class Mapping {
public:
    vector<Range> mappings;

    // utility functions
    int64_t mapValue(int64_t value) const {
        // iterate over ranges
        for (const Range& range : mappings) {
            if (range.contains(value)) {
                return range.mapValue(value);
            }
        }

        return value;
    }

    vector<Range> mapRanges(const vector<Range>& ranges) const {
        // temporary storage
        vector<Range> result;

        // iterate over ranges
        for (const Range& range : ranges) {
            // calculate changes to range
            map<int64_t, int64_t> deltas;
            deltas[range.from] = 0;
            deltas[range.to] = 0;

            // iterate over mappings
            for (const Range& mapping : mappings) {
                // check if mapping affects range
                if (mapping.hasIntersection(range)) {
                    // query intersection
                    Range intersection = mapping.getIntersection(range);

                    // update deltas
                    deltas[intersection.from] = intersection.delta;
                    deltas[intersection.to] = intersection.delta;
                }
            }

            // get the keys of the map as vector
            vector<int64_t> keys;
            keys.reserve(deltas.size());

            for (const pair<const int64_t, int64_t>& pair : deltas) {
                keys.push_back(pair.first);
            }

            // iterate over pairs of keys
            for (size_t k = 0; k < keys.size() - 1; k += 1) {
                // temporary storage
                int64_t from = keys.at(k);
                int64_t to = keys.at(k + 1);

                int64_t deltaAtFrom = deltas.at(from);
                int64_t deltaAtTo = deltas.at(to);

                if (deltaAtFrom != deltaAtTo) {
                    // add new mapping
                    deltas[to - 1] = deltaAtFrom;
                }
            }

            // re-generate keys
            keys.clear();

            for (const pair<const int64_t, int64_t>& pair : deltas) {
                keys.push_back(pair.first);
            }

            // iterate over pairs of keys
            for (size_t k = 0; k < keys.size() - 1; k += 1) {
                // temporary storage
                int64_t from = keys.at(k);
                int64_t to = keys.at(k + 1);

                int64_t deltaAtFrom = deltas.at(from);
                int64_t deltaAtTo = deltas.at(to);

                if (deltaAtFrom == deltaAtTo) {
                    // add new range
                    result.emplace_back(from + deltaAtFrom, to + deltaAtTo, 0);
                }
            }

            // check if last delta is without a pair
            if (deltas.size() % 2 == 1) {
                // temporary storage
                int64_t from = keys.back();
                int64_t delta = deltas.at(from);

                // add new range
                result.emplace_back(from + delta, from + delta, 0);
            }

            if (result.empty()) {
                throw runtime_error("result is empty");
            }
        }

        return result;
    }
};

vector<int64_t> splitAndConvert(const string& group) {
    // temporary storage
    vector<int64_t> result;
    string remainder = group;

    while (!remainder.empty()) {
        // find next space
        size_t pos = remainder.find(' ');
        string part = remainder.substr(0, pos);

        if (pos != string::npos) {
            // remove part from remainder
            remainder = remainder.substr(pos + 1);
        } else {
            // remove part from remainder
            remainder.clear();
        }

        if (part.empty()) {
            continue;
        }

        // try to convert to integer
        try {
            int64_t number = stoll(part);
            result.push_back(number);
        } catch (...) {
            // NO-OP
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // store seed numbers & conversion
    vector<int64_t> seeds;
    vector<Mapping> conversions;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (seeds.empty()) {
            seeds = splitAndConvert(line);
        } else {
            // check if this is an empty line
            if (line.empty()) {
                conversions.emplace_back();
            } else {
                // parse conversion
                vector<int64_t> tmp = splitAndConvert(line);

                if (tmp.size() == 3) {
                    // temporary storage
                    int64_t from = tmp.at(1);
                    int64_t to = from + tmp.at(2) - 1;
                    int64_t delta = tmp.at(0) - from;

                    // add new range
                    conversions.back().mappings.emplace_back(from, to, delta);
                }
            }
        }
    }

    // setup for iteration
    int64_t firstLocation = numeric_limits<int64_t>::max();

    // iterate over seeds
    for (int64_t seed : seeds) {
        // temporary storage
        int64_t value = seed;

        // iterate over conversions
        for (const Mapping& conversion : conversions) {
            value = conversion.mapValue(value);
        }

        // check if this is the lowest location
        if (value < firstLocation) {
            firstLocation = value;
        }
    }

    // display progress
    cout << "[first] lowest location : " << (firstLocation != numeric_limits<int64_t>::max() ? to_string(firstLocation) : "N/A") << endl;

    // setup for iteration
    int64_t secondLocation = numeric_limits<int64_t>::max();

    // iterate over ranges of seeds
    for (size_t i = 0; i < seeds.size(); i += 2) {
        // temporary storage
        int64_t from = seeds.at(i);
        int64_t to = seeds.at(i) + seeds.at(i + 1);

        // setup for iteration
        vector<Range> currentRanges;
        currentRanges.emplace_back(from, to, 0);

        // iterate over conversions
        for (const Mapping& conversion : conversions) {
            // update ranges
            currentRanges = conversion.mapRanges(currentRanges);
        }

        // iterate over ranges
        for (const Range& range : currentRanges) {
            if (range.from < secondLocation) {
                secondLocation = range.from;
            }
        }
    }

    // display progress
    cout << "[second] lowest location : " << (secondLocation != numeric_limits<int64_t>::max() ? to_string(secondLocation) : "N/A") << endl;

    return 0;
}
