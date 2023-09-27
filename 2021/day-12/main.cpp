#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

enum class Type {
    START,
    END,
    BIG,
    SMALL
};

class Cave {
public:
    // attributes
    string id;
    Type type;

    // graph edges
    vector<string> neighbors;

    Cave(const string& _id) :
        id(_id) {
        // determine type
        if (id == "start") {
            type = Type::START;
        } else if (id == "end") {
            type = Type::END;
        } else if ((id[0] >= 'A') && (id[0] <= 'Z')) {
            type = Type::BIG;
        } else {
            type = Type::SMALL;
        }
    }
};

typedef unordered_map<string, Cave> Caves;

class Step {
public:
    string location;

    vector<string> history;
    unordered_set<string> visited;
    bool revisitAvailable;

    Step(const string& _location,
         const vector<string>& _history = {},
         const unordered_set<string>& _visited = {}) :
        location(_location),
        history(_history),
        visited(_visited),
        revisitAvailable(true) {
        // add location to stuff
        history.push_back(location);
        visited.insert(location);
    }

    // utility functions
    vector<Step> getNextSteps(const Caves& caves,
                              bool doubleVisit) const {
        // temporary storage
        vector<Step> result;

        // filter out visited locations
        for (const string& location : caves.at(location).neighbors) {
            // retrieve short-hands
            const Cave& cave = caves.at(location);

            if ((visited.count(location) == 0) ||
                (cave.type == Type::BIG) ||
                (cave.type == Type::END)) {
                // create new step
                result.emplace_back(location, history, visited);

                // copy revisit availability
                result.back().revisitAvailable = revisitAvailable;
            } else if (doubleVisit) {
                // check for special location
                if ((cave.type == Type::SMALL) &&
                    (visited.count(location) != 0) &&
                    (revisitAvailable)) {
                    // allow double visit on a single location
                    result.emplace_back(location, history, visited);

                    // revisit is no longer available
                    result.back().revisitAvailable = false;
                }
            }
        }

        return result;
    }
};

int64_t findAllPaths(const Caves& caves,
                     bool doubleVisit) {
    // temporary storage
    int64_t result = 0;

    // setup queue for iteration
    queue<Step> queue;
    queue.emplace("start");

    while (!queue.empty()) {
        // get next step in queue
        Step step = queue.front();
        queue.pop();

        // check if step reached the end
        if (step.location == "end") {
            result += 1;
            continue;
        }

        // generate next steps
        vector<Step> nextStep = step.getNextSteps(caves, doubleVisit);

        // add next steps to queue
        for (const Step& next : nextStep) {
            queue.push(next);
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // cave graph
    Caves caves;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // parse line
        string firstId;
        getline(buffer, firstId, '-');

        string secondId;
        getline(buffer, secondId, '-');

        // add caves to graph, if missing
        if (caves.count(firstId) == 0) {
            caves.emplace(firstId, firstId);
        }

        if (caves.count(secondId) == 0) {
            caves.emplace(secondId, secondId);
        }

        // add connections to graph
        caves.at(firstId).neighbors.push_back(secondId);
        caves.at(secondId).neighbors.push_back(firstId);
    }

    // find all paths
    int64_t firstResult = findAllPaths(caves, false);

    // display results
    cout << "[first] number of paths: " << firstResult << endl;

    // find all paths
    int64_t secondResult = findAllPaths(caves, true);

    // display results
    cout << "[second] number of paths: " << secondResult << endl;

    return 0;
}
