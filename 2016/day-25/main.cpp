#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

using namespace std;

class Computer {
public:
    // registers
    unordered_map<char, int64_t> registers;
    vector<int64_t> signals;

    Computer() {
        registers = {
            { 'a', 0 },
            { 'b', 0 },
            { 'c', 0 },
            { 'd', 0 }
        };
    }
};

typedef function<void (Computer&, int64_t&)> Command;
typedef std::vector<Command> Program;

const Command DEFAULT_COMMAND = [] (Computer&,
                                    int64_t& cursor) {
        cursor++;
    };

Command createCPY(const string& from,
                  const string& to) {
    if (to.length() == 1) {
        char t = to.front();
        if ((t >= 'a') && (t <= 'd')) {
            if (from.length() == 1) {
                char f = from.front();
                if ((f >= 'a') && (f <= 'd')) {
                    return [t, f] (Computer& computer,
                                   int64_t& cursor) {
                               computer.registers[t] = computer.registers[f];
                               cursor++;
                    };
                }
            }

            // from is not a register
            int64_t v = stoll(from);

            return [t, v] (Computer& computer,
                           int64_t& cursor) {
                       computer.registers[t] = v;
                       cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

Command createINC(const string& reg) {
    if (reg.length() == 1) {
        char r = reg.front();
        if ((r >= 'a') && (r <= 'd')) {
            return [r] (Computer& computer,
                        int64_t& cursor) {
                       computer.registers[r]++;
                       cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

Command createDEC(const string& reg) {
    if (reg.length() == 1) {
        char r = reg.front();
        if ((r >= 'a') && (r <= 'd')) {
            return [r] (Computer& computer,
                        int64_t& cursor) {
                       computer.registers[r]--;
                       cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

Command createJNZ(const string& value,
                  const string& offset) {
    // handle edge-cases
    if ((value == string("0")) &&
        (offset == string("0"))) {
        return DEFAULT_COMMAND;
    }

    // convert offset to integer
    int64_t delta = stoll(offset);

    if (value.length() == 1) {
        char r = value.front();
        if ((r >= 'a') && (r <= 'd')) {
            return [r, delta] (Computer& computer,
                               int64_t& cursor) {
                       if (computer.registers[r] != 0) {
                           cursor += delta;
                       } else {
                           cursor++;
                       }
            };
        }
    }

    // value is not a register
    int64_t v = stoll(value);

    return [v, delta] (Computer&,
                       int64_t& cursor) {
               if (v != 0) {
                   cursor += delta;
               } else {
                   cursor++;
               }
    };
}

Command createOUT(const string& reg) {
    if (reg.length() == 1) {
        char r = reg.front();
        if ((r >= 'a') && (r <= 'd')) {
            return [r] (Computer& computer,
                        int64_t& cursor) {
                       computer.signals.push_back(computer.registers[r]);
                       cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

bool runProgram(Computer& computer,
                const Program& program) {
    // set command position
    int64_t cursor = 0;
    size_t lastSize = 0;

    while ((cursor >= 0) &&
           (cursor < static_cast<int64_t>(program.size()))) {
        // execute command
        program[cursor](computer, cursor);

        if (computer.signals.size() != lastSize) {
            // check for signal values
            if (computer.signals.size() >= 2) {
                // check if last two signals are not 0 and 1 in some order
                if (computer.signals[computer.signals.size() - 1] == 0) {
                    if (computer.signals[computer.signals.size() - 2] != 1) {
                        return false;
                    }
                } else if (computer.signals[computer.signals.size() - 1] == 1) {
                    if (computer.signals[computer.signals.size() - 2] != 0) {
                        return false;
                    }
                } else {
                    return false;
                }

                // check for a suitable long time, like 64 signals
                if (computer.signals.size() >= 64) {
                    break;
                }
            }

            lastSize = computer.signals.size();
        }
    }

    return true;
}

int main() {
    // temporary storage
    string line;

    // setup program
    Program program;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // parse command
        string command;
        getline(buffer, command, ' ');

        // parse arguments
        string arg1, arg2;
        getline(buffer, arg1, ' ');
        getline(buffer, arg2, ' ');

        // create command
        if (command == "cpy") {
            program.push_back(createCPY(arg1, arg2));
        } else if (command == "inc") {
            program.push_back(createINC(arg1));
        } else if (command == "dec") {
            program.push_back(createDEC(arg1));
        } else if (command == "jnz") {
            program.push_back(createJNZ(arg1, arg2));
        } else if (command == "out") {
            program.push_back(createOUT(arg1));
        } else {
            program.push_back(DEFAULT_COMMAND);
        }
    }

    int64_t result = -1;
    for (int64_t i = 0; i < 1048576; i++) {
        // setup computer
        Computer computer;
        computer.registers.at('a') = i;

        // run program
        bool success = runProgram(computer, program);
        if (success) {
            result = i;
            break;
        }
    }

    // display result
    cout << "[first] signal gained with 'a' set to : " << result << endl;

    return 0;
}
