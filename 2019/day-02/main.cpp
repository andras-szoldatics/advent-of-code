#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

    Computer() :
        cursor(0),
        state(State::INITIALIZED) {}

    // tape functions
    int64_t read() {
        size_t c = cursor;

        // increment cursor
        cursor += 1;
        countRead += 1;

        return check(c);
    }

    int64_t& check(size_t ix) {
        // check for tape bounds
        if (ix >= tape.size()) {
            state = State::ERROR;
            throw runtime_error("tape out of bounds @ " + to_string(ix));
        }

        return tape.at(ix);
    }

    // operator functions
    void add() {
        // calculate result
        int64_t p1 = read();
        int64_t p2 = read();
        int64_t r = check(p1) + check(p2);

        // store result
        int64_t p3 = read();
        check(p3) = r;

        // increment step counter
        countStep += 1;
    }

    void multiply() {
        // calculate result
        int64_t p1 = read();
        int64_t p2 = read();
        int64_t r = check(p1) * check(p2);

        // store result
        int64_t p3 = read();
        check(p3) = r;

        // increment step counter
        countStep += 1;
    }

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
};

void run(Computer& computer) {
    try {
        // initialize computers to run
        computer.initialize();

        while (computer.state == State::RUNNING) {
            // read opcode
            int64_t opcode = computer.read();

            switch (opcode) {
            case 1:
                computer.add();
                break;
            case 2:
                computer.multiply();
                break;
            case 99:
                computer.finish();
                break;
            default:
                throw runtime_error("unknown opcode");
                break;
            }
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

    Computer first;
    // restore alarm state to first computer
    if (!backup.empty()) {
        first.tape = backup;
        // first.tape[1] = 12;
        // first.tape[2] = 2;
    }

    // run the computer
    run(first);

    if (first.state == State::FINISHED) {
        // display final results
        cout << "[first] final value: " << first.tape.front() << endl;
    } else {
        return 1;
    }

    // iterate over possible values
    int64_t TARGET = 19690720;
    int64_t match = 0;

    for (int64_t noun = 0; noun < 100; noun++) {
        for (int64_t verb = 0; verb < 100; verb++) {
            // simulate new computer
            Computer second;
            second.tape = backup;

            // set alarm state combination
            second.tape[1] = noun;
            second.tape[2] = verb;
            run(second);

            // check if target is reached
            if (second.tape.front() == TARGET) {
                match = 100 * noun + verb;
                break;
            }

            // handle error state
            if (second.state == State::ERROR) {
                return 2;
            }
        }
    }

    // display final results
    cout << "[second] final input: " << match << endl;

    return 0;
}
