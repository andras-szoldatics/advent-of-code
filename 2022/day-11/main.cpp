#include <cstdint>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef function<int64_t (int64_t, int64_t)> Operation;

int64_t addNumber(int64_t original,
                  int64_t number) {
    return (original + number);
}

int64_t addSelf(int64_t original,
                int64_t /*number*/ = 0) {
    return (original * 2);
}

int64_t multiplyNumber(int64_t original,
                       int64_t number) {
    return (original * number);
}

int64_t multiplySelf(int64_t original,
                     int64_t /*number*/ = 0) {
    return (original * original);
}

class Monkey {
public:
    // currently held items
    list<int64_t> heldItems;

    // define operation
    int64_t operationNumber;
    Operation operation;

    // define test attributes
    int64_t testNumber;
    size_t throwIfTrue;
    size_t throwIfFalse;

    Monkey() :
        operationNumber(0),
        testNumber(0),
        throwIfTrue(0),
        throwIfFalse(0) {}
};

void parseItems(Monkey& monkey,
                string line) {
    // get the part of the string with the items
    size_t colonPosition = line.find(':');
    line = line.substr(colonPosition + 2);

    // put the line in a stringstream
    stringstream buffer(line);

    string stringValue;
    // read the values using trickery
    while (getline(buffer, stringValue, ',')) {
        // parse the value
        int64_t value = stoll(stringValue);

        // add value to monkey
        monkey.heldItems.push_back(value);
    }
}

void parseOperation(Monkey& monkey,
                    string line) {
    // get the part of the string with the operation
    size_t colonPosition = line.find(':');
    line = line.substr(colonPosition + 2);

    // put the line in a stringstream
    stringstream buffer(line);
    string operatorReached;

    string value;
    // read the values using trickery
    while (getline(buffer, value, ' ')) {
        // check value for operator
        if ((value == "*") ||
            (value == "+")) {
            // operator reached
            operatorReached = value;
            continue;
        }

        if (!operatorReached.empty()) {
            // check operator type
            if (operatorReached == "+") {
                if (value == "old") {
                    // this is a self reference
                    monkey.operation = addSelf;
                } else {
                    // assign function
                    monkey.operation = addNumber;

                    // parse operation number
                    monkey.operationNumber = stoll(value);
                }
            } else if (operatorReached == "*") {
                if (value == "old") {
                    // this is a self reference
                    monkey.operation = multiplySelf;
                } else {
                    // assign function
                    monkey.operation = multiplyNumber;

                    // parse operation number
                    monkey.operationNumber = stoll(value);
                }
            }
        }
    }
}

vector<size_t> monkeyBusiness(const vector<Monkey>& monkeys,
                              const size_t throwingRounds,
                              int64_t superTestNumber,
                              int64_t worryDivisor = 1) {
    // temporary storage
    vector<size_t> totalInspections(monkeys.size(), 0);
    vector<Monkey> currentMonkeys = monkeys;

    // execute rounds one-by-one
    for (size_t round = 0; round < throwingRounds; round++) {
        // iterate over monkey items
        for (size_t i = 0; i < monkeys.size(); i++) {
            // retrieve short-hands
            Monkey& monkey = currentMonkeys.at(i);

            // count items at monkey
            totalInspections.at(i) += monkey.heldItems.size();

            // iterate over items in order
            while (!monkey.heldItems.empty()) {
                // retrieve first item
                int64_t currentItem = monkey.heldItems.front();
                monkey.heldItems.pop_front();

                // execute operation
                currentItem = monkey.operation(currentItem, monkey.operationNumber);
                currentItem /= worryDivisor;
                currentItem %= superTestNumber;

                // test current item
                if (currentItem % monkey.testNumber == 0) {
                    // throw to first monkey
                    currentMonkeys.at(monkey.throwIfTrue).heldItems.push_back(currentItem);
                } else {
                    // throw to second monkey
                    currentMonkeys.at(monkey.throwIfFalse).heldItems.push_back(currentItem);
                }
            }
        }
    }

    return totalInspections;
}

int main() {
    // temporary storage
    string line;

    // vector of monkeys
    vector<Monkey> monkeys;

    // produce super test number
    int64_t superTestNumber = 1;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create new monkey
        Monkey monkey;

        // parse items
        getline(input, line);
        parseItems(monkey, line);

        // parse operations
        getline(input, line);
        parseOperation(monkey, line);

        // parse test number
        getline(input, line);
        line = line.substr(21);
        monkey.testNumber = stoll(line);

        // factor into super test
        superTestNumber *= monkey.testNumber;

        // parse first throw
        getline(input, line);
        line = line.substr(29);
        monkey.throwIfTrue = stoll(line);

        // parse second throw
        getline(input, line);
        line = line.substr(30);
        monkey.throwIfFalse = stoll(line);

        // skip empty line
        getline(input, line);

        // add monkey to vector
        monkeys.push_back(monkey);
    }

    // check for early termination
    if (monkeys.empty()) {
        return 1;
    }

    // count items at monkeys at start
    vector<size_t> firstInspections = monkeyBusiness(monkeys, 20, superTestNumber, 3);

    // sort inspections
    sort(firstInspections.begin(), firstInspections.end(), greater<int64_t>());

    // display two most inspections
    cout << "[first] #1 monkey : " << firstInspections.at(0) << endl;
    cout << "[first] #2 monkey : " << firstInspections.at(1) << endl;
    cout << "[first] monkey business : " << (firstInspections.at(0) * firstInspections.at(1)) << endl << endl;

    // count items at monkeys at start
    vector<size_t> secondInspections = monkeyBusiness(monkeys, 10000, superTestNumber);

    // sort inspections
    sort(secondInspections.begin(), secondInspections.end(), greater<int64_t>());

    // display two most inspections
    cout << "[second] #1 monkey : " << secondInspections.at(0) << endl;
    cout << "[second] #2 monkey : " << secondInspections.at(1) << endl;
    cout << "[second] monkey business : " << (secondInspections.at(0) * secondInspections.at(1)) << endl;

    return 0;
}
