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

char getRegister(const string& argument) {
    if (argument.length() == 1) {
        char reg = argument.front();
        if ((reg >= 'a') && (reg <= 'd')) {
            return reg;
        }
    }

    return '\0';
}

Command createCPY(const string& argument1,
                  const string& argument2) {
    char regF = getRegister(argument1);
    char regT = getRegister(argument2);

    if (regT != '\0') {
        if (regF != '\0') {
            return [regT, regF] (Computer& computer,
                                 int64_t& cursor) {
                       computer.registers[regT] = computer.registers[regF];
                       cursor++;
            };
        } else {
            int64_t value = stoll(argument1);
            return [regT, value] (Computer& computer,
                                  int64_t& cursor) {
                       computer.registers[regT] = value;
                       cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

Command createINC(const string& argument) {
    char reg = getRegister(argument);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      int64_t& cursor) {
                   computer.registers[reg]++;
                   cursor++;
        };
    }

    return DEFAULT_COMMAND;
}

Command createDEC(const string& argument) {
    char reg = getRegister(argument);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      int64_t& cursor) {
                   computer.registers[reg]--;
                   cursor++;
        };
    }

    return DEFAULT_COMMAND;
}

Command createJNZ(const string& argument1,
                  const string& argument2) {
    char regV = getRegister(argument1);
    char regO = getRegister(argument2);

    if (regO != '\0') {
        if (regV != '\0') {
            return [regV, regO] (Computer& computer,
                                 int64_t& cursor) {
                       if (computer.registers[regV] != 0) {
                           cursor += computer.registers[regO];
                       } else {
                           cursor++;
                       }
            };
        } else {
            int64_t value = stoll(argument1);
            return [value, regO] (Computer& computer,
                                  int64_t& cursor) {
                       if (value != 0) {
                           cursor += computer.registers[regO];
                       } else {
                           cursor++;
                       }
            };
        }
    } else {
        int64_t offset = stoll(argument2);

        if (regV != '\0') {
            return [regV, offset] (Computer& computer,
                                   int64_t& cursor) {
                       if (computer.registers[regV] != 0) {
                           cursor += offset;
                       } else {
                           cursor++;
                       }
            };
        } else {
            int64_t value = stoll(argument1);
            return [value, offset] (Computer&,
                                    int64_t& cursor) {
                       if (value != 0) {
                           cursor += offset;
                       } else {
                           cursor++;
                       }
            };
        }
    }
}

Command createOUT(const string& argument1) {
    char reg = getRegister(argument1);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      int64_t& cursor) {
                   computer.signals.push_back(computer.registers[reg]);
                   cursor++;
        };
    }

    int64_t value = stoll(argument1);
    return [value] (Computer& computer,
                    int64_t& cursor) {
               computer.signals.push_back(value);
               cursor++;
    };
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
