#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

template<>
struct std::hash<Position> {
    size_t operator()(const array<int64_t, 2>& a) const {
        size_t h1 = std::hash<int64_t>() (a.front());
        size_t h2 = std::hash<int64_t>() (a.back());

        return (h1 + h2);
    }
};

typedef vector<Position> Layout;
typedef unordered_map<Position, char> Lookup;

enum class Traversal {
    CAUTIOUS,
    DARING
};

vector<Position> getNeighbors(const Lookup& lookup,
                              const Position& position,
                              Traversal traversal) {
    // temporary storage
    vector<Position> neighbors;

    // check for neighbors - UP
    Position up = { position.front() - 1, position.back() };
    if (lookup.find(up) != lookup.end()) {
        if ((traversal != Traversal::CAUTIOUS) ||
            (lookup.at(position) == '^') ||
            (lookup.at(position) == '.')) {
            neighbors.push_back(up);
        }
    }

    // check for neighbors - DOWN
    Position down = { position.front() + 1, position.back() };
    if (lookup.find(down) != lookup.end()) {
        if ((traversal != Traversal::CAUTIOUS) ||
            (lookup.at(position) == 'v') ||
            (lookup.at(position) == '.')) {
            neighbors.push_back(down);
        }
    }

    // check for neighbors - LEFT
    Position left = { position.front(), position.back() - 1 };
    if (lookup.find(left) != lookup.end()) {
        if ((traversal != Traversal::CAUTIOUS) ||
            (lookup.at(position) == '<') ||
            (lookup.at(position) == '.')) {
            neighbors.push_back(left);
        }
    }

    // check for neighbors - RIGHT
    Position right = { position.front(), position.back() + 1 };
    if (lookup.find(right) != lookup.end()) {
        if ((traversal != Traversal::CAUTIOUS) ||
            (lookup.at(position) == '>') ||
            (lookup.at(position) == '.')) {
            neighbors.push_back(right);
        }
    }

    return neighbors;
}

typedef vector<Position> Nodes;

void markNodes(const Layout& layout,
               const Lookup& lookup,
               Nodes& nodes) {
    // iterate over layout
    for (const Position& entry : layout) {
        // generate neighbors
        vector<Position> neighbors = getNeighbors(lookup, entry, Traversal::DARING);

        // check for start / end
        if ((neighbors.size() == 1) ||
            (neighbors.size() > 2)) {
            nodes.emplace_back(entry);
        }
    }
}

class Edge {
public:
    size_t from;
    size_t to;

    int64_t weight;

    Edge(size_t f,
         size_t t,
         int64_t w) :
        from(f),
        to(t),
        weight(w) {}
};

typedef vector<vector<Edge>> Graph;

Graph generateGraph(const Layout& layout,
                    const Lookup& lookup,
                    const Nodes& nodes,
                    Traversal traversal) {
    // temporary storage
    Graph graph;

    // generate lookup for nodes
    unordered_map<Position, size_t> nodeLookup;
    for (size_t i = 0; i < nodes.size(); i++) {
        // retrieve short-hands
        Position entry = nodes.at(i);
        nodeLookup[entry] = i;
    }

    // iterate over all nodes
    for (size_t from = 0; from < nodes.size(); from++) {
        // retrieve short-hands
        Position entry = nodes.at(from);
        graph.emplace_back();

        // setup for BFS
        unordered_map<Position, int64_t> visited;
        visited[entry] = 0;

        queue<Position> queue;
        queue.push(entry);

        // iterate over queue
        while (!queue.empty()) {
            // get current position
            Position current = queue.front();
            queue.pop();

            // generate distance
            int64_t distance = visited.at(current);

            // check if we reached a node
            if ((current != entry) &&
                (nodeLookup.count(current) != 0)) {
                // retrieve index
                size_t to = nodeLookup.at(current);

                // create edge
                Edge edge(from, to, distance);
                graph.back().push_back(edge);

                // stop further processing on this line
                continue;
            }

            // check for neighbors
            vector<Position> neighbors = getNeighbors(lookup, current, traversal);
            for (const Position& neighbor : neighbors) {
                // check if visited
                if (visited.find(neighbor) != visited.end()) {
                    continue;
                }

                // mark visitation
                visited[neighbor] = distance + 1;

                // add to queue
                queue.push(neighbor);
            }
        }
    }

    return graph;
}

class State {
public:
    size_t node;

    int64_t totalDistance;
    vector<bool> visited;

    State(size_t n,
          int64_t d,
          const vector<bool>& v) :
        node(n),
        totalDistance(d),
        visited(v) {}
};

int64_t findLongestPath(const Graph& graph) {
    // setup for BFS
    vector<bool> visited(graph.size(), false);
    int64_t longestPath = -1;

    queue<State> queue;
    queue.emplace(0, 0, visited);

    // iterate over queue
    while (!queue.empty()) {
        // get current state
        State current = queue.front();
        queue.pop();

        // check if we reached the end
        if (current.node == graph.size() - 1) {
            // check if we found a longer path
            if (current.totalDistance > longestPath) {
                longestPath = current.totalDistance;
            }

            // stop further processing on this line
            continue;
        }

        // check for next edges
        for (const Edge& edge : graph.at(current.node)) {
            // check if we visited this node already
            if (current.visited.at(edge.to)) {
                continue;
            }

            // create new state
            State next(edge.to, current.totalDistance + edge.weight, current.visited);
            next.visited.at(edge.to) = true;

            // add to queue
            queue.push(next);
        }
    }

    return longestPath;
}

int main() {
    // temporary storage line
    string line;

    // store reduced graph
    Layout layout;
    Lookup lookup;
    Nodes nodes;

    // setup for iteration
    Position cursor = { 1, 1 };

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // iterate over line
        for (char c : line) {
            // store node
            if (c != '#') {
                layout.emplace_back(cursor);
                lookup[cursor] = c;
            }

            // move cursor
            cursor.back() += 1;
        }

        // shift to next line
        cursor.front() += 1;
        cursor.back() = 1;
    }

    // mark nodes
    markNodes(layout, lookup, nodes);

    // execute search - first
    Graph firstGraph = generateGraph(layout, lookup, nodes, Traversal::CAUTIOUS);
    int64_t firstLongest = findLongestPath(firstGraph);
    cout << "[first] longest path: " << firstLongest << endl;

    // execute search - second
    Graph secondGraph = generateGraph(layout, lookup, nodes, Traversal::DARING);
    int64_t secondLongest = findLongestPath(secondGraph);
    cout << "[second] longest path: " << secondLongest << endl;

    return 0;
}
