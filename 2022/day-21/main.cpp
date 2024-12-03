#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

typedef function<int64_t ()> Yell;

class Monkey {
public:
    // simple attributes
    string id;
    Yell yell;

    // parsing function
    void parse(const unordered_map<string, Monkey>& monkeys,
               const string& line,
               bool rootOverride = false) {
        // assign line into buffer
        stringstream buffer(line);

        // retrieve id
        getline(buffer, id, ':');

        // retrieve left operand
        string leftOperand;
        getline(buffer, leftOperand, ' ');
        getline(buffer, leftOperand, ' ');

        // retrieve operator
        string sign;
        getline(buffer, sign, ' ');

        // retrieve right operand
        string rightOperand;
        getline(buffer, rightOperand, ' ');

        // execute root override
        if (rootOverride) {
            sign = "-";
        }

        // check for single number
        if (sign.empty()) {
            // retrieve single number
            int64_t number = stoll(leftOperand);

            // assign a simple lambda
            yell = [number] () -> int64_t {
                    return number;
                };
        } else {
            // assign a flexible lamdba
            yell = [&monkeys, leftOperand, rightOperand, sign] () -> int64_t {
                    // retrieve short-hands
                    int64_t left = monkeys.at(leftOperand).yell();
                    int64_t right = monkeys.at(rightOperand).yell();

                    // check sign type
                    switch (sign.front()) {
                    case '+':
                        return (left + right);
                        break;
                    case '-':
                        return (left - right);
                        break;
                    case '*':
                        return (left * right);
                        break;
                    case '/':
                        return (left / right);
                        break;
                    default:
                        // NO-OP
                        (void) nullptr;
                    }

                    return 0;
                };
        }
    }
};

int64_t getResult(unordered_map<string, Monkey>& monkeys,
                  int64_t humanInput) {
    // re-write humn number
    monkeys.at("humn").yell = [humanInput] () -> int64_t {
            return humanInput;
        };
    return monkeys.at("root").yell();
}

int64_t binarySearch(unordered_map<string, Monkey>& monkeys,
                     int64_t firstYell) {
    // temporary storage
    int64_t humanValue = 0;

    // setup interval for search
    array<int64_t, 3> humanInterval = { 0, 0, firstYell };
    array<int64_t, 3> rootInterval = { 0, 0, 0 };

    // retrieve interval values
    rootInterval.front() = getResult(monkeys, humanInterval.front());
    rootInterval.back() = getResult(monkeys, humanInterval.back());

    if (rootInterval.front() < rootInterval.back()) {
        // swap interval, if appropriate
        swap(humanInterval.front(), humanInterval.back());
        swap(rootInterval.front(), rootInterval.back());
    }

    bool foundValue = false;
    // iterate until 0 is found
    while (!foundValue) {
        // retrieve middle value
        humanInterval.at(1) = ((humanInterval.front() + humanInterval.back()) / 2);
        rootInterval.at(1) = getResult(monkeys, humanInterval.at(1));

        // check for termination
        for (size_t i = 0; i < rootInterval.size(); i++) {
            // retrieve short-hands
            const int64_t& rootValue = rootInterval.at(i);

            // check value for 0
            if (rootValue == 0) {
                // this is a good value
                int64_t goodValue = humanInterval.at(i);

                // try to round down, to account for integer division
                while (getResult(monkeys, goodValue) == 0) {
                    // set human value
                    humanValue = goodValue;

                    // shift value down
                    goodValue--;
                }

                // found value
                foundValue = true;

                continue;
            }
        }

        // check for the better half
        if ((rootInterval.front() >= 0) &&
            (0 >= rootInterval.at(1))) {
            // move interval limits
            humanInterval.back() = humanInterval.at(1);
            rootInterval.back() = rootInterval.at(1);
        } else if ((rootInterval.at(1) >= 0) &&
                   (0 >= rootInterval.back())) {
            // move interval limits
            humanInterval.front() = humanInterval.at(1);
            rootInterval.front() = rootInterval.at(1);
        } else {
            break;
        }
    }

    return humanValue;
}

int main() {
    // temporary storage
    string line;

    // stored monkeys
    unordered_map<string, Monkey> monkeys;
    string rootLine;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line into monkey
        Monkey monkey;
        monkey.parse(monkeys, line);

        // add monkey to monkeys
        monkeys[monkey.id] = monkey;

        // set line for root
        if (monkey.id == "root") {
            rootLine = line;
        }
    }

    // retrieve yell of the root monkey
    int64_t firstYell = monkeys.at("root").yell();

    // diplay results
    cout << "[first] yell of the 'root' monkey : " << firstYell << endl;

    // re-write root monkey function
    monkeys.at("root").parse(monkeys, rootLine, true);

    // search for the target 'humn' value
    int64_t secondYell = binarySearch(monkeys, firstYell);

    // display results
    cout << "[second] yell of the 'humn' monkey : " << secondYell << endl;

    return 0;
}
