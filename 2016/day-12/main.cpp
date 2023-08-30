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
