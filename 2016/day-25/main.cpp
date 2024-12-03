#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

using namespace std;

class Computer {
public:
    // registers
    unordered_map<char, int64_t> registers;

    // signal buffer
    vector<int64_t> buffer;

    Computer() {
        registers = {
            { 'a', 0 },
            { 'b', 0 },
            { 'c', 0 },
            { 'd', 0 }
        };
    }
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

class Program;

typedef tuple<string, string, string> Instruction;
typedef vector<Instruction> Instructions;

typedef function<void (Computer&, Program&)> Command;
typedef vector<Command> Commands;

class Program {
public:
    // instruction storage
    Instructions instructions;
    Commands commands;

    // program state
    int64_t cursor;

    Program() :
        cursor(0) {}

    // utility functions
    bool inBounds() const {
        return (cursor >= 0) && (cursor < static_cast<int64_t>(commands.size()));
    }

    void doCommand(Computer& computer,
                   Program& program) {
        // execute command
        commands[cursor](computer, program);
    }
};

const Command DEFAULT_COMMAND = [] (Computer&,
                                    Program& program) {
        program.cursor++;
    };

Command createCPY(const string& argument1,
                  const string& argument2) {
    char regF = getRegister(argument1);
    char regT = getRegister(argument2);

    if (regT != '\0') {
        if (regF != '\0') {
            return [regT, regF] (Computer& computer,
                                 Program& program) {
                       computer.registers[regT] = computer.registers[regF];
                       program.cursor++;
            };
        } else {
            int64_t value = stoll(argument1);
            return [regT, value] (Computer& computer,
                                  Program& program) {
                       computer.registers[regT] = value;
                       program.cursor++;
            };
        }
    }

    return DEFAULT_COMMAND;
}

Command createINC(const string& argument) {
    char reg = getRegister(argument);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      Program& program) {
                   computer.registers[reg]++;
                   program.cursor++;
        };
    }

    return DEFAULT_COMMAND;
}

Command createDEC(const string& argument) {
    char reg = getRegister(argument);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      Program& program) {
                   computer.registers[reg]--;
                   program.cursor++;
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
                                 Program& program) {
                       if (computer.registers[regV] != 0) {
                           program.cursor += computer.registers[regO];
                       } else {
                           program.cursor++;
                       }
            };
        } else {
            int64_t value = stoll(argument1);
            return [value, regO] (Computer& computer,
                                  Program& program) {
                       if (value != 0) {
                           program.cursor += computer.registers[regO];
                       } else {
                           program.cursor++;
                       }
            };
        }
    } else {
        int64_t offset = stoll(argument2);

        if (regV != '\0') {
            return [regV, offset] (Computer& computer,
                                   Program& program) {
                       if (computer.registers[regV] != 0) {
                           program.cursor += offset;
                       } else {
                           program.cursor++;
                       }
            };
        } else {
            int64_t value = stoll(argument1);
            return [value, offset] (Computer&,
                                    Program& program) {
                       if (value != 0) {
                           program.cursor += offset;
                       } else {
                           program.cursor++;
                       }
            };
        }
    }
}

Command createOUT(const string& argument1) {
    char reg = getRegister(argument1);
    if (reg != '\0') {
        return [reg] (Computer& computer,
                      Program& program) {
                   computer.buffer.push_back(computer.registers[reg]);
                   program.cursor++;
        };
    }

    int64_t value = stoll(argument1);
    return [value] (Computer& computer,
                    Program& program) {
               computer.buffer.push_back(value);
               program.cursor++;
    };
}

bool runProgram(Computer& computer,
                Program& program) {
    // reset cursor
    program.cursor = 0;

    // reset signal check
    size_t lastSize = 0;

    while (program.inBounds()) {
        // execute command
        program.doCommand(computer, program);

        if (computer.buffer.size() != lastSize) {
            // check for signal values
            if (computer.buffer.size() >= 2) {
                // check if last two signals are not 0 and 1 in some order
                if (computer.buffer[computer.buffer.size() - 1] == 0) {
                    if (computer.buffer[computer.buffer.size() - 2] != 1) {
                        return false;
                    }
                } else if (computer.buffer[computer.buffer.size() - 1] == 1) {
                    if (computer.buffer[computer.buffer.size() - 2] != 0) {
                        return false;
                    }
                } else {
                    return false;
                }

                // check for a suitable long time, like 64 signals
                if (computer.buffer.size() >= 64) {
                    break;
                }
            }

            lastSize = computer.buffer.size();
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
            program.commands.push_back(createCPY(arg1, arg2));
        } else if (command == "inc") {
            program.commands.push_back(createINC(arg1));
        } else if (command == "dec") {
            program.commands.push_back(createDEC(arg1));
        } else if (command == "jnz") {
            program.commands.push_back(createJNZ(arg1, arg2));
        } else if (command == "out") {
            program.commands.push_back(createOUT(arg1));
        } else {
            program.commands.push_back(DEFAULT_COMMAND);
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
