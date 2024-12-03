#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

class Brick {
public:
    int64_t fromX;
    int64_t fromY;
    int64_t fromZ;

    int64_t sizeX;
    int64_t sizeY;
    int64_t sizeZ;

    Brick(const string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // read brick data
        int64_t x1, y1, z1;
        getline(buffer, tmp, ',');
        x1 = stoi(tmp);

        getline(buffer, tmp, ',');
        y1 = stoi(tmp);

        getline(buffer, tmp, '~');
        z1 = stoi(tmp);

        int64_t x2, y2, z2;
        getline(buffer, tmp, ',');
        x2 = stoi(tmp);

        getline(buffer, tmp, ',');
        y2 = stoi(tmp);

        getline(buffer, tmp);
        z2 = stoi(tmp);

        // normalize data
        fromX = min(x1, x2);
        fromY = min(y1, y2);
        fromZ = min(z1, z2);

        sizeX = abs(x1 - x2) + 1;
        sizeY = abs(y1 - y2) + 1;
        sizeZ = abs(z1 - z2) + 1;
    }

    // helper functions
    bool contains(int64_t x,
                  int64_t y,
                  int64_t z) const {
        // calculate absolute coordinates
        int64_t deltaX = x - fromX;
        if ((deltaX < 0) ||
            (deltaX >= sizeX)) {
            return false;
        }

        int64_t deltaY = y - fromY;
        if ((deltaY < 0) ||
            (deltaY >= sizeY)) {
            return false;
        }

        int64_t deltaZ = z - fromZ;
        if ((deltaZ < 0) ||
            (deltaZ >= sizeZ)) {
            return false;
        }

        return true;
    }

    // operator functions
    bool operator<(const Brick& other) const {
        if (fromZ != other.fromZ) {
            return fromZ < other.fromZ;
        }

        if (fromY != other.fromY) {
            return fromY < other.fromY;
        }

        return fromX < other.fromX;
    }
};

class Stack {
public:
    // brick storage
    vector<Brick> bricks;

    Stack() {
        // initialize height map
        heightMap.fill({ 0 });
    }

    // helper functions
    void add(const Brick& brick) {
        // find hold height
        int64_t lastHeight = 0;

        // iterate over base of brick
        for (int64_t x = 0; x < brick.sizeX; x++) {
            for (int64_t y = 0; y < brick.sizeY; y++) {
                // update height
                lastHeight = max(lastHeight, heightMap[brick.fromX + x][brick.fromY + y]);
            }
        }

        // make a temporary copy
        Brick tmp = brick;

        // shift brick height
        tmp.fromZ = lastHeight + 1;

        for (int64_t x = 0; x < brick.sizeX; x++) {
            for (int64_t y = 0; y < brick.sizeY; y++) {
                // calculate absolute coordinates
                int64_t stackX = brick.fromX + x;
                int64_t stackY = brick.fromY + y;

                // update height map
                heightMap[stackX][stackY] = lastHeight + brick.sizeZ;
            }
        }

        // add empty set
        thisIsSupported[bricks.size()] = {};
        thisOneSupports[bricks.size()] = {};

        // iterate over other bricks
        for (size_t i = 0; i < bricks.size(); i++) {
            // retrieve short-hands
            const Brick& other = bricks.at(i);
            bool isHeldUp = false;

            for (int64_t x = 0; x < brick.sizeX; x++) {
                for (int64_t y = 0; y < brick.sizeY; y++) {
                    // calculate absolute coordinates
                    int64_t stackX = brick.fromX + x;
                    int64_t stackY = brick.fromY + y;

                    if (other.contains(stackX, stackY, lastHeight)) {
                        isHeldUp = true;
                        break;
                    }
                }
            }

            if (isHeldUp) {
                // update lookup
                thisIsSupported[bricks.size()].push_back(i);
                thisOneSupports[i].push_back(bricks.size());
            }
        }

        // add brick to stack
        bricks.push_back(tmp);
    }

    // utility functions
    size_t findUselessBricks() const {
        // temporary storage
        unordered_set<size_t> crucialBricks;

        // iterate over bricks
        for (size_t i = 0; i < bricks.size(); i++) {
            // retrieve short-hands
            const vector<size_t>& supportBricks = thisIsSupported.at(i);

            // check if brick is crucial
            if (supportBricks.size() == 1) {
                crucialBricks.insert(supportBricks.front());
            }
        }

        // return number of useless bricks
        return bricks.size() - crucialBricks.size();
    }

    size_t findMaximumCascade() const {
        // temporary storage
        size_t totalCascade = 0;

        // iterate over bricks
        for (size_t i = 0; i < bricks.size(); i++) {
            // setup for iteration
            queue<size_t> queue;
            queue.emplace(i);

            // setup for cascade
            unordered_set<size_t> cascade;

            while (!queue.empty()) {
                // retrieve short-hands
                size_t index = queue.front();
                queue.pop();

                // check if brick is already visited
                if (cascade.empty()) {
                    // mark brick as visited
                    cascade.insert(index);
                } else if (cascade.count(index) != 0) {
                    continue;
                } else {
                    // temporary storage
                    size_t supports = 0;

                    // check if this brick is still supported
                    for (size_t j : thisIsSupported.at(index)) {
                        if (cascade.count(j) == 0) {
                            supports += 1;
                        }
                    }

                    if (supports == 0) {
                        // mark brick as visited
                        cascade.insert(index);
                    } else {
                        continue;
                    }
                }

                // iterate over bricks
                for (size_t j : thisOneSupports.at(index)) {
                    // add brick to queue
                    queue.emplace(j);
                }
            }

            // update total cascade
            if (cascade.size() > 0) {
                totalCascade += cascade.size() - 1;
            }
        }

        return totalCascade;
    }

private:
    // lookup stroge
    array<array<int64_t, 10>, 10> heightMap;
    unordered_map<size_t, vector<size_t>> thisIsSupported;
    unordered_map<size_t, vector<size_t>> thisOneSupports;
};

int main() {
    // temporary storage line
    string line;

    // store bricks
    vector<Brick> bricks;
    Stack stack;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse brick
        bricks.emplace_back(line);
    }

    // sort bricks
    sort(bricks.begin(), bricks.end());

    // add bricks to stack
    for (const Brick& brick : bricks) {
        stack.add(brick);
    }

    // calculate useless bricks
    size_t firstResult = stack.findUselessBricks();

    // display result
    cout << "[first] number of useless bricks: " << firstResult << endl;

    // calculate maximum cascade
    size_t secondResult = stack.findMaximumCascade();

    // display result
    cout << "[second] maximum cascade: " << secondResult << endl;

    return 0;
}
