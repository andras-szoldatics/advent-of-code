#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

const int64_t TOP_OF_TUNNEL = numeric_limits<int32_t>::max();

class Rock {
public:
    // attribute
    int64_t width;

    vector<int64_t> underSide;
    vector<int64_t> topSide;

    // position
    int64_t leftMostBlock;
    int64_t lowestBlock;

    Rock(int64_t _width) :
        width(_width),
        underSide(width, 0),
        topSide(width, 0),
        leftMostBlock(2),
        lowestBlock(TOP_OF_TUNNEL) {}

    // utility functions
    void moveDown() {
        // bounded move
        if (lowestBlock > 1) {
            lowestBlock--;
        }
    }

    void moveLeft(size_t /*size*/) {
        // bounded move
        if (leftMostBlock > 0) {
            leftMostBlock--;
        }
    }

    void moveRight(size_t size) {
        size_t rightMostBlock = (leftMostBlock + width - 1);

        // bounded move
        if (rightMostBlock + 1 < size) {
            leftMostBlock++;
        }
    }
};

template<size_t SIZE>
class VerticalTunnel {
public:
    // state of the tunnel
    array<int64_t, SIZE> blocks;
    int64_t highestBlock;

    // exact state of the tunnel
    array<vector<bool>, SIZE> matrix;

    VerticalTunnel() :
        highestBlock(0) {
        fill(blocks.begin(), blocks.end(), 0);
        fill(matrix.begin(), matrix.end(), vector<bool>(10, false));
    }

    // utility functions
    bool isOverLapping(const Rock& shape) {
        // iterate over shape's bottom blocks
        for (int64_t i = 0; i < shape.width; i++) {
            // calculate bottom block's position
            int64_t left = (shape.leftMostBlock + i);
            int64_t top = (shape.lowestBlock + shape.topSide.at(i));
            int64_t bottom = (shape.lowestBlock + shape.underSide.at(i));

            // iterate over blocks in shape
            for (int64_t block = bottom; block <= top; block++) {
                // fill in block
                if (matrix.at(left).at(block)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isTouchingLeft(const Rock& shape) {
        // temporary storage
        Rock tmp = shape;
        tmp.moveLeft(SIZE);

        // check for overlap
        return isOverLapping(tmp);
    }

    bool isTouchingRight(const Rock& shape) {
        // temporary storage
        Rock tmp = shape;
        tmp.moveRight(SIZE);

        // check for overlap
        return isOverLapping(tmp);
    }

    bool isTouchingDown(const Rock& shape) {
        // simple check
        if (shape.lowestBlock == 1) {
            return true;
        }

        // temporary storage
        Rock tmp = shape;
        tmp.moveDown();

        // check for overlap
        return isOverLapping(tmp);
    }

    void addShape(const Rock& shape) {
        // iterate over shape's bottom blocks
        for (int64_t i = 0; i < shape.width; i++) {
            // calculate bottom block's position
            int64_t left = (shape.leftMostBlock + i);
            int64_t top = (shape.lowestBlock + shape.topSide.at(i));
            int64_t bottom = (shape.lowestBlock + shape.underSide.at(i));

            // assign new block height
            if (blocks.at(left) < top) {
                blocks.at(left) = top;

                // check for highest block change
                if (highestBlock < top) {
                    highestBlock = top;

                    // resize fill data
                    for (vector<bool>& column : matrix) {
                        column.resize(highestBlock + 10, false);
                    }
                }
            }

            // iterate over blocks in shape
            for (int64_t block = bottom; block <= top; block++) {
                // fill in block
                matrix.at(left).at(block) = true;
            }
        }
    }

    // print functions
    void printFilledState() {
        // iterate over rows
        for (int64_t row = (highestBlock + 6); row >= 1; row--) {
            // iterate over entries
            for (size_t column = 0; column < SIZE; column++) {
                cout << (matrix.at(column).at(row) ? "#" : ".");
            }

            cout << endl;
        }

        cout << endl;
    }
};

class State {
public:
    int64_t rocksDropped;
    int64_t highestBlock;
    int64_t clock;

    State() :
        rocksDropped(0),
        highestBlock(0),
        clock(0) {}
    State(int64_t _rocksDropped,
          int64_t _highestBlock,
          int64_t _clock) :
        rocksDropped(_rocksDropped),
        highestBlock(_highestBlock),
        clock(_clock) {}
};

int main() {
    // temporary storage
    string line;

    // initialize singular shapes
    Rock wall(4);

    Rock cross(3);
    cross.underSide = { 1, 0, 1 };
    cross.topSide = { 1, 2, 1 };

    Rock bigL(3);
    bigL.topSide = { 0, 0, 2 };

    Rock tower(1);
    tower.topSide = { 3 };

    Rock chonky(2);
    chonky.topSide = { 1, 1 };

    // initialize list of shapes
    vector<Rock> rocks = { wall, cross, bigL, tower, chonky };
    size_t rockIndex = 0;

    // initialize tunnel
    VerticalTunnel<7> tunnel;

    // setup iteration
    const int64_t INTERLUDE_ROUNDS = 2022;
    const int64_t MAXIMUM_ROUNDS = 1000000000000;
    int64_t rocksDropped = 0;
    int64_t clock = 0;

    // loop detection
    unordered_map<string, State> loopDetection;
    int64_t loopedHeight = 0;

    // read input file
    ifstream input("input.txt");

    // read first & only relevant line
    getline(input, line);

    // temporary storage
    Rock shape = rocks.at(rockIndex++);
    shape.lowestBlock = (tunnel.highestBlock + 4);
    int64_t windIndex = 0;

    // iterate over line's characters
    while (rocksDropped < MAXIMUM_ROUNDS) {
        // retrieve wind and step onto next
        char wind = line.at(windIndex++);
        windIndex %= line.size();

        // follow the wind
        if (wind == '>') {
            if (!tunnel.isTouchingRight(shape)) {
                shape.moveRight(tunnel.blocks.size());
            }
        } else if (wind == '<') {
            if (!tunnel.isTouchingLeft(shape)) {
                shape.moveLeft(tunnel.blocks.size());
            }
        }

        // falling down step
        if (!tunnel.isTouchingDown(shape)) {
            // falling down
            shape.moveDown();
        } else {
            // coming to rest
            tunnel.addShape(shape);
            rocksDropped++;

            // select next shape
            shape = rocks.at(rockIndex++);
            shape.lowestBlock = (tunnel.highestBlock + 4);

            // mod shape index to make it circular
            rockIndex %= rocks.size();

            // check for interlude rounds
            if (rocksDropped == INTERLUDE_ROUNDS) {
                // display highest block
                cout << "after round #" << INTERLUDE_ROUNDS << ", highest block in tunnel : " << tunnel.highestBlock << endl;
            }
        }

        // increase clock to keep a tally
        clock++;

        if (rocksDropped < INTERLUDE_ROUNDS) {
            // skip looping before the interlude
            continue;
        }

        // check for loops
        if (clock % line.size() == 0) {
            // assemble a hash string
            stringstream buffer;

            // assemble hash
            buffer << shape.leftMostBlock << ":" << (shape.lowestBlock - tunnel.highestBlock) << ":" << rockIndex;
            for (int64_t block : tunnel.blocks) {
                buffer << ":" << (tunnel.highestBlock - block);
            }

            string hash = buffer.str();
            // check if this hash already exists
            if (loopDetection.count(hash) == 0) {
                // add hash to detection
                loopDetection[hash] = State(rocksDropped, tunnel.highestBlock, clock);
            } else {
                // loop found, advance iteration
                const State& previous = loopDetection.at(hash);

                // determine loop size
                State loop = { rocksDropped - previous.rocksDropped, tunnel.highestBlock - previous.highestBlock, clock - previous.clock };
                cout << "loop detected with size of : " << loop.rocksDropped << endl;

                // calculate loops to go
                int64_t loopsToGo = (MAXIMUM_ROUNDS - rocksDropped - 1) / loop.rocksDropped;

                // advence iteration using the loop
                rocksDropped += (loopsToGo * loop.rocksDropped);
                loopedHeight += (loopsToGo * loop.highestBlock);
                clock += (loopsToGo * loop.clock);

                // clear loop detection cache
                loopDetection.clear();
            }
        }
    }

    // display highest block
    cout << "after round #" << MAXIMUM_ROUNDS << ", highest block in tunnel : " << (tunnel.highestBlock + loopedHeight) << endl;

    return 0;
}
