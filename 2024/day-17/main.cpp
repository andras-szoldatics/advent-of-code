#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Computer {
public:
    // registers
    uint64_t A;
    uint64_t B;
    uint64_t C;

    // input
    vector<uint64_t> program;
    uint64_t index;

    // output
    vector<uint64_t> output;

    Computer() :
        A(0),
        B(0),
        C(0),
        index(0) {}

    // helper functions
    string getOutput() const {
        string tmp;

        for (uint64_t value : this->output) {
            tmp += to_string(value) + ",";
        }

        tmp.pop_back();
        return tmp;
    }

    uint64_t read() {
        if (index >= program.size()) {
            return 0;
        }

        index += 1;
        return program.at(index - 1);
    }

    uint64_t asCombo(uint64_t operand) const {
        switch (operand) {
        case 4:
            return this->A;
        case 5:
            return this->B;
        case 6:
            return this->C;
        default:
            return operand;
        }
    }

    // utility functions
    void adv(uint64_t value) {
        // perform division
        A /= (1 << asCombo(value));
    }

    void bxl(uint64_t value) {
        // perform bitwise XOR
        B ^= value;
    }

    void bst(uint64_t value) {
        // perform modulo 8 assign
        B = (asCombo(value) % 8);
    }

    void jnz(uint64_t value) {
        // check A register
        if (A != 0) {
            index = value;
        }
    }

    void bxc(uint64_t /*value*/) {
        // perform bitwise XOR
        B ^= C;
    }

    void out(uint64_t value) {
        // output value
        uint64_t o = asCombo(value);
        output.push_back(o % 8);
    }

    void bdv(uint64_t value) {
        // perform division
        B = A / (1 << asCombo(value));
    }

    void cdv(uint64_t value) {
        // perform division
        C = A / (1 << asCombo(value));
    }

    // operation functions
    void runProgram() {
        while (index < program.size()) {
            // retrieve short-hands
            uint64_t opcode = read();
            uint64_t value = read();

            switch (opcode) {
            case 0:
                adv(value);
                break;
            case 1:
                bxl(value);
                break;
            case 2:
                bst(value);
                break;
            case 3:
                jnz(value);
                break;
            case 4:
                bxc(value);
                break;
            case 5:
                out(value);
                break;
            case 6:
                bdv(value);
                break;
            case 7:
                cdv(value);
                break;
            default:
                throw logic_error("invalid opcode");
            }
        }
    }
};

Computer readInput() {
    // temporary storage
    string line;
    Computer c;

    // read input file
    ifstream input("input.txt");

    // registers
    getline(input, line, ':');
    getline(input, line);
    c.A = stoull(line);

    getline(input, line, ':');
    getline(input, line);
    c.B = stoull(line);

    getline(input, line, ':');
    getline(input, line);
    c.C = stoull(line);

    getline(input, line);
    getline(input, line, ':');

    while (getline(input, line, ',')) {
        uint64_t value = stoull(line);
        c.program.push_back(value);
    }

    return c;
}

int main() {
    // store the computer
    Computer backup = readInput();

    // make a copy & run it
    Computer first = backup;
    first.runProgram();

    // display progress
    cout << "[first] output string: " << first.getOutput() << endl;

    uint64_t input = 0;
    size_t limit = backup.program.size();
    // try the safe-cracking approach
    for (size_t i = 0; i < limit; i++) {
        // temporary storage
        bool match = false;

        // shift input to look for the other 'digit'
        input <<= 3;

        // try the next 'digit'
        while (!match) {
            // make a copy & run it
            Computer second = backup;
            second.A = input;
            second.runProgram();

            match = true;
            for (size_t o = 0; o < second.output.size(); o++) {
                // check the new digit
                uint64_t result = second.output.at(o);
                uint64_t value = second.program.at(limit - i - 1 + o);

                if (result != value) {
                    match = false;
                    break;
                }
            }

            if (!match) {
                // shift input
                input += 1;
            }
        }

        if (!match) {
            input = 0;
            break;
        }
    }

    // display progress
    cout << "[second] code to the computer: " << input << endl;

    return 0;
}
