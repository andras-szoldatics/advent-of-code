#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

enum class Pulse {
    NONE,
    LOW,
    HIGH
};

class Signal {
public:
    string sender;
    string receiver;
    Pulse pulse;

    Signal(const string& s,
           const string& r,
           Pulse p) :
        sender(s),
        receiver(r),
        pulse(p) {}
};

enum class Type {
    BROADCAST,
    FLIP_FLOP,
    CONJUNCTION
};

class Module {
public:
    // common attributes
    string id;
    Type type;

    vector<string> outputs;

    // specific states
    // flip-flop
    bool flipFlopState;

    // conjunction
    unordered_set<string> allInputs;
    unordered_set<string> highInputs;

    Module(const string& i,
           Type t) :
        id(i),
        type(t),
        flipFlopState(false) {}

    // help functions
    bool inStartingState() {
        if (type == Type::FLIP_FLOP) {
            return !flipFlopState;
        } else if (type == Type::CONJUNCTION) {
            return highInputs.empty();
        }

        return true;
    }

    // utility functions
    vector<Signal> process(const Signal& signal) {
        // assemble result
        vector<Signal> result;

        // determine pulse type
        Pulse pulse = Pulse::NONE;
        if (type == Type::BROADCAST) {
            // broadcast works on all pulses
            pulse = signal.pulse;
        } else if (type == Type::FLIP_FLOP) {
            // flip-flops work on low pulses
            if (signal.pulse == Pulse::LOW) {
                flipFlopState = !flipFlopState;
                pulse = flipFlopState ? Pulse::HIGH : Pulse::LOW;
            }
        } else if (type == Type::CONJUNCTION) {
            // conjunctions work on high pulses
            if (signal.pulse == Pulse::HIGH) {
                // add input to set
                highInputs.insert(signal.sender);
            } else {
                // remove input from set
                highInputs.erase(signal.sender);
            }

            // check if all inputs are high
            pulse = (allInputs.size() == highInputs.size() ? Pulse::LOW : Pulse::HIGH);
        }

        if (pulse != Pulse::NONE) {
            // add output signals
            for (const string& output : outputs) {
                result.emplace_back(id, output, pulse);
            }
        }

        return result;
    }
};

size_t runModules(unordered_map<string, Module>& modules,
                  size_t maxPresses) {
    // temporary storage
    size_t lowPulses = 0;
    size_t highPulses = 0;

    // repeat for button presses
    for (size_t i = 0; i < maxPresses; i++) {
        // temporary storage
        queue<Signal> signals;

        // press the button, LOW to broadcast
        signals.emplace("button", "broadcaster", Pulse::LOW);

        // execute all signals
        while (!signals.empty()) {
            // get signal
            Signal signal = signals.front();
            signals.pop();

            // check for alternative skip
            if ((signal.receiver == string("rx")) &&
                (signal.pulse == Pulse::LOW)) {
                // we finished
                return i + 1;
            }

            // count pulses
            if (signal.pulse == Pulse::LOW) {
                lowPulses++;
            } else {
                highPulses++;
            }

            if (modules.count(signal.receiver) != 0) {
                // get module
                Module& module = modules.at(signal.receiver);

                // process signal
                vector<Signal> newSignals = module.process(signal);
                for (const Signal& newSignal : newSignals) {
                    signals.push(newSignal);
                }
            }
        }
    }

    return lowPulses * highPulses;
}

// this requires a lot of presumptions about the input, but it works

vector<Module> getBinaryCounter(const unordered_map<string, Module>& modules,
                                const string& entry) {
    // temporary storage
    vector<Module> result = { modules.at(entry) };

    // iterate over outputs
    for (const string& output : result.back().outputs) {
        if (modules.count(output) != 0) {
            // retrieve short-hands
            const Module& nextModule = modules.at(output);

            if (nextModule.type != Type::CONJUNCTION) {
                // we have a conjunction
                vector<Module> nextGroup = getBinaryCounter(modules, output);
                result.insert(result.end(), nextGroup.begin(), nextGroup.end());
            }
        }
    }

    return result;
}

size_t buildAsBinary(const unordered_map<string, Module>& modules,
                     const vector<Module>& group) {
    // temporary storage
    size_t result = 0;

    // iterate over group
    for (const Module& module : group) {
        // shift result
        result <<= 1;

        // temporary storage
        bool hasConjunction = false;

        // iterate over outputs
        for (const string& output : module.outputs) {
            if (modules.count(output) != 0) {
                // retrieve short-hands
                const Module& nextModule = modules.at(output);

                if (nextModule.type == Type::CONJUNCTION) {
                    // we have a conjunction
                    hasConjunction = true;
                    break;
                }
            }
        }

        if (hasConjunction) {
            // we have a binary counter
            result += 1;
        }
    }

    // return result
    return result;
}

size_t analyzeModules(const unordered_map<string, Module>& modules) {
    // retrieve short-hands
    const Module& broadcaster = modules.at("broadcaster");

    // setup for iteration
    vector<size_t> pressesTillPulse;
    size_t result = 1;

    for (const string& initial : broadcaster.outputs) {
        // retrieve outputs as group
        vector<Module> group = getBinaryCounter(modules, initial);
        reverse(group.begin(), group.end());
        size_t generateNumber = buildAsBinary(modules, group);

        result = lcm(result, generateNumber);
    }

    return result;
}

int main() {
    // temporary storage line
    string line;

    // store modules
    unordered_map<string, Module> modules;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // temporary storage
        Type type = Type::BROADCAST;
        string id;

        // parse modules
        getline(buffer, id, ' ');
        if (id.front() == '%') {
            type = Type::FLIP_FLOP;
            id = id.substr(1);
        } else if (id.front() == '&') {
            type = Type::CONJUNCTION;
            id = id.substr(1);
        }

        // create module
        Module module(id, type);

        // skip to output lists
        string tmp;
        getline(buffer, tmp, ' ');

        while (getline(buffer, tmp, ',')) {
            // read output
            module.outputs.push_back(tmp);

            // skip space
            getline(buffer, tmp, ' ');
        }

        // add module to map
        modules.emplace(id, module);
    }

    // itereta over modules
    for (const pair<const string, Module>& entry : modules) {
        // iterate over outputs
        for (const string& output : entry.second.outputs) {
            if (modules.count(output) != 0) {
                // add input to output module
                modules.at(output).allInputs.insert(entry.first);
            }
        }
    }

    // run modules for second part
    size_t firstResult = runModules(modules, 1000);

    // display result
    cout << "[first] multiplication number of low and high pulses: " << firstResult << endl;

    // run modules for second part
    size_t secondResult = analyzeModules(modules);

    // display result
    cout << "[second] number of button presses to send final signal: " << secondResult << endl;

    return 0;
}
