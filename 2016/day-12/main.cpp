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

void runProgram(Computer& computer,
                Program& program) {
    // reset cursor
    program.cursor = 0;

    while (program.inBounds()) {
        // execute command
        program.doCommand(computer, program);
    }
}

int main() {
    // temporary storage
    string line;

    // setup computer
    Computer firstComputer;
    Computer secondComputer;
    secondComputer.registers['c'] = 1;
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

        // push instruction
        program.instructions.push_back(make_tuple(command, arg1, arg2));

        // create command
        if (command == "cpy") {
            program.commands.push_back(createCPY(arg1, arg2));
        } else if (command == "inc") {
            program.commands.push_back(createINC(arg1));
        } else if (command == "dec") {
            program.commands.push_back(createDEC(arg1));
        } else if (command == "jnz") {
            program.commands.push_back(createJNZ(arg1, arg2));
        } else {
            program.commands.push_back(DEFAULT_COMMAND);
        }
    }

    // run the first program
    runProgram(firstComputer, program);
    cout << "[first] a = " << firstComputer.registers.at('a') << endl;

    // run the second program
    runProgram(secondComputer, program);
    cout << "[second] a = " << secondComputer.registers.at('a') << endl;

    return 0;
}
