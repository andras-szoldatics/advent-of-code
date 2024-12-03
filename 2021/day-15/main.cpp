#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Coordinate;

template<>
struct std::hash<Coordinate> {
    std::size_t operator()(const array<int64_t, 2>& a) const {
        std::size_t h1 = std::hash<int64_t>() (a.at(0));
        std::size_t h2 = std::hash<int64_t>() (a.at(1));

        return (h1 + h2);
    }
};

typedef vector<vector<int64_t>> Cavern;

class Step {
public:
    // current position
    Coordinate position;

    // total danger value
    int64_t danger;

    Step(Coordinate position = { 0, 0 },
         int64_t danger = 0) :
        position(position),
        danger(danger) {}

    // operator functions
    bool operator<(const Step& other) const {
        return (danger > other.danger);
    }
};

// utility functions
vector<Step> getNextSteps(const Cavern& cavern,
                          const Step& step) {
    // temporary storage
    vector<Step> nextSteps;

    // check if we can move up
    if (step.position.at(0) > 0) {
        // temporary storage
        Coordinate nextPosition = step.position;
        nextPosition.at(0) -= 1;

        // create next step
        Step nextStep(nextPosition, step.danger + cavern.at(nextPosition.at(0)).at(nextPosition.at(1)));

        // add next step to vector
        nextSteps.push_back(nextStep);
    }

    // check if we can move down
    if (step.position.at(0) < static_cast<int64_t>(cavern.size() - 1)) {
        // temporary storage
        Coordinate nextPosition = step.position;
        nextPosition.at(0) += 1;

        // create next step
        Step nextStep(nextPosition, step.danger + cavern.at(nextPosition.at(0)).at(nextPosition.at(1)));

        // add next step to vector
        nextSteps.push_back(nextStep);
    }

    // check if we can move left
    if (step.position.at(1) > 0) {
        // temporary storage
        Coordinate nextPosition = step.position;
        nextPosition.at(1) -= 1;

        // create next step
        Step nextStep(nextPosition, step.danger + cavern.at(nextPosition.at(0)).at(nextPosition.at(1)));

        // add next step to vector
        nextSteps.push_back(nextStep);
    }

    // check if we can move right
    if (step.position.at(1) < static_cast<int64_t>(cavern.at(0).size() - 1)) {
        // temporary storage
        Coordinate nextPosition = step.position;
        nextPosition.at(1) += 1;

        // create next step
        Step nextStep(nextPosition, step.danger + cavern.at(nextPosition.at(0)).at(nextPosition.at(1)));

        // add next step to vector
        nextSteps.push_back(nextStep);
    }

    return nextSteps;
}

Cavern extendCavern(const Cavern& cavern) {
    // check for early return
    if (cavern.empty()) {
        return cavern;
    }

    // temporary storage
    size_t height = cavern.size() * 5;
    size_t width = cavern.front().size() * 5;

    // allocate empty cavern
    Cavern extendedCavern(height, vector<int64_t>(width, 0));

    // iterate over new cavern
    for (size_t row = 0; row < height; row++) {
        for (size_t column = 0; column < width; column++) {
            // calculate danger value
            int64_t danger = cavern.at(row % cavern.size()).at(column % cavern.front().size());

            // increase danger value
            danger += static_cast<int64_t>(row / cavern.size());
            danger += static_cast<int64_t>(column / cavern.front().size());

            // check for wrap-around
            while (danger > 9) {
                danger -= 9;
            }

            // add danger value to cavern
            extendedCavern.at(row).at(column) = danger;
        }
    }

    return extendedCavern;
}

int64_t findBestPath(const Cavern& cavern) {
    // setup priority queue
    priority_queue<Step> queue;

    // setup visited set
    unordered_set<Coordinate> visited;
    Coordinate target = { static_cast<int64_t>(cavern.size() - 1), static_cast<int64_t>(cavern.at(0).size() - 1) };

    // setup iteration
    Step start;
    queue.push(start);

    while (!queue.empty()) {
        // retrieve step
        Step currentStep = queue.top();
        queue.pop();

        // check if we have reached the end
        if (currentStep.position == target) {
            return currentStep.danger;
        }

        // check if we have visited this position
        if (visited.find(currentStep.position) != visited.end()) {
            continue;
        }

        // add current position to visited set
        visited.insert(currentStep.position);

        // get next steps
        vector<Step> nextSteps = getNextSteps(cavern, currentStep);

        // add next steps to queue
        for (const Step& nextStep : nextSteps) {
            queue.push(nextStep);
        }
    }

    return -1;
}

int main() {
    // temporary storage
    string line;

    // storage
    Cavern firstCavern;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        vector<int64_t> danger(line.size(), 0);

        // parse line into danger values
        for (size_t i = 0; i < line.length(); i++) {
            danger.at(i) = static_cast<int64_t>(line.at(i) - '0');
        }

        // add danger values to cavern
        firstCavern.push_back(danger);
    }

    // find best path
    int64_t firstPath = findBestPath(firstCavern);

    // display result
    cout << "[first] best path: " << firstPath << endl;

    // crerate extended cavern
    Cavern secondCavern = extendCavern(firstCavern);

    // find best path
    int64_t secondPath = findBestPath(secondCavern);

    // display result
    cout << "[second] best path: " << secondPath << endl;

    return 0;
}
