#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

typedef function<bool ()> Gate;

class Module {
public:
    string id;
    size_t type;

    optional<string> in1;
    optional<string> in2;

    Module(const string& id,
           bool v) :
        id(id),
        out(v),
        type(0) {}

    Module(const string& id,
           const string& i1,
           const string& i2,
           const Gate& g,
           size_t t) :
        id(id),
        in1(i1),
        in2(i2),
        gate(g),
        type(t) {}

    // utility functions
    bool get() {
        if (!out) {
            out = gate();
        }

        return out.value();
    }

private:
    Gate gate;
    optional<bool> out;
};

uint64_t getNumber(unordered_map<string, Module>& modules) {
    // check for increasing keys
    string key("z00");
    size_t index = 0;
    vector<bool> bits;

    while (modules.count(key) != 0) {
        // get the module
        Module& module = modules.at(key);

        // get the value
        bool value = module.get();
        bits.push_back(value);

        // shift to next index
        index += 1;

        // generate new key
        stringstream buffer;
        buffer << "z" << index / 10 << index % 10;
        key = buffer.str();
    }

    // generate number from bits
    uint64_t number = 0;
    for (size_t i = 0; i < bits.size(); i++) {
        if (bits.at(i)) {
            number |= (1ull << i);
        }
    }

    return number;
}

string findFaultyWires(const unordered_map<string, Module>& modules) {
    // temporary storage
    vector<string> wires;
    string largestZ("z00");

    // generate output connections
    unordered_map<string, vector<string>> outgoing;
    for (const pair<const string, Module>& module : modules) {
        if (module.second.type != 0) {
            outgoing[module.second.in1.value()].push_back(module.first);
            outgoing[module.second.in2.value()].push_back(module.first);
        }

        if (module.first.front() == 'z') {
            if (largestZ < module.first) {
                largestZ = module.first;
            }
        }
    }

    // iterate over all the modules
    for (const pair<const string, Module>& entry : modules) {
        // retrieve short-hands
        const Module& module = entry.second;

        // skip incoming signals
        if (entry.second.type == 0) {
            continue;
        }

        // check each module type according to a presumed schematic
        bool faulty = false;

        // AND gates
        if (module.type == 1) {
            // retrieve short-hands
            const string& input1 = module.in1.value();

            // separate positions
            if ((input1 == string("x00")) ||
                (input1 == string("y00"))) {
                if (outgoing.count(module.id) == 0) {
                    faulty = true;
                } else {
                    // retrieve short-hands
                    vector<string> connections = outgoing.at(module.id);

                    if (connections.size() != 2) {
                        faulty = true;
                    } else {
                        // check types of connections
                        for (const string& connection : connections) {
                            if (modules.at(connection).type == 2) {
                                faulty = true;
                            }
                        }
                    }
                }
            } else {
                if (outgoing.count(module.id) == 0) {
                    faulty = true;
                } else {
                    // retrieve short-hands
                    vector<string> connections = outgoing.at(module.id);

                    if (connections.size() != 1) {
                        faulty = true;
                    } else {
                        // check types of connections
                        for (const string& connection : connections) {
                            if (modules.at(connection).type != 2) {
                                faulty = true;
                            }
                        }
                    }
                }
            }
        }

        // OR gates
        if (module.type == 2) {
            if (outgoing.count(module.id) == 0) {
                if (module.id != largestZ) {
                    faulty = true;
                }
            } else {
                // retrieve short-hands
                vector<string> connections = outgoing.at(module.id);

                if (connections.size() != 2) {
                    faulty = true;
                } else {
                    // check types of connections
                    for (const string& connection : connections) {
                        if (modules.at(connection).type == 2) {
                            faulty = true;
                        }
                    }
                }
            }
        }

        // XOR gates
        if (module.type == 3) {
            // retrieve short-hands
            const string& input1 = module.in1.value();

            if (((input1.front() == 'x') && (input1 != string("x00")) ||
                 ((input1.front() == 'y') && (input1 != string("y00"))))) {
                if (outgoing.count(module.id) == 0) {
                    faulty = true;
                } else {
                    // retrieve short-hands
                    vector<string> connections = outgoing.at(module.id);

                    if (connections.size() != 2) {
                        faulty = true;
                    } else {
                        // check types of connections
                        for (const string& connection : connections) {
                            if (modules.at(connection).type == 2) {
                                faulty = true;
                            }
                        }
                    }
                }
            } else if (module.id.front() != 'z') {
                    faulty = true;
            }
        }

        // add to faulty wires
        if (faulty) {
            wires.push_back(module.id);
        }
    }

    // sort wires
    sort(wires.begin(), wires.end());

    // generate string
    string tmp;
    for (size_t i = 0; i < wires.size(); i++) {
        if (i != 0) {
            tmp += ",";
        }

        tmp += wires.at(i);
    }

    return tmp;
}

int main() {
    // temporary storage
    string line;
    size_t mode = 0;

    // store the parsed modules
    unordered_map<string, Module> modules;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (line.empty()) {
            mode += 1;
            continue;
        }

        // assign line into buffer
        stringstream buffer(line);
        string s;

        if (mode == 0) {
            getline(buffer, s, ':');
            string id = s;

            getline(buffer, s);
            bool value = stoi(s);

            // add simple module
            modules.emplace(id, Module(id, value));
        } else if (mode == 1) {
            getline(buffer, s, ' ');
            string in1 = s;

            getline(buffer, s, ' ');
            string op = s;

            getline(buffer, s, ' ');
            string in2 = s;

            getline(buffer, s, ' ');
            getline(buffer, s);
            string id = s;

            Gate gate;
            size_t type = 0;
            if (op == string("AND")) {
                gate = [&modules, in1, in2] {
                        return modules.at(in1).get() && modules.at(in2).get();
                    };
                type = 1;
            } else if (op == string("OR")) {
                gate = [&modules, in1, in2] {
                        return modules.at(in1).get() || modules.at(in2).get();
                    };
                type = 2;
            } else if (op == string("XOR")) {
                gate = [&modules, in1, in2] {
                        return modules.at(in1).get() ^ modules.at(in2).get();
                    };
                type = 3;
            } else {
                gate = [] {
                        return false;
                    };
            }

            // add complex module
            modules.emplace(id, Module(id, in1, in2, gate, type));
        }
    }

    // display progress
    cout << "[first] final value from bits: " << getNumber(modules) << endl;

    // display progress
    cout << "[second] faulty wires: " << findFaultyWires(modules) << endl;

    return 0;
}
