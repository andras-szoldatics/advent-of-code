#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

enum Comparison {
    NONE,
    LESS,
    MORE
};

class Part {
public:
    // ratings (X, M, A, S)
    array<int64_t, 4> ratings;

    Part(int64_t x,
         int64_t m,
         int64_t a,
         int64_t s) :
        ratings({ x, m, a, s }) {}

    Part(const string& line) {
        // assign line to buffer
        stringstream buffer(line);

        // temporary storage
        string tmp;

        for (size_t i = 0; i < 4; i++) {
            // skip to number
            getline(buffer, tmp, '=');

            // parse first number
            getline(buffer, tmp, ',');
            ratings[i] = stoi(tmp);
        }
    }

    // utility functions
    int64_t getTotal() const {
        return ratings.at(0) +
               ratings.at(1) +
               ratings.at(2) +
               ratings.at(3);
    }
};

const unordered_map<string, size_t> RATING_MAP = {
    { "x", 0 },
    { "m", 1 },
    { "a", 2 },
    { "s", 3 }
};

class Check {
public:
    // attributes
    size_t ratingIndex;
    Comparison comparison;
    int64_t value;

    string nextFlow;

    Check(size_t rI,
          Comparison c,
          int64_t v,
          const string& nF) :
        ratingIndex(rI),
        comparison(c),
        value(v),
        nextFlow(nF) {}

    // utility functions
    bool checkPart(const Part& part) const {
        if (comparison == LESS) {
            return part.ratings.at(ratingIndex) < value;
        } else if (comparison == MORE) {
            return part.ratings.at(ratingIndex) > value;
        }

        return false;
    }
};

class WorkFlow {
public:
    // attributes
    string id;

    // checks
    vector<Check> checks;
    string fallback;

    WorkFlow(const string& line) {
        // assign line to buffer
        stringstream buffer(line);

        // temporary storage
        string tmp;

        // parse id
        getline(buffer, id, '{');

        while (getline(buffer, tmp, ',')) {
            // check if this is the last entry
            if (tmp.back() == '}') {
                tmp.pop_back();
                fallback = tmp;
                continue;
            }

            Comparison comparison = NONE;

            // seperate types
            size_t lessIndex = tmp.find('<');
            if (lessIndex != string::npos) {
                comparison = LESS;
            }

            size_t moreIndex = tmp.find('>');
            if (moreIndex != string::npos) {
                comparison = MORE;
            }

            size_t comparisonIndex = min(lessIndex, moreIndex);

            // find rating
            string ratingStr = tmp.substr(0, comparisonIndex);
            size_t ratingIndex = RATING_MAP.at(ratingStr);

            // find value
            string valueStr = tmp.substr(comparisonIndex + 1);
            int64_t value = stoll(valueStr);

            // find last part
            size_t colonIndex = tmp.find(':');
            string nextFlow = tmp.substr(colonIndex + 1);

            // add check
            checks.emplace_back(ratingIndex, comparison, value, nextFlow);
        }
    }

    // utility functions
    string processPart(const Part& part) const {
        // iterate over checks
        for (const Check& check : checks) {
            if (check.checkPart(part)) {
                return check.nextFlow;
            }
        }

        return fallback;
    }
};

const string ACCEPT("A");
const string REJECT("R");

string processOnWorkFlows(const unordered_map<string, WorkFlow>& workFlows,
                          const Part& part) {
    // setup for iteration
    string current("in");

    while ((current != ACCEPT) &&
           (current != REJECT)) {
        // get workflow
        const WorkFlow& workFlow = workFlows.at(current);

        // process part
        current = workFlow.processPart(part);
    }

    return current;
}

class Ranges {
public:
    // state
    string workFlow;

    // attributes
    array<int64_t, 4> minimums;
    array<int64_t, 4> maximums;

    Ranges(const string& w = string("in"),
           const array<int64_t, 4>& mins = { 1, 1, 1, 1 },
           const array<int64_t, 4>& maxs = { 4000, 4000, 4000, 4000 }) :
        workFlow(w),
        minimums(mins),
        maximums(maxs) {}

    // helper functions
    int64_t getCombinations() {
        int64_t xDelta = maximums.at(0) - minimums.at(0) + 1;
        if (xDelta <= 0) {
            return 0;
        }

        int64_t mDelta = maximums.at(1) - minimums.at(1) + 1;
        if (mDelta <= 0) {
            return 0;
        }

        int64_t aDelta = maximums.at(2) - minimums.at(2) + 1;
        if (aDelta <= 0) {
            return 0;
        }

        int64_t sDelta = maximums.at(3) - minimums.at(3) + 1;
        if (sDelta <= 0) {
            return 0;
        }

        return xDelta * mDelta * aDelta * sDelta;
    }

    // utility functions
    Ranges truePart(const Check& check) {
        // create new ranges
        Ranges result(check.nextFlow, minimums, maximums);

        // update ranges
        if (check.comparison == LESS) {
            result.maximums.at(check.ratingIndex) = check.value - 1;
        } else if (check.comparison == MORE) {
            result.minimums.at(check.ratingIndex) = check.value + 1;
        }

        return result;
    }

    Ranges falsePart(const Check& check) {
        // create new ranges
        Ranges result(workFlow, minimums, maximums);

        // update ranges
        if (check.comparison == LESS) {
            result.minimums.at(check.ratingIndex) = check.value;
        } else if (check.comparison == MORE) {
            result.maximums.at(check.ratingIndex) = check.value;
        }

        return result;
    }
};

int64_t analyzeWorkFlows(const unordered_map<string, WorkFlow>& workFlows) {
    // setup for iteration
    queue<Ranges> analysis;
    analysis.emplace();

    // setup for iteration
    int64_t total = 0;

    // iterate over different ranges
    while (!analysis.empty()) {
        // get current ranges
        Ranges ranges = analysis.front();
        analysis.pop();

        // check if we arrived at a conclusion
        if (ranges.workFlow == ACCEPT) {
            // we are done
            total += ranges.getCombinations();
            continue;
        } else if (ranges.workFlow == REJECT) {
            // skip further processing
            continue;
        }

        // get workflow
        const WorkFlow& workFlow = workFlows.at(ranges.workFlow);

        // iterate over all checks
        for (const Check& check : workFlow.checks) {
            // create new range for true
            Ranges trueRanges = ranges.truePart(check);
            analysis.push(trueRanges);

            // modify current range
            ranges = ranges.falsePart(check);
        }

        // add fallback option
        Ranges fallbackRanges = ranges;
        fallbackRanges.workFlow = workFlow.fallback;

        analysis.push(fallbackRanges);
    }

    return total;
}

int main() {
    // temporary storage line
    string line;

    // store pile of stuff
    unordered_map<string, WorkFlow> workFlows;
    vector<Part> parts;

    // setup for iteration
    size_t state = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        if (state == 0) {
            // parse workflows
            WorkFlow workFlow(line);
            workFlows.emplace(workFlow.id, workFlow);
        } else if (state == 1) {
            // parse parts
            parts.emplace_back(line);
        }

        if (line.empty()) {
            // increment state
            state += 1;
        }
    }

    // setup for iteration
    int64_t firstTotal = 0;

    // iterate over all parts
    for (const Part& part : parts) {
        // get workflow
        string finalBin = processOnWorkFlows(workFlows, part);
        if (finalBin == ACCEPT) {
            firstTotal += part.getTotal();
        }
    }

    // display result
    cout << "[first] total rating numbers: " << firstTotal << endl;

    // setup for iteration
    int64_t secondTotal = analyzeWorkFlows(workFlows);

    // display result
    cout << "[second] total valid combinations: " << secondTotal << endl;

    return 0;
}
