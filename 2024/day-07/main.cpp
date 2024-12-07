#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

class Expression {
public:
    int64_t target;
    vector<int64_t> numbers;

    Expression() :
        target(0) {}
};

optional<int64_t> decode(const Expression& expression,
                         size_t mod) {
    vector<size_t> state(expression.numbers.size(), 0);

    while (state.front() == 0) {
        // temporary storage
        int64_t result = 0;

        for (size_t i = 0; i < state.size(); i++) {
            int64_t number = expression.numbers.at(i);
            int64_t factor = 1;

            switch (state.at(i)) {
            case 0:
                result += number;
                break;
            case 1:
                result *= number;
                break;
            case 2:
                while (factor <= number) {
                    factor *= 10;
                }

                result *= factor;
                result += number;
                break;
            default:
                // NO-OP
                break;
            }

            if (result > expression.target) {
                // every operation is incremental
                break;
            }
        }

        if (result == expression.target) {
            return result;
        }

        size_t shift = 1;
        // shift to next state, modulo style
        for (size_t i = 0; i < state.size(); i++) {
            // retrieve short-hands
            size_t& op = state.at(state.size() - i - 1);
            op += 1;

            if (op >= mod) {
                op = 0;
            } else {
                break;
            }
        }
    }

    return nullopt;
}

int main() {
    // temporary storage
    string line, s;

    // store the expression
    vector<Expression> expressions;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Expression expression;

        // assign to buffer
        stringstream buffer(line);

        getline(buffer, s, ':');
        expression.target = stoll(s);

        while (getline(buffer, s, ' ')) {
            if (s.empty()) {
                continue;
            }

            int64_t v = stoll(s);
            expression.numbers.push_back(v);
        }

        // store the expression
        expressions.push_back(expression);
    }

    // setup for iteration
    int64_t firstTotal = 0;
    int64_t secondTotal = 0;

    // decode the expressions - first
    for (const auto& expression : expressions) {
        optional<int64_t> result1 = decode(expression, 2);
        firstTotal += result1.value_or(0);
    }

    cout << "[first] total calibration result: " << firstTotal << endl;

    // decode the expressions - second
    for (const Expression& expression : expressions) {
        optional<int64_t> result2 = decode(expression, 3);
        secondTotal += result2.value_or(0);
    }

    cout << "[second] total calibration result: " << secondTotal << endl;

    return 0;
}
