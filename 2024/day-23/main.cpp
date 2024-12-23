#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef unordered_set<string> Connections;
typedef unordered_map<string, Connections> Graph;

string generateKey(const vector<string>& nodes) {
    // temporary storage
    string key;

    // sort the nodes
    vector<string> sorted(nodes);
    sort(sorted.begin(), sorted.end());

    // generate key
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (i != 0) {
            key += ",";
        }

        key += sorted.at(i);
    }

    return key;
}

size_t findSetsOfT(const Graph& graph) {
    // temporary storage
    unordered_set<string> foundSet;

    // iterate over graphs
    for (const pair<const string, Connections>& entry : graph) {
        // retrieve short-hands
        const string& first = entry.first;

        // filter to 't'
        if (first.front() != 't') {
            continue;
        }

        // generate possible pairs
        for (const string& second : entry.second) {
            for (const string& third : graph.at(second)) {
                // skip two-loops
                if (entry.first == third) {
                    continue;
                }

                // check if the pair is a set
                if (graph.at(first).count(third) != 0) {
                    vector<string> tmp = { first, second, third };
                    string key = generateKey(tmp);
                    foundSet.insert(key);
                }
            }
        }
    }

    return foundSet.size();
}

typedef vector<string> State;

string findLargestSet(const Graph& graph) {
    // temporary storage
    size_t largestSet = 0;
    string largestKey;

    // setup for iteration
    queue<State> queue;
    for (const pair<const string, Connections>& entry : graph) {
        queue.push({ entry.first });
    }

    unordered_set<string> visitedKeys;

    while (!queue.empty()) {
        // retrieve the current state
        State current = queue.front();
        queue.pop();

        // check for duplicated keys
        string key = generateKey(current);
        if (visitedKeys.count(key) == 0) {
            visitedKeys.insert(key);
        } else {
            continue;
        }

        // retrieve the last node
        const string& last = current.back();

        // check for largest set
        if (largestSet < current.size()) {
            largestSet = current.size();
            largestKey = generateKey(current);
        }

        // generate lookup
        unordered_set<string> lookup(current.begin(), current.end());

        // iterate over the connections
        for (const string& connection : graph.at(last)) {
            // check for ordering
            if (last >= connection) {
                continue;
            }

            // check for possible new node
            if (lookup.count(connection) != 0) {
                continue;
            }

            // retrieve short-hands
            const Connections& connections = graph.at(connection);

            bool connected = true;
            // check if this node is connected to previous nodes
            for (const string& node : current) {
                if (connections.count(node) == 0) {
                    connected = false;
                    break;
                }
            }

            // add to queue
            if (connected) {
                State next = current;
                next.push_back(connection);
                queue.push(next);
            }
        }
    }

    return largestKey;
}

int main() {
    // temporary storage
    string line;

    // store the graph
    Graph graph;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line into buffer
        stringstream buffer(line);

        string first;
        getline(buffer, first, '-');

        string second;
        getline(buffer, second);

        // add two-way edges
        graph[first].insert(second);
        graph[second].insert(first);
    }

    // check specific sets
    size_t numberOfSets = findSetsOfT(graph);
    cout << "[first] number of sets: " << numberOfSets << endl;

    // find the largest set
    string largestSet = findLargestSet(graph);
    cout << "[second] largest set: " << largestSet << endl;

    return 0;
}
