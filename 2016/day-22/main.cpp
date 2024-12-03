#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

class Node {
public:
    // position
    int64_t x;
    int64_t y;

    // disk usage
    int64_t size;
    int64_t used;
    int64_t avail;

    Node(int64_t _x,
         int64_t _y,
         int64_t _size,
         int64_t _used,
         int64_t _avail) :
        x(_x),
        y(_y),
        size(_size),
        used(_used),
        avail(_avail) {}
};

typedef vector<Node> Nodes;
typedef unordered_map<int64_t, unordered_map<int64_t, size_t>> Lookup;

bool isViablePair(const Node& a,
                  const Node& b) {
    if (a.used == 0) {
        return false;
    }

    if ((a.x == b.x) && (a.y == b.y)) {
        return false;
    }

    if (a.used <= b.avail) {
        return true;
    }

    return false;
}

void displayNodesAsMatrix(const Nodes& nodes,
                          const Lookup& lookup) {
    // find max x & y
    int64_t maxX = 0;
    int64_t maxY = 0;

    for (const Node& node : nodes) {
        maxX = max(maxX, node.x);
        maxY = max(maxY, node.y);
    }

    // display nodes as matrix
    for (int64_t y = 0; y <= maxY; y++) {
        for (int64_t x = 0; x <= maxX; x++) {
            // retrieve node index
            optional<size_t> index;
            if (lookup.count(x) && lookup.at(x).count(y)) {
                index = lookup.at(x).at(y);
            }

            // display node as three character block
            if (index) {
                const Node& node = nodes.at(*index);
                char start = ' ';
                char middle = '.';
                char end = ' ';

                // check special nodes
                if ((node.x == 0) && (node.y == 0)) {
                    start = '(';
                    end = ')';
                } else if ((node.x == maxX) && (node.y == 0)) {
                    middle = 'G';
                } else if (node.used > 100) {
                    start = '#';
                    middle = '#';
                    end = '#';
                } else if (node.used == 0) {
                    start = '_';
                    middle = '_';
                    end = '_';
                }

                // display node
                cout << start << middle << end;
            } else {
                cout << "   ";
            }
        }

        // new line
        cout << endl;
    }
}

int main() {
    // storage
    Lookup lookup;
    Nodes nodes;

    // parse input
    string line;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // skip command & headers
        if (line.front() != '/') {
            continue;
        }

        // temporary storage
        string tmp;

        // assign line to buffer
        stringstream buffer(line);

        // parse line part by part
        getline(buffer, tmp, '-');
        getline(buffer, tmp, '-');
        tmp = tmp.substr(1);
        int64_t x = stoi(tmp);

        getline(buffer, tmp, ' ');
        tmp = tmp.substr(1);
        int64_t y = stoi(tmp);

        getline(buffer, tmp, 'T');
        int64_t size = stoi(tmp);

        getline(buffer, tmp, 'T');
        int64_t used = stoi(tmp);

        getline(buffer, tmp, 'T');
        int64_t avail = stoi(tmp);

        // assemble node
        Node node(x, y, size, used, avail);

        // store node
        lookup[x][y] = nodes.size();
        nodes.push_back(node);
    }

    // setup for iteration
    int64_t viablePairs = 0;

    // count viable pairs
    for (const Node& a : nodes) {
        for (const Node& b : nodes) {
            if (isViablePair(a, b)) {
                viablePairs++;
            }
        }
    }

    // print result
    cout << "[first] # of viable pairs: " << viablePairs << endl;

    // display nodes as matrix, that way you can solve it by hand, as I did
    cout << "[second] display nodes as nice matrix" << endl;
    displayNodesAsMatrix(nodes, lookup);

    // original input took 236 moves

    return 0;
}
