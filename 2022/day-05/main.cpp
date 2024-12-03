#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>

using namespace std;

class Command {
public:
    size_t fromStack;
    size_t toStack;
    size_t amount;

    Command() :
        fromStack(0),
        toStack(0),
        amount(0) {}

    // parsing functions
    void parse(const string& line) {
        // temporary storage
        string tmp;

        // assign remaining line to buffer
        stringstream buffer(line);

        // read first value
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        amount = stoul(tmp);

        // read second value
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        fromStack = stoul(tmp) - 1;

        // read third value
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');
        toStack = stoul(tmp) - 1;
    }
};

template<size_t SIZE>
class CargoStacks {
public:
    // current state
    array<list<char>, SIZE> cargo;

    // parsing functions
    void parseCargoLine(const string& line) {
        // parse line as columns
        for (size_t i = 0; i < SIZE; i++) {
            // parse box letters
            size_t letterPosition = (i * 4) + 1;
            char letter = line.at(letterPosition);

            if (letter != ' ') {
                // skip on the number line
                if (letter == '1') {
                    break;
                }

                cargo.at(i).push_front(letter);
            }
        }
    }

    // command functions
    void command9000(const Command& command) {
        // iterate over the number of crate
        for (size_t i = 0; i < command.amount; i++) {
            // remove crate
            char crate = cargo.at(command.fromStack).back();
            cargo.at(command.fromStack).pop_back();

            // put back crate
            cargo.at(command.toStack).push_back(crate);
        }
    }

    void command9001(const Command& command) {
        // temporary storage
        list<char> temporaryStack;

        // execute command's removal
        for (size_t i = 0; i < command.amount; i++) {
            // remove crate from stack
            char crate = cargo.at(command.fromStack).back();
            cargo.at(command.fromStack).pop_back();

            // add crate to temporary stack
            temporaryStack.push_front(crate);
        }

        // execute command's addition
        for (size_t i = 0; i < command.amount; i++) {
            // remove crate from temporary stack
            char crate = temporaryStack.front();
            temporaryStack.pop_front();

            // add crate to stack
            cargo.at(command.toStack).push_back(crate);
        }
    }

    // display functions
    void printTopCargo() {
        // display results
        for (size_t i = 0; i < SIZE; i++) {
            // format the output
            if (i != 0) {
                cout << " ";
            }

            // print back item of each stack
            cout << "[" << cargo.at(i).back() << "]";
        }
        cout << endl;
    }
};

int main() {
    // temporary storage
    string line;

    // current state of cargo ship
    CargoStacks<9> firstStacks;
    CargoStacks<9> secondStacks;

    // commands for the crane
    bool commandMode = false;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // do something with the line
        if (line.empty()) {
            commandMode = true;
            continue;
        }

        if (commandMode) {
            // parse command
            Command command;
            command.parse(line);

            // add command to storage
            firstStacks.command9000(command);
            secondStacks.command9001(command);
        } else {
            // add stack to storage
            firstStacks.parseCargoLine(line);
            secondStacks.parseCargoLine(line);
        }
    }

    // display final cargo stacks
    cout << "first cargo : ";
    firstStacks.printTopCargo();

    cout << "second cargo : ";
    secondStacks.printTopCargo();

    return 0;
}
