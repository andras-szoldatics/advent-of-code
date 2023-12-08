#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

using namespace std;

class Map {
public:
    string instructions;

    unordered_map<string, string> leftNode;
    unordered_map<string, string> rightNode;
};

int64_t findFirstEnd(const Map& map) {
    // temporary storage
    size_t cursor = 0;
    string node = "AAA";
    int64_t steps = 0;

    while (node != "ZZZ") {
        // get current node
        char instruction = map.instructions.at(cursor % map.instructions.size());
        if (instruction == 'L') {
            node = map.leftNode.at(node);
        } else if (instruction == 'R') {
            node = map.rightNode.at(node);
        }

        // update cursor
        cursor += 1;
        steps += 1;
    }

    return steps;
}

int64_t findSecondEnd(const Map& map) {
    // collect nodes with end of A
    vector<string> nodes;
    for (const pair<const string, string>& entry : map.leftNode) {
        if (entry.first.back() == 'A') {
            nodes.push_back(entry.first);
        }
    }

    // solution steps
    vector<int64_t> stepsPerNode;

    for (string& node : nodes) {
        // temporary storage
        size_t cursor = 0;
        int64_t steps = 0;

        while (node.back() != 'Z') {
            // get current node
            char instruction = map.instructions.at(cursor % map.instructions.size());
            if (instruction == 'L') {
                node = map.leftNode.at(node);
            } else if (instruction == 'R') {
                node = map.rightNode.at(node);
            }

            // update cursor
            cursor += 1;
            steps += 1;
        }

        // store steps
        stepsPerNode.push_back(steps);
    }

    // calculate least common multiple
    int64_t result = stepsPerNode.at(0);
    for (size_t i = 1; i < stepsPerNode.size(); i++) {
        result = lcm(stepsPerNode.at(i), result);
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // store stuff
    Map map;

    // read input file
    ifstream input("input.txt");
    getline(input, map.instructions);
    getline(input, line);

    while (getline(input, line)) {
        // parse connections
        string node = line.substr(0, 3);
        string left = line.substr(7, 3);
        string right = line.substr(12, 3);

        // assign nodes
        map.leftNode[node] = left;
        map.rightNode[node] = right;
    }

    // execute search
    int64_t firstSteps = findFirstEnd(map);

    // display result
    cout << "[first] # of steps : " << firstSteps << endl;

    // execute search
    int64_t secondSteps = findSecondEnd(map);

    // display result
    cout << "[second] # of steps: " << secondSteps << endl;

    return 0;
}
