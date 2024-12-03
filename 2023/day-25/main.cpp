#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Edge {
public:
    size_t id;

    string node1;
    string node2;
};

size_t findTwoGroups(const vector<Edge>& edges) {
    // temporary storage
    unordered_map<string, int64_t> unconnected;
    unordered_set<string> connected;

    // setup for connections
    for (const Edge& edge : edges) {
        unconnected[edge.node1] = 0;
        unconnected[edge.node2] = 0;
    }

    // iterate over stuff
    while (!unconnected.empty()) {
        // temporary storage
        int64_t value = numeric_limits<int64_t>::min();
        string largest;

        // find largest unconnected vertex
        for (const pair<const string, int64_t>& entry : unconnected) {
            if (entry.second > value) {
                value = entry.second;
                largest = entry.first;
            }
        }

        // add to connected
        connected.insert(largest);
        unconnected.erase(largest);

        // update connected
        for (const Edge& edge : edges) {
            if (edge.node1 == largest) {
                if (unconnected.count(edge.node2) != 0) {
                    unconnected.at(edge.node2) += 1;
                }
            }

            if (edge.node2 == largest) {
                if (unconnected.count(edge.node1) != 0) {
                    unconnected.at(edge.node1) += 1;
                }
            }
        }

        // check for termination
        int64_t sumOfUnconnected = 0;
        for (const pair<const string, int64_t>& entry : unconnected) {
            sumOfUnconnected += entry.second;
        }

        if (sumOfUnconnected == 3) {
            break;
        }
    }

    return connected.size() * unconnected.size();
}

int main() {
    // temporary storage line
    string line, tmp;

    // store edges
    vector<Edge> edges;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);
        string node1;

        // read node1
        getline(buffer, node1, ':');
        getline(buffer, tmp, ' ');

        while (getline(buffer, tmp, ' ')) {
            // create edge
            Edge edge;
            edge.id = edges.size();
            edge.node1 = node1;
            edge.node2 = tmp;

            // add edge to vector
            edges.push_back(edge);
        }
    }

    // find two groups
    size_t firstResult = findTwoGroups(edges);
    cout << "[first] extent of two groups: " << firstResult << endl;

    return 0;
}
