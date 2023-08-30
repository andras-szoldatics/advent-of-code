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

void runProgram(Computer& computer,
                const Program& program) {
    // set command position
    int64_t cursor = 0;

    while ((cursor >= 0) &&
           (cursor < static_cast<int64_t>(program.size()))) {
        // execute command
        program[cursor](computer, cursor);
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

        // create command
        if (command == "cpy") {
            program.push_back(createCPY(arg1, arg2));
        } else if (command == "inc") {
            program.push_back(createINC(arg1));
        } else if (command == "dec") {
            program.push_back(createDEC(arg1));
        } else if (command == "jnz") {
            program.push_back(createJNZ(arg1, arg2));
        } else {
            program.push_back(DEFAULT_COMMAND);
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
