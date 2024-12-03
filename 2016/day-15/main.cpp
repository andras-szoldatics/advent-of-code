#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Disc {
public:
    // attributes
    int64_t count;
    int64_t positions;
    int64_t filter;

    Disc(int64_t count,
         int64_t positions,
         int64_t atStart) :
        count(count),
        positions(positions),
        filter(atStart) {
        // adjust filter
        filter += count;
        filter %= positions;
    }

    // utility functions
    bool isMatch(int64_t time) const {
        // check if the disc is at the right position
        return (time + filter) % positions == 0;
    }
};

class Statue {
public:
    // all the discs in order
    vector<Disc> discs;

    int64_t findMatch() {
        // setup for iteration
        int64_t time = 0;
        int64_t step = 1;

        // iterate over discs in order, presume co-prime disc positions
        for (const Disc& disc : discs) {
            // find the next step that matches
            while (!disc.isMatch(time)) {
                // increment time
                time += step;
            }

            // update step
            step *= disc.positions;
        }

        return time;
    }
};

int main() {
    // temporary storage
    string line;

    // allocate storage
    Statue statue;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        istringstream buffer(line);

        // parse line
        getline(buffer, line, '#');
        getline(buffer, line, ' ');

        // get disc number
        int64_t discNumber = stoll(line);

        // parse line
        getline(buffer, line, ' ');
        getline(buffer, line, ' ');

        // get disc positions
        int64_t positions = stoll(line);

        // parse line
        getline(buffer, line, ',');
        getline(buffer, line, 'n');
        getline(buffer, line, '.');

        // get disc at start
        int64_t atStart = stoll(line);

        // create disc
        Disc disc(discNumber, positions, atStart);
        statue.discs.push_back(disc);
    }

    // find match for first part
    int64_t firstMatch = statue.findMatch();
    cout << "[first] first match: " << firstMatch << endl;

    // add disc
    Disc disc(7, 11, 0);
    statue.discs.push_back(disc);

    // find match for second part
    int64_t secondMatch = statue.findMatch();
    cout << "[second] second match: " << secondMatch << endl;

    return 0;
}
