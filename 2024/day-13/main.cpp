#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

class Equation {
public:
    int64_t xA;
    int64_t yA;

    int64_t xB;
    int64_t yB;

    int64_t targetX;
    int64_t targetY;

    Equation() :
        xA(0),
        yA(0),
        xB(0),
        yB(0),
        targetX(0),
        targetY(0) {}

    // utility functions
    optional<pair<int64_t, int64_t>> solve() const {
        // calculate determinant
        int64_t det = xA * yB - xB * yA;

        // check if determinant is zero
        if (det == 0) {
            return nullopt;
        }

        // calculate factors
        int64_t factorA = targetX * yB - targetY * xB;
        int64_t factorB = xA * targetY - yA * targetX;

        // check for integer solution
        if ((factorA % det != 0) ||
            (factorB % det != 0)) {
            return nullopt;
        }

        // calculate solution
        int64_t solutionA = factorA / det;
        int64_t solutionB = factorB / det;

        // check for non-negative solutions
        if ((solutionA < 0) ||
            (solutionB < 0)) {
            return nullopt;
        }

        return make_pair(solutionA, solutionB);
    }
};

void parseButton(const string& line,
                 int64_t& x,
                 int64_t& y) {
    // setup buffer
    stringstream buffer(line);
    string s;

    // parse X
    getline(buffer, s, 'X');
    getline(buffer, s, ',');
    x = stoll(s);

    // parse Y
    getline(buffer, s, 'Y');
    getline(buffer, s);
    y = stoll(s);
}

void parseTarget(const string& line,
                 int64_t& x,
                 int64_t& y) {
    // setup buffer
    stringstream buffer(line);
    string s;

    // parse X
    getline(buffer, s, '=');
    getline(buffer, s, ',');
    x = stoll(s);

    // parse Y
    getline(buffer, s, '=');
    getline(buffer, s);
    y = stoll(s);
}

int main() {
    // temporary storage
    string line;

    // temporary storage
    vector<Equation> equations;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Equation equation;

        // parse A button
        parseButton(line, equation.xA, equation.yA);

        // next line & parse
        getline(input, line);
        parseButton(line, equation.xB, equation.yB);

        // next line & parse
        getline(input, line);
        parseTarget(line, equation.targetX, equation.targetY);

        // skip next line
        getline(input, line);

        // store equation
        equations.push_back(equation);
    }

    // setup margin of error
    int64_t MARGIN = 10'000'000'000'000;

    // setup for iteration
    int64_t firstTokens = 0;
    int64_t secondTokens = 0;

    // try to solve equations
    for (Equation& equation : equations) {
        // calculate first variation
        optional<pair<int64_t, int64_t>> firstSolution = equation.solve();
        if (firstSolution) {
            firstTokens += firstSolution->first * 3 + firstSolution->second;
        }

        // shift equation by margin
        equation.targetX += MARGIN;
        equation.targetY += MARGIN;

        // calculate second variation
        optional<pair<int64_t, int64_t>> secondSolution = equation.solve();
        if (secondSolution) {
            secondTokens += secondSolution->first * 3 + secondSolution->second;
        }
    }

    // display results
    cout << "[first] # of tokens: " << firstTokens << endl;
    cout << "[second] # of tokens: " << secondTokens << endl;
}
