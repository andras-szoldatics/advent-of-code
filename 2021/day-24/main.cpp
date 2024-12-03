#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

enum class Type {
    UNDEFINED,
    PUSH,
    POP
};

class SubAlgorithm {
public:
    // attributes
    size_t index;
    Type type;

    int64_t deltaX;
    int64_t deltaY;

    SubAlgorithm() :
        index(0),
        type(Type::UNDEFINED),
        deltaX(0),
        deltaY(0) {}
};

int main() {
    // temporary storage
    string line;

    // keep a running tally
    size_t row = 0;

    // store the sub algorithms
    array<SubAlgorithm, 18> algorithms;

    // read in the file
    ifstream file("input.txt");
    while (getline(file, line)) {
        // temporary storage
        string tmp;

        // check for parts of algorithms
        size_t index = (row / 18);
        size_t partial = (row % 18);

        switch (partial) {
        case 0:
            // assign index
            algorithms.at(index).index = index;
            break;
        case 4:
            // assign type
            if (line.back() == '1') {
                algorithms.at(index).type = Type::PUSH;
            } else {
                algorithms.at(index).type = Type::POP;
            }
            break;
        case 5:
            // assign deltaX
            tmp = line.substr(5);
            algorithms.at(index).deltaX = stoll(tmp);
            break;
        case 15:
            // assign deltaY
            tmp = line.substr(5);
            algorithms.at(index).deltaY = stoll(tmp);
            break;
        }

        // increment row
        row += 1;
    }

    // setup initial numbers
    string largest(14, '.');
    string smallest(14, '.');

    // setup for iteration
    stack<size_t> stack;

    // simulate the stack pairings
    for (size_t i = 0; i < 18; i++) {
        // get the current algorithm
        const SubAlgorithm& current = algorithms.at(i);

        // check for push
        if (current.type == Type::PUSH) {
            // push the index
            stack.push(current.index);
        } else if (current.type == Type::POP) {
            // pop the previous index
            size_t pair = stack.top();
            stack.pop();

            // get the previous algorithm
            const SubAlgorithm& previous = algorithms.at(pair);

            // calculate delta to previous
            int64_t delta = current.deltaX + previous.deltaY;

            if (delta >= 0) {
                // assign smallest value
                smallest.at(previous.index) = to_string(1).front();
                smallest.at(current.index) = to_string(1 + delta).front();

                // assign largest value
                largest.at(previous.index) = to_string(9 - delta).front();
                largest.at(current.index) = to_string(9).front();
            } else {
                // assign smallest value
                smallest.at(previous.index) = to_string(1 - delta).front();
                smallest.at(current.index) = to_string(1).front();

                // assign largest value
                largest.at(previous.index) = to_string(9).front();
                largest.at(current.index) = to_string(9 + delta).front();
            }
        }
    }

    // display results
    cout << "[first] largest valid number: " << largest << endl;
    cout << "[second] smallest valid number: " << smallest << endl;

    return 0;
}
