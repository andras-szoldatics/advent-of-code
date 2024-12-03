#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

using namespace std;

enum class Type : uint8_t {
    UNDEFINED = 0,
    HALF,
    TRIPLE,
    INCREMENT,
    JUMP,
    JUMP_IF_EVEN,
    JUMP_IF_ONE
};

enum class Address : uint8_t {
    UNDEFINED = 0,
    ALPHA,
    BETA
};

class Instruction {
public:
    // simple attributes
    Type type;
    Address address;
    int64_t offset;

    Instruction(const string& line) :
        type(Type::UNDEFINED),
        address(Address::UNDEFINED),
        offset(0) {
        // temporary storage
        string tmp;

        // assign line to buffer
        stringstream buffer(line);

        // parse type
        getline(buffer, tmp, ' ');
        if (tmp == string("hlf")) {
            type = Type::HALF;
        } else if (tmp == string("tpl")) {
            type = Type::TRIPLE;
        } else if (tmp == string("inc")) {
            type = Type::INCREMENT;
        } else if (tmp == string("jmp")) {
            type = Type::JUMP;
        } else if (tmp == string("jie")) {
            type = Type::JUMP_IF_EVEN;
        } else if (tmp == string("jio")) {
            type = Type::JUMP_IF_ONE;
        }

        if ((type != Type::UNDEFINED) &&
            (type != Type::JUMP)) {
            // parse register
            getline(buffer, tmp, ',');
            if (tmp == string("a")) {
                address = Address::ALPHA;
            } else if (tmp == string("b")) {
                address = Address::BETA;
            }
        }

        if ((type == Type::JUMP) ||
            (type == Type::JUMP_IF_EVEN) ||
            (type == Type::JUMP_IF_ONE)) {
            // parse offset
            getline(buffer, tmp);
            offset = stoll(tmp);
        }
    }
};

class Computer {
public:
    // simple attributes
    shared_ptr<int64_t> valueA;
    shared_ptr<int64_t> valueB;

    // program code
    vector<Instruction> instructions;
    int64_t currentInstruction;

    Computer(int64_t _valueA = 0,
             int64_t _valueB = 0) :
        currentInstruction(0) {
        valueA = make_shared<int64_t>(_valueA);
        valueB = make_shared<int64_t>(_valueB);
    }

    // utility functions
    bool step() {
        // pre-check for instruction pointer
        if (currentInstruction >= static_cast<int64_t>(instructions.size())) {
            return false;
        }

        // retrieve short-hands
        const Instruction& instruction = instructions.at(currentInstruction);
        shared_ptr<int64_t> value = make_shared<int64_t>(0);

        // select value
        switch (instruction.address) {
        case Address::ALPHA:
            value = valueA;
            break;
        case Address::BETA:
            value = valueB;
            break;
        default:
            (void) nullptr;
        }

        // execute instruction
        switch (instruction.type) {
        case Type::HALF:
            (*value) /= 2;
            currentInstruction += 1;
            break;
        case Type::TRIPLE:
            (*value) *= 3;
            currentInstruction += 1;
            break;
        case Type::INCREMENT:
            (*value) += 1;
            currentInstruction += 1;
            break;
        case Type::JUMP:
            currentInstruction += instruction.offset;
            break;
        case Type::JUMP_IF_EVEN:
            if ((*value) % 2 == 0) {
                currentInstruction += instruction.offset;
            } else {
                currentInstruction += 1;
            }
            break;
        case Type::JUMP_IF_ONE:
            if ((*value) == 1) {
                currentInstruction += instruction.offset;
            } else {
                currentInstruction += 1;
            }
            break;
        default:
            (void) nullptr;
            break;
        }

        return true;
    }

    void run() {
        // temporary storage
        bool canRun = true;

        while (canRun) {
            canRun = step();
        }
    }
};

int main() {
    // temporary storage
    string line;

    // computer to run code
    Computer firstComputer;
    Computer secondComputer(1);

    // read input filef
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse as instruction
        Instruction instruction(line);

        // add to computers
        firstComputer.instructions.push_back(instruction);
        secondComputer.instructions.push_back(instruction);
    }

    // run code on computers
    firstComputer.run();
    secondComputer.run();

    // display results
    cout << "[first] final value of B register : " << (*firstComputer.valueB) << endl;
    cout << "[second] final value of B register : " << (*secondComputer.valueB) << endl;

    return 0;
}
