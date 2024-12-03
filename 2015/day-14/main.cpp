#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

class Reindeer {
public:
    // basic attributes
    string name;

    // basis statistics
    int64_t flightSpeed;
    int64_t flightDuration;
    int64_t restDuration;

    Reindeer(const string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // parse name
        getline(buffer, name, ' ');

        // skip next two words
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse flight speed
        getline(buffer, tmp, ' ');
        flightSpeed = stoll(tmp);

        // skip next two words
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse flight duration
        getline(buffer, tmp, ' ');
        flightDuration = stoll(tmp);

        // skip next six words
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse rest duration
        getline(buffer, tmp, ' ');
        restDuration = stoll(tmp);
    }

    // utility functions
    int64_t calculateDistance(int64_t duration) const {
        // temporary storage
        int64_t cycleDuration = (flightDuration + restDuration);

        // calculate flight characteristics
        int64_t fullCycles = (duration / cycleDuration);
        int64_t partialDuration = (duration % cycleDuration);

        // calculate total flight duration
        int64_t totalFlightDuration = (flightDuration * fullCycles);
        totalFlightDuration += min(flightDuration, partialDuration);

        return (flightSpeed * totalFlightDuration);
    }
};

int main() {
    // temporary storage
    const int64_t RACE_DURATION = 2503;
    string line, tmp;

    // list of reindeer
    vector<Reindeer> reindeers;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create reindeer
        Reindeer tmp(line);

        // add reindeer to list
        reindeers.push_back(tmp);
    }

    // setup for first iteration
    int64_t firstBest = numeric_limits<int64_t>::min();

    // iterate over reindeer
    for (const Reindeer& reindeer : reindeers) {
        // calculate reindeer stuff
        int64_t distance = reindeer.calculateDistance(RACE_DURATION);

        if (firstBest < distance) {
            firstBest = distance;
        }
    }

    // setup for second iteration
    vector<int64_t> secondPoints(reindeers.size(), 0);

    // iterate over the time slots
    for (int64_t d = 1; d <= RACE_DURATION; d++) {
        // temporary storage
        size_t bestIndex = 0;
        int64_t bestDistance = numeric_limits<int64_t>::min();

        // iterate over reindeers
        for (size_t i = 0; i < reindeers.size(); i++) {
            // temporary storage
            int64_t distance = reindeers.at(i).calculateDistance(d);

            if (bestDistance < distance) {
                bestDistance = distance;
                bestIndex = i;
            }
        }

        // add point to the best
        secondPoints.at(bestIndex)++;
    }

    // display results
    cout << "[first] best distance travelled : " << firstBest << endl;
    cout << "[second] most points acquired : " << *max_element(secondPoints.begin(), secondPoints.end()) << endl;

    return 0;
}
