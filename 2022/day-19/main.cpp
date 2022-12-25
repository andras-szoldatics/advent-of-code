// C++ standard includes
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

// utility functions

string filterToNumbers(const string& line) {
    // temporary storage
    string result;

    // iterate over line
    for (char c : line) {
        // filter characters
        if (c == ' ') {
            // filter spacing
            if ((!result.empty()) &&
                (result.back() != ' ')) {
                result.push_back(c);
            }
        } else if (isdigit(c)) {
            // add numbers, as is
            result.push_back(c);
        }
    }

    return result;
}

enum class Types : int8_t {
    ORE = 0,
    CLAY = 1,
    OBSIDIAN = 2,
    GEODE = 3,
    NONE = -127
};

const array<Types, 5> ROBOTS = {
    Types::GEODE,
    Types::OBSIDIAN,
    Types::CLAY,
    Types::ORE,
    Types::NONE
};

class Storage {
public:
    // current values
    unordered_map<Types, int64_t> values;

    Storage() {
        // assign zero to all type
        for (Types robot : ROBOTS) {
            values[robot] = 0;
        }
    }

    // access functions
    int64_t& at(Types type) {
        return values.at(type);
    }
    const int64_t& at(Types type) const {
        return values.at(type);
    }

    // operator functions
    bool operator<=(const Storage& rhs) const {
        // iterate over values
        for (Types robot : ROBOTS) {
            if (values.at(robot) > rhs.values.at(robot)) {
                return false;
            }
        }

        return true;
    }

    bool operator>=(const Storage& rhs) const {
        // iterate over values
        for (Types robot : ROBOTS) {
            if (values.at(robot) < rhs.values.at(robot)) {
                return false;
            }
        }

        return true;
    }

    void operator+=(const Storage& rhs) {
        // iterate over values
        for (Types robot : ROBOTS) {
            values.at(robot) += rhs.values.at(robot);
        }
    }

    void operator-=(const Storage& rhs) {
        // iterate over values
        for (Types robot : ROBOTS) {
            values.at(robot) -= rhs.values.at(robot);
        }
    }
};

class Blueprint {
public:
    // simple attributes
    int64_t id;
    int64_t geodes;

    // blueprints for robots
    unordered_map<Types, Storage> costOfRobot;

    // starting state
    Storage robots;

    Blueprint() :
        id(0),
        geodes(0) {
        // assign no cost to all robots
        for (Types robot : ROBOTS) {
            costOfRobot[robot] = Storage();
        }

        // you start with one ore robot
        robots.values.at(Types::ORE) = 1;
    }

    // parsing functions
    void parse(const string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // read identifier
        getline(buffer, tmp, ' ');
        id = stoll(tmp);

        // read ore robot
        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::ORE).at(Types::ORE) = stoll(tmp);

        // read clay robot
        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::CLAY).at(Types::ORE) = stoll(tmp);

        // read obsidian robot
        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::OBSIDIAN).at(Types::ORE) = stoll(tmp);

        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::OBSIDIAN).at(Types::CLAY) = stoll(tmp);

        // read geode robot
        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::GEODE).at(Types::ORE) = stoll(tmp);

        getline(buffer, tmp, ' ');
        costOfRobot.at(Types::GEODE).at(Types::OBSIDIAN) = stoll(tmp);
    }
};

class State {
public:
    // original blueprint
    shared_ptr<Blueprint> original;

    // current state
    Storage storage;
    Storage robots;

    // manufacturing information
    vector<Types> pastRobots;
    Types nextRobot;

    // time left, until the elephants riot
    int64_t timeLeft;

    State(const shared_ptr<Blueprint>& blueprint,
          int64_t minutes) :
        original(blueprint),
        nextRobot(Types::NONE),
        timeLeft(minutes) {
        // you begin with an ORE robot
        robots.at(Types::ORE) = 1;
    }
};

int64_t evaluate(const Blueprint& blueprint,
                 int64_t minutes) {
    // temporary storage
    vector<int64_t> mostGeodes;
    mostGeodes.resize(minutes + 1, 0);

    // create a shared copy of blueprint
    shared_ptr<Blueprint> original = make_shared<Blueprint>(blueprint);

    // setup for iteration
    list<State> states;
    State start(original, minutes);
    states.push_front(start);

    // execute DFS iteration
    while (!states.empty()) {
        // retrieve next state
        State previous = states.front();
        states.pop_front();

        // make a copy of state, just in case
        State state = previous;

        if (state.nextRobot != Types::NONE) {
            // spend resources to start building next robot
            state.storage -= state.original->costOfRobot.at(state.nextRobot);
        }

        // already built robots work
        state.storage += state.robots;

        // check for cut-off on poor branches
        if (state.storage.at(Types::GEODE) < mostGeodes.at(state.timeLeft)) {
            // stop processing
            continue;
        }

        // check if we have more geodes, than before
        if (mostGeodes.at(state.timeLeft) < state.storage.at(Types::GEODE)) {
            // this is the most geodes, so far
            mostGeodes.at(state.timeLeft) = state.storage.at(Types::GEODE);
        }

        if (state.nextRobot != Types::NONE) {
            // finished building robot
            state.robots.at(state.nextRobot)++;
        }

        // mark as past robot, even the wait
        state.pastRobots.push_back(state.nextRobot);

        // check if we have more time
        if (state.timeLeft <= 1) {
            continue;
        }

        // decide on next robot
        for (const Types& nextRobot : ROBOTS) {
            // check if we have enough on storage
            if (state.original->costOfRobot.at(nextRobot) <= state.storage) {
                // check if we need any more of this robot
                bool wasteOfResources = true;

                if ((nextRobot == Types::NONE) ||
                    (nextRobot == Types::GEODE)) {
                    // you can always wait & mine geodes
                    wasteOfResources = false;
                } else {
                    // iterate over costs
                    for (Types robot : ROBOTS) {
                        // check if we need more of this type
                        if (state.robots.at(nextRobot) < state.original->costOfRobot.at(robot).at(nextRobot)) {
                            // we could use more, in theory
                            wasteOfResources = false;
                        }
                    }
                }

                // check for inefficient construction
                bool missedOpportunity = false;

                // check if we could've built this robot, before
                if (nextRobot != Types::NONE) {
                    if (previous.nextRobot == Types::NONE) {
                        // check if this robot could've been built previously
                        if (previous.storage >= state.original->costOfRobot.at(nextRobot)) {
                            // this was a missed opportunity
                            missedOpportunity = true;
                        }
                    }
                }

                if ((!wasteOfResources) &&
                    (!missedOpportunity)) {
                    // if everything checks out, add new state
                    State nextState = state;

                    // select robot to build
                    nextState.nextRobot = nextRobot;

                    // decrease remaining time
                    nextState.timeLeft--;

                    // add to queue
                    states.push_front(nextState);
                }
            }
        }
    }

    return mostGeodes.at(1);
}

int main() {
    // temporary storage
    string line;

    // stored blueprints
    vector<Blueprint> blueprints;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // filter line into number
        line = filterToNumbers(line);

        // parse line into blueprint
        Blueprint blueprint;
        blueprint.parse(line);

        // evaluate blueprint
        blueprint.geodes = evaluate(blueprint, 24);

        // add blueprint to storage
        blueprints.push_back(blueprint);
    }

    int64_t firstTotalQuality = 0;
    for (const Blueprint& blueprint : blueprints) {
        int64_t quality = (blueprint.id * blueprint.geodes);
        firstTotalQuality += quality;
    }

    // display first result
    cout << "[first] total quality of blueprints : " << firstTotalQuality << endl;

    int64_t secondTotalGeodes = 1;
    // re-evaluate top three blueprints
    if (blueprints.size() >= 1) {
        int64_t geodes = evaluate(blueprints.at(0), 32);
        secondTotalGeodes *= geodes;
    }

    if (blueprints.size() >= 2) {
        int64_t geodes = evaluate(blueprints.at(1), 32);
        secondTotalGeodes *= geodes;
    }

    if (blueprints.size() >= 3) {
        int64_t geodes = evaluate(blueprints.at(2), 32);
        secondTotalGeodes *= geodes;
    }

    // display second result
    cout << "[second] final geode value : " << secondTotalGeodes << endl;

    return 0;
}
