#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

typedef function<uint16_t ()> Signal;

class Wire {
public:
    // simple attributes
    string id;
    Signal signal;

    // cached signal
    mutable optional<uint16_t> cached;

    // signal functions
    uint16_t getSignal() const {
        // simple caching
        if (!cached) {
            cached = signal();
        }

        return *cached;
    }

    // parsing function
    void parse(const unordered_map<string, Wire>& wires,
               const string& line) {
        // assign line into buffer
        stringstream buffer(line);

        // retrieve first element
        string firstItem;
        getline(buffer, firstItem, ' ');

        // retrieve second element
        string secondItem;
        getline(buffer, secondItem, ' ');

        if (secondItem == string("->")) {
            // this a simple signal wire, get id
            getline(buffer, id);

            // check first item for digits
            bool digitsOnly = true;
            for (char c : firstItem) {
                if (!isdigit(c)) {
                    digitsOnly = false;
                }
            }

            if (digitsOnly) {
                // retrieve single number
                int64_t number = stoll(firstItem);

                // assign a simple lambda
                signal = [number] () -> uint16_t {
                        return number;
                    };
            } else {
                // assign copy lambda
                signal = [&wires, firstItem] () -> uint16_t {
                        return wires.at(firstItem).getSignal();
                    };
            }
        }

        // retrieve third elemnt
        string thirdItem;
        getline(buffer, thirdItem, ' ');

        if ((thirdItem == string("->")) &&
            (firstItem == string("NOT"))) {
            // this is a NOT wire, get id
            getline(buffer, id);

            // assign NOT lambda
            signal = [&wires, secondItem] () -> uint16_t {
                    return ~wires.at(secondItem).getSignal();
                };
        }

        // retrieve fourth element
        string fourthItem;
        getline(buffer, fourthItem, ' ');

        if (fourthItem == string("->")) {
            // this is a two part wire, get id
            getline(buffer, id);

            // separate operation type
            if (secondItem == string("AND")) {
                if (firstItem == string("1")) {
                    // assign special AND lambda
                    signal = [&wires, thirdItem] () -> uint16_t {
                            return (0x01 &
                                    wires.at(thirdItem).getSignal());
                        };
                } else {
                    // assign AND lambda
                    signal = [&wires, firstItem, thirdItem] () -> uint16_t {
                            return (wires.at(firstItem).getSignal() &
                                    wires.at(thirdItem).getSignal());
                        };
                }
            } else if (secondItem == string("OR")) {
                // assign OR lambda
                signal = [&wires, firstItem, thirdItem] () -> uint16_t {
                        return (wires.at(firstItem).getSignal() |
                                wires.at(thirdItem).getSignal());
                    };
            } else {
                // convert third item into number
                int64_t number = stoll(thirdItem);

                if (secondItem == string("LSHIFT")) {
                    // assign LSHIFT lambda
                    signal = [&wires, firstItem, number] () -> uint16_t {
                            return (wires.at(firstItem).getSignal() << number);
                        };
                } else if (secondItem == string("RSHIFT")) {
                    // assign RSHIFT lambda
                    signal = [&wires, firstItem, number] () -> uint16_t {
                            return (wires.at(firstItem).getSignal() >> number);
                        };
                }
            }
        }
    }
};

int main() {
    // temporary storage
    string line;

    // stored monkeys
    unordered_map<string, Wire> wires;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line into monkey
        Wire wire;
        wire.parse(wires, line);

        if (!wire.id.empty()) {
            // add monkey to monkeys
            wires[wire.id] = wire;
        }
    }

    // display first result
    uint16_t firstSignal = wires.at("a").getSignal();
    cout << "[first] signal of wire \"a\" : " << firstSignal << endl;

    // assemble manual override
    string newLine = to_string(firstSignal);
    newLine += string(" -> b");

    Wire newWire;
    newWire.parse(wires, newLine);

    // assign override
    wires[newWire.id] = newWire;

    // clear cache
    for (const pair<const string, Wire>& entry : wires) {
        // clear cache for second phase
        entry.second.cached.reset();
    }

    // display second result
    uint16_t secondSignal = wires.at("a").getSignal();
    cout << "[second] signal of wire \"a\" : " << secondSignal << endl;

    return 0;
}
