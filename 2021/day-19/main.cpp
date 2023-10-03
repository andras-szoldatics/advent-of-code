#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<array<int64_t, 3>, 3> Translation;

class Coordinate {
public:
    int64_t x;
    int64_t y;
    int64_t z;

    Coordinate(int64_t _x = 0,
               int64_t _y = 0,
               int64_t _z = 0) :
        x(_x),
        y(_y),
        z(_z) {}

    Coordinate translateWith(const Translation& translation,
                             const Coordinate& other,
                             bool additive) const {
        // create new coordinate
        Coordinate result;

        result.x = x + (additive ? 1 : -1) *
            (other.x * translation[0][0] +
             other.y * translation[0][1] +
             other.z * translation[0][2]);

        result.y = y + (additive ? 1 : -1) *
            (other.x * translation[1][0] +
             other.y * translation[1][1] +
             other.z * translation[1][2]);

        result.z = z + (additive ? 1 : -1) *
            (other.x * translation[2][0] +
             other.y * translation[2][1] +
             other.z * translation[2][2]);

        return result;
    }

    bool operator==(const Coordinate& other) const {
        return (x == other.x) &&
               (y == other.y) &&
               (z == other.z);
    }
};

namespace std {
template<>
struct hash<Coordinate> {
    size_t operator()(const Coordinate& coordinate) const {
        size_t bitfield = hash<int64_t>() (coordinate.x);
        bitfield += hash<int64_t>() (coordinate.y);
        bitfield += hash<int64_t>() (coordinate.z);

        return bitfield;
    }
};

}

class Scanner {
public:
    int64_t id;

    unordered_set<Coordinate> beacons;
    unordered_set<Coordinate> sensors;

    Scanner(int64_t _id) :
        id(_id) {
        // reserve space for beacons
        beacons.reserve(1000);

        // add own location
        sensors.emplace(0, 0, 0);
    }

    // utility functions
    int64_t getLargestManhattan() const {
        int64_t largest = 0;

        for (const Coordinate& from : sensors) {
            for (const Coordinate& to : sensors) {
                // skip same coordinate
                if (from == to) {
                    continue;
                }

                // calculate Manhattan distance
                int64_t distance = abs(max(from.x, to.x) - min(from.x, to.x));
                distance += abs(max(from.y, to.y) - min(from.y, to.y));
                distance += abs(max(from.z, to.z) - min(from.z, to.z));

                // check for largest distance
                if (distance > largest) {
                    largest = distance;
                }
            }
        }

        return largest;
    }
};

vector<Translation> generateTranslations() {
    // temporary storage
    vector<Translation> result;
    result.reserve(48);

    // create all possible translations
    for (size_t x = 1; x <= 6; x++) {
        for (size_t y = 1; y <= 6; y++) {
            // skip overlap
            if (x % 3 == y % 3) {
                continue;
            }

            for (size_t z = 1; z <= 6; z++) {
                // skip overlap
                if ((x % 3 == z % 3) ||
                    (y % 3 == z % 3)) {
                    continue;
                }

                // create translation
                Translation entry = { {
                    { { 0, 0, 0 } },
                    { { 0, 0, 0 } },
                    { { 0, 0, 0 } }
                } };

                entry[0][x % 3] = (x > 3 ? -1 : 1);
                entry[1][y % 3] = (y > 3 ? -1 : 1);
                entry[2][z % 3] = (z > 3 ? -1 : 1);

                // add translation to result
                result.push_back(entry);
            }
        }
    }

    return result;
}

bool checkWithSensor(Scanner& first,
                     Scanner& second,
                     const vector<Translation>& translations) {
    // check for early return
    if (first.beacons.empty() ||
        second.beacons.empty()) {
        return false;
    }

    // create a map for counting
    unordered_map<Coordinate, unordered_map<size_t, int64_t>> mapping;
    bool foundMatch = false;

    // setup for iteration
    size_t translationIndex = 0;
    Coordinate matched;

    // iterate over the pair the first scanners
    for (const Coordinate& first : first.beacons) {
        // iterate over the second scanners
        for (const Coordinate& second : second.beacons) {
            // iterate over all translations
            for (size_t t = 0; t < translations.size(); t++) {
                // translate the second scanner
                const Translation& translation = translations.at(t);
                Coordinate translated = first.translateWith(translation, second, true);

                // count the translated scanner
                if (mapping.count(translated) == 0) {
                    mapping[translated][t] = 1;
                } else {
                    if (mapping.at(translated).count(t) == 0) {
                        mapping[translated][t] = 1;
                    } else {
                        mapping.at(translated).at(t) += 1;

                        // check for early termination
                        if (mapping.at(translated).at(t) >= 12) {
                            // store the conversion state
                            translationIndex = t;
                            matched = translated;

                            foundMatch = true;
                        }
                    }
                }
            }

            if (foundMatch) {
                break;
            }
        }

        if (foundMatch) {
            break;
        }
    }

    // display the best matches
    if (foundMatch) {
        // translate beacons
        for (const Coordinate& beacon : second.beacons) {
            // translate the second scanner's coordinates
            const Translation& translation = translations.at(translationIndex);
            Coordinate translated = matched.translateWith(translation, beacon, false);

            // add converted coordinates to first scanner
            first.beacons.insert(translated);
        }

        // translate sensors
        for (const Coordinate& sensor : second.sensors) {
            // translate the second scanner's coordinates
            const Translation& translation = translations.at(translationIndex);
            Coordinate translated = matched.translateWith(translation, sensor, false);

            // add converted coordinates to first scanner
            first.sensors.insert(translated);
        }

        // remove second scanner
        second.beacons.clear();
        second.sensors.clear();
    }

    return foundMatch;
}

int main() {
    // temporary storage
    string line;

    // storage for scanners
    vector<Scanner> scanners;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        // assign line into buffer
        stringstream buffer(line);
        string tmp;

        // check for new scanner
        if (line.substr(0, 3) == "---") {
            getline(buffer, tmp, ' ');
            getline(buffer, tmp, ' ');
            getline(buffer, tmp, ' ');

            // create new scanner
            int64_t id = stoll(tmp);
            scanners.emplace_back(id);
        } else {
            // get coordinate
            getline(buffer, tmp, ',');
            int64_t x = stoll(tmp);

            getline(buffer, tmp, ',');
            int64_t y = stoll(tmp);

            getline(buffer, tmp, ',');
            int64_t z = stoll(tmp);

            // add coordinate to scanner
            scanners.back().beacons.emplace(x, y, z);
        }
    }

    // setup for iteration
    vector<Translation> translations = generateTranslations();
    bool matchFound = true;

    // display progress
    cout << "this might take a while ..." << endl;

    // iterate over all scanners
    while (matchFound) {
        // reset match flag
        matchFound = false;

        // check all pairs of scanners
        for (size_t f = 0; f < scanners.size(); f++) {
            for (size_t s = 1; s < scanners.size(); s++) {
                // skip same pair
                if (f == s) {
                    continue;
                }

                // check for match
                if (checkWithSensor(scanners.at(f), scanners.at(s), translations)) {
                    // mark match as found
                    matchFound = true;
                }
            }
        }
    }

    // display result
    cout << "[first] # of beacons: " << scanners.front().beacons.size() << endl;
    cout << "[second] largest Manhattan distance: " << scanners.front().getLargestManhattan() << endl;

    return 0;
}
