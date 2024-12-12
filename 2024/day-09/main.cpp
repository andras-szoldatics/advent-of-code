#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

const size_t EMPTY = numeric_limits<size_t>::max();

class Block {
public:
    size_t index;
    size_t size;
};

void merge(vector<Block>& disk,
           size_t index) {
    for (size_t i = 0; i < 2;) {
        if (index + i >= disk.size()) {
            break;
        }

        // retrieve short-hands
        Block& first = disk.at(index + i - 1);
        Block& second = disk.at(index + i);

        if (first.index == second.index) {
            first.size += second.size;

            // remove the second block
            disk.erase(disk.begin() + index + i);
        } else {
            i += 1;
        }
    }
}

void fragment(vector<Block>& disk) {
    // setup for iteration
    size_t s = disk.size() - 1;

    for (size_t f = 0; f < s; f++) {
        // check for an empty space
        if (disk.at(f).index != EMPTY) {
            continue;
        }

        while ((f < s) &&
               (disk.at(s).index == EMPTY)) {
            // find the next value
            s -= 1;
        }

        if (f < s) {
            // retrieve short-hands
            Block& first = disk.at(f);
            Block& second = disk.at(s);

            if (first.size < second.size) {
                // partial move
                first.index = second.index;
                second.size -= first.size;

                // merge blocks
                merge(disk, f);
            } else if (first.size == second.size) {
                // exact swap in
                swap(first.index, second.index);

                // merge blocks
                merge(disk, f);
                merge(disk, s);
            } else if (first.size > second.size) {
                // calculate delta
                size_t delta = first.size - second.size;

                // full swap in
                first.index = second.index;
                first.size = second.size;

                // clear original
                second.index = EMPTY;

                // add empty block
                if (disk.at(f + 1).index == EMPTY) {
                    disk.at(f + 1).size += delta;
                } else {
                    disk.insert(disk.begin() + f + 1, { EMPTY, delta });
                }

                // merge blocks
                merge(disk, f + 1);
                merge(disk, s);
            }
        }

        // shift search index
        if (s >= disk.size()) {
            s = disk.size() - 1;
        }
    }
}

void defragment(vector<Block>& disk) {
    // setup for iteration
    size_t f = 0;

    for (size_t s = disk.size() - 1; s < disk.size(); s--) {
        // check for value
        if (disk.at(s).index == EMPTY) {
            continue;
        }

        // reset search index
        f = 0;

        bool searching = true;
        while (searching) {
            // check for early termination
            if (f >= s) {
                break;
            }

            // check for value
            if ((disk.at(f).index == EMPTY) &&
                (disk.at(f).size >= disk.at(s).size)) {
                searching = false;
            } else {
                f += 1;
            }
        }

        if (f < s) {
            // retrieve short-hands
            Block& first = disk.at(f);
            Block& second = disk.at(s);

            if (first.size == second.size) {
                // exact swap in
                swap(first.index, second.index);

                // merge blocks
                merge(disk, f);
                merge(disk, s);
            } else if (first.size > second.size) {
                // calculate delta
                size_t delta = first.size - second.size;

                // full swap in
                first.index = second.index;
                first.size = second.size;

                // clear original
                second.index = EMPTY;

                // add empty block
                if (disk.at(f + 1).index == EMPTY) {
                    disk.at(f + 1).size += delta;
                } else {
                    disk.insert(disk.begin() + f + 1, { EMPTY, delta });
                }

                // merge blocks
                merge(disk, f + 1);
                merge(disk, s);
            }
        }
    }
}

size_t checkSum(const vector<Block>& disk) {
    // setup for iteration
    size_t sum = 0;
    size_t index = 0;

    for (size_t i = 0; i < disk.size(); i++) {
        // retrieve short-hands
        const Block& block = disk.at(i);

        if (block.index != EMPTY) {
            for (size_t i = 0; i < block.size; i++) {
                sum += (block.index * index);

                index += 1;
            }
        } else {
            index += block.size;
        }

    }

    return sum;
}

int main() {
    // temporary storage
    string line;

    // setup lookup storage
    vector<Block> disk;
    size_t index = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // reserve disk space to reduce allocations
        disk.reserve(disk.size() + line.size());

        // iterate over each character
        for (char c : line) {
            size_t size = c - '0';

            if (index % 2 == 0) {
                disk.push_back({ index / 2, size });
            } else {
                disk.push_back({ EMPTY, size });
            }

            // shift index
            index += 1;
        }
    }

    // create clones of the backup
    vector<Block> disk1 = disk;
    vector<Block> disk2 = disk;

    // run fragmentation program
    fragment(disk1);
    size_t firstSum = checkSum(disk1);

    cout << "[first] filesystem checksum: " << firstSum << endl;

    // run defragmentation program
    defragment(disk2);
    size_t secondSum = checkSum(disk2);

    cout << "[second] filesystem checksum: " << secondSum << endl;

    return 0;
}
