#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>

using namespace std;

typedef array<bool, 3> Modes;

class Instruction {
public:
    Modes modes;

    int64_t code;

    Instruction(int64_t opcode) {
        // extract opcode
        code = opcode % 100;
        opcode /= 100;

        // extract modes
        for (size_t i = 0; i < 3; i++) {
            modes.at(i) = (opcode % 10) != 0;
            opcode /= 10;
        }
    }
};

enum class State : uint8_t {
    INITIALIZED,
    RUNNING,
    ERROR,
    FINISHED
};

class Computer {
public:
    // tape storage
    vector<int64_t> tape;
    size_t cursor;

    // state storage
    State state;
    size_t countStep;
    size_t countRead;

    // I/O storage
    queue<int64_t> input;
    vector<int64_t> output;

    Computer(const vector<int64_t>& backup) :
        tape(backup),
        cursor(0),
        countStep(0),
        countRead(0),
        state(State::INITIALIZED) {}

    // state functions
    void initialize() {
        // mark running state
        state = State::RUNNING;
    }

    void finish() {
        // mark finished state
        state = State::FINISHED;

        // increment step counter
        countStep += 1;
    }

    // tape functions
    int64_t tapeRead() {
        size_t c = cursor;

        // increment cursor
        cursor += 1;
        countRead += 1;

        return tapeAt(c);
    }

    int64_t& tapeAt(size_t ix) {
        // check for tape bounds
        if (ix >= tape.size()) {
            state = State::ERROR;
            throw runtime_error("tape out of bounds @ " + to_string(ix));
        }

        return tape.at(ix);
    }

    // operator functions
    void add(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();
        int64_t r = (modes.at(0) ? p1 : tapeAt(p1)) +
            (modes.at(1) ? p2 : tapeAt(p2));

        // store result
        int64_t p3 = tapeRead();
        tapeAt(p3) = r;

        // increment step counter
        countStep += 1;
    }

    void multiply(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();
        int64_t r = (modes.at(0) ? p1 : tapeAt(p1)) *
            (modes.at(1) ? p2 : tapeAt(p2));

        // store result
        int64_t p3 = tapeRead();
        tapeAt(p3) = r;

        // increment step counter
        countStep += 1;
    }

    void read(const Modes& modes) {
        // check for input
        if (input.empty()) {
            state = State::ERROR;
            throw runtime_error("input missing @ " + to_string(cursor));
        }

        // read input
        int64_t r = input.front();
        input.pop();

        // store result
        int64_t p1 = tapeRead();
        tapeAt(p1) = r;

        // increment step counter
        countStep += 1;
    }

    void write(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t r = modes.at(0) ? p1 : tapeAt(p1);

        // store result
        output.push_back(r);

        // increment step counter
        countStep += 1;
    }

    void jumpIfTrue(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();

        int64_t value = modes.at(0) ? p1 : tapeAt(p1);
        int64_t jump = modes.at(1) ? p2 : tapeAt(p2);

        if (value != 0) {
            cursor = jump;
        }

        // increment step counter
        countStep += 1;
    }

    void jumpIfFalse(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();

        int64_t value = modes.at(0) ? p1 : tapeAt(p1);
        int64_t jump = modes.at(1) ? p2 : tapeAt(p2);

        if (value == 0) {
            cursor = jump;
        }

        // increment step counter
        countStep += 1;
    }

    void lessThan(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();

        int64_t value1 = modes.at(0) ? p1 : tapeAt(p1);
        int64_t value2 = modes.at(1) ? p2 : tapeAt(p2);

        int64_t p3 = tapeRead();
        tapeAt(p3) = (value1 < value2) ? 1 : 0;

        // increment step counter
        countStep += 1;
    }

    void equals(const Modes& modes) {
        // calculate result
        int64_t p1 = tapeRead();
        int64_t p2 = tapeRead();

        int64_t value1 = modes.at(0) ? p1 : tapeAt(p1);
        int64_t value2 = modes.at(1) ? p2 : tapeAt(p2);

        int64_t p3 = tapeRead();
        tapeAt(p3) = (value1 == value2) ? 1 : 0;

        // increment step counter
        countStep += 1;
    }

    // utility functions
    void execute(const Instruction& step) {
        switch (step.code) {
        case 1:
            add(step.modes);
            break;

        case 2:
            multiply(step.modes);
            break;

        case 3:
            read(step.modes);
            break;

        case 4:
            write(step.modes);
            break;

        case 5:
            jumpIfTrue(step.modes);
            break;

        case 6:
            jumpIfFalse(step.modes);
            break;

        case 7:
            lessThan(step.modes);
            break;

        case 8:
            equals(step.modes);
            break;

        case 99:
            finish();
            break;

        default:
            state = State::ERROR;
            throw runtime_error("invalid opcode: " + to_string(step.code));
        }
    }
};

void run(Computer& computer) {
    try {
        // initialize computers to run
        computer.initialize();

        while (computer.state == State::RUNNING) {
            // read instruction
            int64_t value = computer.tapeRead();
            Instruction step(value);

            // execute instruction
            computer.execute(step);
        }
    } catch (const runtime_error& e) {
        cout << "[error] " << e.what() << endl;
    }
}

int main() {
    // temporary storage
    string line, s;

    // setup tape storage
    vector<int64_t> backup;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign into buffer
        stringstream buffer(line);

        while (getline(buffer, s, ',')) {
            // convert to integer
            int64_t i = stoll(s);

            // store on backup
            backup.push_back(i);
        }
    }

    // run the computer
    Computer first(backup);
    first.input.push(1);
    run(first);

    if ((first.state == State::FINISHED) &&
        (!first.output.empty())) {
        // display final results
        cout << "[first] final value: " << first.output.back() << endl;
    } else {
        return 1;
    }

    // run the computer, again
    Computer second(backup);
    second.input.push(5);
    run(second);

    if ((second.state == State::FINISHED) &&
        (!second.output.empty())) {
        // display final results
        cout << "[second] final value: " << second.output.back() << endl;
    } else {
        return 1;
    }

    return 0;
}
