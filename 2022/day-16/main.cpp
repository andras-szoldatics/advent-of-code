// C++ standard includes
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

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
        if (!getline(buffer, split, delimiter)) {
            break;
        }

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

class Valve {
public:
    // attributes
    string id;
    int64_t flowRate;

    // actual state
    bool isOpen;

    unordered_set<string> tunnels;

    Valve() :
        flowRate(0),
        isOpen(false) {}
    Valve(const string& _id,
          int64_t _flowRate) :
        id(_id),
        flowRate(_flowRate),
        isOpen(false) {}
};

int64_t search(vector<Valve>& valves,
               const vector<vector<int64_t>>& distances,
               size_t firstIndex,
               int64_t firstTime,
               size_t secondIndex,
               int64_t secondTime) {
    // check for early termination
    if ((firstTime <= 1) &&
        (secondTime <= 1)) {
        return 0;
    }

    // temporary storage
    int64_t bestPressure = 0;

    // select a valve to open at this juncture
    for (size_t i = 0; i < valves.size(); i++) {
        // retrieve short-hands
        Valve& valve = valves.at(i);

        // check valve
        if ((valve.isOpen) ||
            (valve.flowRate <= 0)) {
            continue;
        }

        // open valve
        valve.isOpen = true;

        if (firstTime >= secondTime) {
            int64_t restOfTime = (firstTime - distances.at(firstIndex).at(i) - 1);
            int64_t pressure = (restOfTime * valve.flowRate) +
                search(valves, distances, i, restOfTime, secondIndex, secondTime);

            if (bestPressure < pressure) {
                bestPressure = pressure;
            }
        }

        if ((firstTime < secondTime) ||
            ((firstTime == secondTime) && (firstIndex != secondIndex))) {
            int64_t restOfTime = (secondTime - distances.at(secondIndex).at(i) - 1);
            int64_t pressure = (restOfTime * valve.flowRate) +
                search(valves, distances, firstIndex, firstTime, i, restOfTime);

            if (bestPressure < pressure) {
                bestPressure = pressure;
            }
        }

        // close valve
        valve.isOpen = false;
    }

    return bestPressure;
}

int main() {
    // temporary storage
    string line;

    // map of valves
    vector<Valve> valves;
    unordered_map<string, size_t> lookup;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        vector<char> delimiters = { ' ', ' ', '=', ';', '\0' };
        vector<string> parts = splitLine(line, delimiters);

        // parse relevant entries
        string id = parts.at(1);
        int64_t flowRate = stoll(parts.at(3));

        // assemble valve entry
        Valve valve(id, flowRate);

        char last = '\0';
        // iterate over tunnel description
        for (char current : parts.at(4)) {
            // check for a double character
            if ((isupper(last)) &&
                (isupper(current))) {
                // add as tunnel connection
                string tmp = { last, current };
                valve.tunnels.insert(tmp);
            }

            // shift last
            last = current;
        }

        // assign valve to id
        lookup[id] = valves.size();

        // add valve to vector
        valves.push_back(valve);
    }

    if (valves.empty()) {
        return 1;
    }

    // initialize distance matrix
    size_t valveCount = valves.size();
    int64_t TOO_FAR = numeric_limits<int64_t>::max();
    vector<int64_t> tooFarRow = vector<int64_t>(valveCount, TOO_FAR);
    vector<vector<int64_t>> distances(valveCount, tooFarRow);

    // initialize known values
    for (size_t f = 0; f < valveCount; f++) {
        for (size_t t = 0; t < valveCount; t++) {
            // easiest is same place
            if (f == t) {
                distances.at(f).at(t) = 0;
            } else {
                // retrieve short-hands
                const string& toId = valves.at(t).id;

                // check if valves are connected
                if (valves.at(f).tunnels.count(toId) != 0) {
                    distances.at(f).at(t) = 1;
                }
            }
        }
    }

    // run Floyd-Warshall algorithm
    for (size_t k = 0; k < valveCount; k++) {
        // Consider all pairs of vertices
        for (size_t i = 0; i < valveCount; i++) {
            for (size_t j = 0; j < valveCount; j++) {
                // check if distances can be shortened
                if ((distances[i][k] != TOO_FAR) &&
                    (distances[k][j] != TOO_FAR) &&
                    (distances[i][k] + distances[k][j] < distances[i][j])) {
                    distances[i][j] = distances[i][k] + distances[k][j];
                }
            }
        }
    }

    // lookup AA as index
    int64_t bestPressure = 0;
    size_t startIndex = lookup.at("AA");

    bestPressure = search(valves, distances, startIndex, 30, 0, 0);
    cout << "best pressure by going alone : " << bestPressure << endl;

    bestPressure = search(valves, distances, startIndex, 26, startIndex, 26);
    cout << "best pressure by teaming up : " << bestPressure << endl;

    return 0;
}
