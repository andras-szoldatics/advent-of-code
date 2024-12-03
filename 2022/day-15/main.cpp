#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

vector<string> splitLine(const string& line,
                         const vector<char>& delimiters,
                         const string& filter = string()) {
    // temporary storage
    vector<string> splits;

    // assign line to buffer
    stringstream buffer(line);

    string split;
    // read splits from buffer
    for (char delimiter : delimiters) {
        // read line using current delimiter
        getline(buffer, split, delimiter);

        // check for filtered value
        if ((split.empty()) ||
            (split == filter)) {
            continue;
        }

        // add split to splits
        splits.push_back(split);
    }

    return splits;
}

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a[0]);
        std::size_t h2 = std::hash<int64_t>() (a[1]);

        return (h1 + h2);
    }
};

class Sensor {
public:
    // given attributes
    Coordinate position;
    Coordinate beacon;

    // calculated attributes
    int64_t range;

    Sensor(Coordinate _position,
           Coordinate _beacon) :
        position(_position),
        beacon(_beacon),
        range(0) {
        // calculate attributes
        calculateRange();
    }

    // utility functions
    vector<Coordinate> getCoverageOnRow(int64_t row) const {
        // temporary storage
        vector<Coordinate> coverage;

        // calculate remaining signal strength
        int64_t remainingRange = (range - abs(position.at(1) - row));
        int64_t startColumn = (position.at(0) - remainingRange);
        int64_t endColumn = (position.at(0) + remainingRange);

        // iterate over defined range
        for (int64_t column = startColumn; column <= endColumn; column++) {
            // assemble coordinate
            Coordinate coordinate = { column, row };

            // add column to coverage
            coverage.push_back(coordinate);
        }

        return coverage;
    }

    // utility functions
    int64_t getDistance(const Coordinate& coordinate) const {
        // reset values
        int64_t range = 0;

        // add delta distances
        range += abs(position.at(0) - coordinate.at(0));
        range += abs(position.at(1) - coordinate.at(1));

        return range;
    }

    vector<Coordinate> getOutsidePoints() const {
        // temporary storage
        vector<Coordinate> outsidePoints;

        // increased maximum range
        int64_t maximumRange = (range + 1);

        // iterate over rows
        for (int64_t relativeRow = -maximumRange; relativeRow <= maximumRange; relativeRow++) {
            // calculate remaining range
            int64_t remainingRange = (maximumRange - abs(relativeRow));
            int64_t startColumn = (position.at(0) - remainingRange);
            int64_t endColumn = (position.at(0) - remainingRange);

            // add start column as entry
            Coordinate tmp = { startColumn, position.at(1) + relativeRow };
            outsidePoints.push_back(tmp);

            // check defined range
            if (startColumn != endColumn) {
                // add end column, as well
                tmp = { endColumn, position.at(1) + relativeRow };
            }
        }

        return outsidePoints;
    }

private:
    // utility functions
    void calculateRange() {
        range = getDistance(beacon);
    }
};

int main() {
    // temporary storage
    string line;

    // list of sensors
    vector<Sensor> sensors;

    // set of covered coordinates
    unordered_set<Coordinate> coverageOnRow;
    int64_t checkedRow = 2000000;

    // define filter for outside points
    int64_t minimumColumn = 0;
    int64_t maximumColumn = 4000000;

    int64_t minimumRow = 0;
    int64_t maximumRow = 4000000;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        vector<char> delimiters = { '=', ',', '=', ':', '=', ',', '=', '\0' };
        vector<string> parts = splitLine(line, delimiters);

        // temporary storage
        Coordinate position;
        Coordinate beacon;

        // parse coordinates
        position.at(0) = stoll(parts.at(1));
        position.at(1) = stoll(parts.at(3));
        beacon.at(0) = stoll(parts.at(5));
        beacon.at(1) = stoll(parts.at(7));

        // assemble sensor
        Sensor sensor(position, beacon);

        // add sensor to list
        sensors.push_back(sensor);

        // calculate coverage
        vector<Coordinate> currentCoverage = sensor.getCoverageOnRow(checkedRow);

        // iterate over covered coordinates
        for (const Coordinate& coordinate : currentCoverage) {
            coverageOnRow.insert(coordinate);
        }
    }

    // filter out already discovered beacons
    for (const Sensor& sensor : sensors) {
        coverageOnRow.erase(sensor.beacon);
    }

    // display results on selected row
    cout << "covered coordinates on row #" << checkedRow << " : " << coverageOnRow.size() << endl;

    int64_t foundColumn = -1;
    int64_t foundRow = -1;
    // iterate over all sensors
    for (const Sensor& sensor : sensors) {
        // generate outside points
        vector<Coordinate> outsidePoints = sensor.getOutsidePoints();

        // iterate over outside points
        for (const Coordinate& outsidePoint : outsidePoints) {
            // apply coordinate filter
            if ((outsidePoint.at(0) < minimumColumn) ||
                (outsidePoint.at(0) > maximumColumn) ||
                (outsidePoint.at(1) < minimumRow) ||
                (outsidePoint.at(1) > maximumRow)) {
                continue;
            }

            bool foundBeacon = true;
            // iterate over all sensors, check for distance
            for (const Sensor& check : sensors) {
                if (check.getDistance(outsidePoint) <= check.range) {
                    foundBeacon = false;
                    break;
                }
            }

            if (foundBeacon) {
                foundColumn = outsidePoint.at(0);
                foundRow = outsidePoint.at(1);

                // display tuning frequency
                cout << "tuning frequency found : " << (foundColumn * 4000000) + foundRow << endl;

                return 0;
            }
        }
    }

    return 1;
}
