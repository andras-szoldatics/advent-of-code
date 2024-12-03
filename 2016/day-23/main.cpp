#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

using namespace std;

class Computer;

class Program;

typedef tuple<string, string, string> Instruction;
typedef vector<Instruction> Instructions;

typedef function<void (Computer&, Program&)> Command;
typedef vector<Command> Commands;

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

    Command createCommand(const Instruction& instruction);
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

Command createTGL(const string& argument1) {
    char reg = getRegister(argument1);

    if (reg != '\0') {
        return [reg] (Computer& computer,
                      Program& program) {
                   int64_t offset = computer.registers[reg];

                   // shift cursor
                   program.cursor += offset;

                   if (program.inBounds()) {
                       // retrieve original instruction
                       Instruction& instruction = program.instructions[program.cursor];

                       if (get<2>(instruction).empty()) {
                           if (get<0>(instruction) == "inc") {
                               get<0>(instruction) = "dec";
                           } else {
                               get<0>(instruction) = "inc";
                           }
                       } else {
                           if (get<0>(instruction) == "jnz") {
                               get<0>(instruction) = "cpy";
                           } else {
                               get<0>(instruction) = "jnz";
                           }
                       }

                       // rewrite command from instruction
                       program.commands[program.cursor] = computer.createCommand(instruction);
                   }

                   // restore cursor
                   program.cursor -= offset;

                   // command executed
                   program.cursor++;
        };
    } else {
        int64_t offset = stoll(argument1);

        return [offset] (Computer& computer,
                         Program& program) {
                   // shift cursor
                   program.cursor += offset;

                   if (program.inBounds()) {
                       // retrieve original instruction
                       Instruction& instruction = program.instructions[program.cursor];

                       if (get<2>(instruction).empty()) {
                           if (get<0>(instruction) == "inc") {
                               get<0>(instruction) = "dec";
                           } else {
                               get<0>(instruction) = "inc";
                           }
                       } else {
                           if (get<0>(instruction) == "jnz") {
                               get<0>(instruction) = "cpy";
                           } else {
                               get<0>(instruction) = "jnz";
                           }
                       }

                       // rewrite command from instruction
                       program.commands[program.cursor] = computer.createCommand(instruction);
                   }

                   // restore cursor
                   program.cursor -= offset;

                   // command executed
                   program.cursor++;
        };
    }

    return DEFAULT_COMMAND;
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

Command Computer::createCommand(const Instruction& instruction) {
    // retrieve short-hands
    const string& command = get<0>(instruction);
    const string& arg1 = get<1>(instruction);
    const string& arg2 = get<2>(instruction);

    if (command == "cpy") {
        return createCPY(arg1, arg2);
    } else if (command == "inc") {
        return createINC(arg1);
    } else if (command == "dec") {
        return createDEC(arg1);
    } else if (command == "jnz") {
        return createJNZ(arg1, arg2);
    } else if (command == "tgl") {
        return createTGL(arg1);
    }

    return DEFAULT_COMMAND;
}

int main() {
    // temporary storage
    string line;

    // setup computer
    Computer firstComputer;
    firstComputer.registers['a'] = 7;
    Computer secondComputer;
    secondComputer.registers['a'] = 12;
    Program firstProgram;
    Program secondProgram;

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
        Instruction i = make_tuple(command, arg1, arg2);
        firstProgram.instructions.push_back(i);
        secondProgram.instructions.push_back(i);

        // create command
        Command c = firstComputer.createCommand(i);
        firstProgram.commands.push_back(c);
        secondProgram.commands.push_back(c);
    }

    // run the first program
    runProgram(firstComputer, firstProgram);
    cout << "[first] a = " << firstComputer.registers.at('a') << endl;

    // run the second program
    runProgram(secondComputer, secondProgram);
    cout << "[second] a = " << secondComputer.registers.at('a') << endl;

    return 0;
}
