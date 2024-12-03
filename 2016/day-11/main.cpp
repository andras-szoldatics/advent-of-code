#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class State {
public:
    // state of elevator
    size_t elevator;

    // state of items
    unordered_map<string, int64_t> ids;
    unordered_map<int64_t, size_t> items;

    // state of steps
    int64_t steps;

    State() :
        elevator(0),
        steps(0) {}

    // helper functions
    int64_t assignId(const string& element) {
        if (ids.count(element) == 0) {
            ids[element] = static_cast<int64_t>(ids.size() + 1);
        }

        return ids.at(element);
    }

    vector<int64_t> getCurrentFloor() {
        return getFloor(elevator);
    }

    vector<int64_t> getFloor(size_t floorNumber) {
        // retrieve all items on current floor
        vector<int64_t> floor;

        for (const pair<const int64_t, size_t>& item : items) {
            if (item.second == floorNumber) {
                floor.push_back(item.first);
            }
        }

        return floor;
    }

    bool isValid() {
        // check if every element has a pair or alone
        for (const pair<const int64_t, size_t>& item : items) {
            // check if element is a microchip
            if (item.first > 0) {
                // check if there is a generator
                if (items.at(-item.first) != item.second) {
                    // check if there are other generators
                    for (const pair<const int64_t, size_t>& other : items) {
                        if ((other.first < 0) &&
                            (other.second == item.second)) {
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }

    bool finishedTask() {
        // every element must be on the fourth floor
        for (const pair<const int64_t, size_t>& entry : items) {
            if (entry.second != 3) {
                return false;
            }
        }

        return true;
    }

    // utility functions

    string asString() {
        // generate semi-unique id
        string id = to_string(elevator);
        id += "#";

        // mapping for elements
        unordered_map<int64_t, int64_t> remap;

        // get each floor as a vector
        for (size_t i = 0; i < 4; i++) {
            // temporary storage
            vector<int64_t> floor = getFloor(i);
            sort(floor.begin(), floor.end());

            vector<int64_t> tmp;

            // iterate over floor
            for (int64_t entry : floor) {
                if (remap.count(entry) == 0) {
                    int64_t id = static_cast<int64_t>(remap.size() / 2 + 1);
                    if (entry < 0) {
                        id = -id;
                    }

                    // add to remap
                    remap[+entry] = +id;
                    remap[-entry] = -id;
                }

                // add to temporary
                tmp.push_back(remap.at(entry));
            }

            // sort temporary
            sort(tmp.begin(), tmp.end());

            // add to string id
            for (int64_t entry : tmp) {
                id += to_string(entry);
                id += " ";
            }

            if (tmp.empty()) {
                id.push_back('#');
            } else {
                id.back() = '#';
            }
        }

        return id;
    }
};

void parseLine(State& state,
               size_t floor,
               const string& line) {
    // look for microchips
    size_t microchipPos = line.find("microchip");
    while (microchipPos != string::npos) {
        // find the previous word
        size_t previousWordPos = line.rfind(' ', microchipPos - 2);
        string previousWord = line.substr(previousWordPos + 1, microchipPos - previousWordPos - 2);

        // trim the compatible part
        size_t compatiblePos = previousWord.find('-');
        string element = previousWord.substr(0, compatiblePos);

        // add microchip
        int64_t id = state.assignId(element);
        state.items[+id] = floor;

        // look for next microchip
        microchipPos = line.find("microchip", microchipPos + 1);
    }

    // look for generators
    size_t generatorPos = line.find("generator");
    while (generatorPos != string::npos) {
        // find the previous word
        size_t previousWordPos = line.rfind(' ', generatorPos - 2);
        string element = line.substr(previousWordPos + 1, generatorPos - previousWordPos - 2);

        // add generator
        int64_t id = state.assignId(element);
        state.items[-id] = floor;

        // look for next generator
        generatorPos = line.find("generator", generatorPos + 1);
    }
}

int64_t findSolution(const State& initialState) {
    // setup for iteration
    queue<State> states;
    states.push(initialState);

    // cache to filter already visited states
    unordered_set<string> visitedStates;

    while (!states.empty()) {
        // retrieve state
        State state = states.front();
        states.pop();

        // check if this state was already visited
        string stateId = state.asString();
        if (visitedStates.count(stateId) == 0) {
            visitedStates.insert(stateId);
        } else {
            continue;
        }

        // check if this is a solution
        if (state.finishedTask()) {
            return state.steps;
        }

        // retrieve short-hands
        vector<int64_t> floor = state.getCurrentFloor();

        // iterate over all entries on floor
        for (size_t first = 0; first < floor.size(); first++) {
            int64_t firstEntry = floor.at(first);

            // iterate over some entries on floor
            for (size_t second = first; second < floor.size(); second++) {
                int64_t secondEntry = floor.at(second);

                // check for movement possibilities
                if (state.elevator > 0) {
                    // try to move down
                    State nextState = state;

                    // move elevator
                    nextState.elevator -= 1;
                    nextState.steps += 1;

                    // move items
                    nextState.items[firstEntry] = nextState.elevator;
                    nextState.items[secondEntry] = nextState.elevator;

                    // check if state is valid
                    if (nextState.isValid()) {
                        // add state to queue
                        states.push(nextState);
                    }
                }

                if (state.elevator < 3) {
                    // try to move up
                    State nextState = state;

                    // move elevator
                    nextState.elevator += 1;
                    nextState.steps += 1;

                    // move items
                    nextState.items[firstEntry] = nextState.elevator;
                    nextState.items[secondEntry] = nextState.elevator;

                    // check if state is valid
                    if (nextState.isValid()) {
                        // add state to queue
                        states.push(nextState);
                    }
                }
            }
        }
    }

    return -1;
}

int main() {
    // temporary storage
    string line;
    size_t floor = 0;

    // storage of factory
    State initialState;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line
        parseLine(initialState, floor, line);

        // shift floor
        floor += 1;
    }

    // find first solution
    int64_t first = findSolution(initialState);
    cout << "[first] # of steps : " << first << endl;

    // add generators & microchips by hand
    initialState.items[+initialState.assignId("elerium")] = 0;
    initialState.items[+initialState.assignId("dilithium")] = 0;
    initialState.items[-initialState.assignId("elerium")] = 0;
    initialState.items[-initialState.assignId("dilithium")] = 0;

    // find second solution
    int64_t second = findSolution(initialState);
    cout << "[second] # of steps: " << second << endl;

    return 0;
}
