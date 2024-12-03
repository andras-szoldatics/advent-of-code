#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

int64_t decodeString(const string& s) {
    // temporary storage
    int64_t result = 0;

    // iterate over characters
    for (char c : s) {
        if (c == '\n') {
            continue;
        }

        // convert to int
        unsigned char value = static_cast<unsigned char>(c);

        // update result
        int64_t tmp = static_cast<int64_t>(value);
        result += tmp;
        result *= 17;
        result %= 256;
    }

    return result;
}

enum class Operation {
    UNDEFINED,
    ADD,
    REMOVE
};

class Lens {
public:
    string label;
    int64_t focalLength;

    Lens(const string& l = {}) :
        label(l),
        focalLength(0) {}
};

typedef unordered_map<int64_t, vector<Lens>> LensMap;

class Instruction {
public:
    Lens lens;
    Operation operation;

    Instruction(const string& line) :
        operation(Operation::UNDEFINED) {
        // check last character
        char last = line.back();

        if (last == '-') {
            // extract label
            string tmp = line.substr(0, line.size() - 1);
            lens = Lens(tmp);

            // set operation
            operation = Operation::REMOVE;
        } else {
            // extract label
            string tmp = line.substr(0, line.size() - 2);
            lens = Lens(tmp);

            // extract focal length
            unsigned char value = line.back() - '0';
            lens.focalLength = static_cast<int64_t>(value);

            // set operation
            operation = Operation::ADD;
        }
    }

    // utility functions
    void execute(LensMap& configuration) {
        // retrieve key
        int64_t key = decodeString(lens.label);

        // check operation
        if (operation == Operation::ADD) {
            if (configuration.count(key) == 0) {
                // add lens to map
                configuration[key].push_back(lens);
            } else {
                // retrieve short-hands
                vector<Lens>& lenses = configuration.at(key);

                // setup for iteration
                bool replaced = false;

                // remove lenses with matching label
                for (Lens& entry : lenses) {
                    if (entry.label == lens.label) {
                        entry.focalLength = lens.focalLength;
                        replaced = true;
                        break;
                    }
                }

                if (!replaced) {
                    // add lens to map
                    lenses.push_back(lens);
                }
            }
        } else if (operation == Operation::REMOVE) {
            if (configuration.count(key) != 0) {
                // retrieve short-hands
                vector<Lens>& lenses = configuration.at(key);

                // remove lenses with matching label
                for (vector<Lens>::iterator it = lenses.begin(); it != lenses.end(); ) {
                    if (it->label == lens.label) {
                        it = lenses.erase(it);
                        break;
                    } else {
                        it++;
                    }
                }
            }
        }
    }
};

int64_t evaluateConfiguration(const LensMap& configuration) {
    // temporary storage
    int64_t result = 0;

    // iterate over possible boxes
    for (int64_t box = 0; box < 255; box++) {
        // check if box exists
        if (configuration.count(box) != 0) {
            // retrieve short-hands
            const vector<Lens>& lenses = configuration.at(box);

            // setup for iteration
            int64_t slot = 1;

            // iterate over lenses
            for (const Lens& lens : lenses) {
                // update result
                int64_t tmp = (box + 1);
                tmp *= slot;
                tmp *= lens.focalLength;
                result += tmp;

                // onto the next slot
                slot += 1;
            }
        }
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // setup for iteration
    int64_t firstResult = 0;
    LensMap configuration;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line, ',')) {
        // decode line
        if (line.back() == '\n') {
            line.pop_back();
        }
        int64_t result = decodeString(line);

        // update first result
        firstResult += result;

        // extract instruction
        Instruction instruction(line);
        instruction.execute(configuration);
    }

    // display results
    cout << "[first] sum of results: " << firstResult << endl;

    // evaluate configuration
    int64_t secondResult = evaluateConfiguration(configuration);
    cout << "[second] focusing power: " << secondResult << endl;

    return 0;
}
