#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

using namespace std;

class State {
public:
    // positions
    string values;

    // lookout
    unordered_map<char, size_t> positions;

    // constructor
    State(const string& initial) :
        values(initial) {
        // initialize positions
        calculateLookup();
    }

    // utility functions
    void calculateLookup() {
        for (size_t i = 0; i < values.size(); ++i) {
            positions[values[i]] = i;
        }
    }
};

typedef function<void (State&)> Operation;
typedef vector<Operation> OperationList;

Operation parseLine(const string& line) {

    if (line.find("swap position") != string::npos) {
        // swap position X with position Y
        size_t x = stoi(line.substr(14, 1));
        size_t y = stoi(line.substr(30, 1));

        // add operation
        return [x, y] (State& state) {
                   // retrieve short-hands
                   char& cX = state.values.at(x);
                   char& cY = state.values.at(y);

                   // do swap
                   swap(cX, cY);

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("swap letter") != string::npos) {
        // swap letter X with letter Y
        char x = line[12];
        char y = line[26];

        // add operation
        return [x, y] (State& state) {
                   // retrieve short-hands
                   size_t pX = state.positions.at(x);
                   size_t pY = state.positions.at(y);

                   char& cX = state.values.at(pX);
                   char& cY = state.values.at(pY);

                   // do swap
                   swap(cX, cY);

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("rotate left") != string::npos) {
        // rotate left X steps
        size_t x = stoi(line.substr(12, 1));

        // add operation
        return [x] (State& state) {
                   // select moved part
                   string temp = state.values.substr(0, x);

                   // make the actual move
                   state.values.erase(0, x);
                   state.values += temp;

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("rotate right") != string::npos) {
        // rotate right X steps
        size_t x = stoi(line.substr(13, 1));

        // add operation
        return [x] (State& state) {
                   // select moved part
                   string temp = state.values.substr(state.values.size() - x, x);

                   // make the actual move
                   state.values.erase(state.values.size() - x, x);
                   state.values = temp + state.values;

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("rotate based") != string::npos) {
        // rotate based on position of letter X
        char x = line[35];

        // add operation
        return [x] (State& state) {
                   // get position
                   size_t position = state.positions[x];

                   // calculate steps
                   size_t steps = 1 + position;
                   if (position >= 4) {
                       steps += 1;
                   }

                   // account for wrap-around
                   steps %= state.values.size();

                   // select moved part
                   string temp = state.values.substr(state.values.size() - steps, steps);

                   // make the actual move
                   state.values.erase(state.values.size() - steps, steps);
                   state.values = temp + state.values;

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("reverse positions") != string::npos) {
        // reverse positions X through Y
        int32_t x = stoi(line.substr(18, 1));
        int32_t y = stoi(line.substr(28, 1));

        // add operation
        return [x, y] (State& state) {
                   // reverse the relevant part
                   reverse(state.values.begin() + x, state.values.begin() + y + 1);

                   // update lookup
                   state.calculateLookup();
        };
    } else if (line.find("move position") != string::npos) {
        // move position X to position Y
        size_t x = stoi(line.substr(14, 1));
        size_t y = stoi(line.substr(28, 1));

        // add operation
        return [x, y] (State& state) {
                   // get character
                   char c = state.values[x];

                   // make the actual move
                   state.values.erase(x, 1);
                   state.values.insert(y, 1, c);

                   // update lookup
                   state.calculateLookup();
        };
    }

    // no operation
    return [] (State&) {};
}

void run(State& state,
         const OperationList& operations) {
    for (auto& operation : operations) {
        operation(state);
    }
}

int main() {
    // input string
    string initial("abcdefgh");
    string target("fbgdceah");

    // setup operation list
    OperationList operations;

    // parse input
    string line;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line
        Operation operation = parseLine(line);

        // add operation
        operations.push_back(operation);
    }

    // run operations
    State firstState(initial);
    run(firstState, operations);

    // print result
    cout << "[first] scrambled password: " << firstState.values << endl;

    // setup for iteration
    string from = initial;
    sort(from.begin(), from.end());

    // find all permutations
    while (next_permutation(from.begin(), from.end())) {
        // run operations
        State state(from);
        run(state, operations);

        // get result
        if (state.values == target) {
            // print result
            cout << "[second] un-scrambled password: " << from << endl;
            break;
        }
    }

    return 0;
}
