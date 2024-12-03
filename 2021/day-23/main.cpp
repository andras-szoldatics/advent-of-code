#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

enum class Type : uint8_t {
    EMPTY = 0,
    POD_A = 1,
    POD_B = 2,
    POD_C = 3,
    POD_D = 4
};

const array<Type, 4> TYPES = {
    Type::POD_A,
    Type::POD_B,
    Type::POD_C,
    Type::POD_D
};

const unordered_map<Type, int64_t> ENERGY_FOR_TYPE = {
    { Type::EMPTY, 0 },
    { Type::POD_A, 1 },
    { Type::POD_B, 10 },
    { Type::POD_C, 100 },
    { Type::POD_D, 1000 }
};

const size_t CORRIDOR_SPACE = 11;
const size_t BURROW_SPACE = 16;

const unordered_set<size_t> ENTRANCES = {
    2,
    4,
    6,
    8
};

typedef array<Type, CORRIDOR_SPACE + BURROW_SPACE> Space;
typedef pair<size_t, size_t> Hash;

namespace std {

template<>
struct hash<Hash> {
    size_t operator()(const Hash& entry) const {
        return hash<size_t>() (entry.first) +
               hash<size_t>() (entry.second);
    }
};

}

class State {
public:
    // play space
    Space space;

    // optimization attributes
    int64_t energy;
    int64_t steps;

    State(const string& line = {}) :
        energy(0),
        steps(0) {
        // fill space with empty cells
        space.fill(Type::EMPTY);
        size_t i = 0;

        // fill space with input
        for (; i < line.size(); i++) {
            // retrieve short-hands
            char c = line.at(i);
            if ((c >= 'A') && (c <= 'D')) {
                space.at(i) = static_cast<Type>(line.at(i) - 'A' + 1);
            }
        }

        // handle missing input
        if (i < CORRIDOR_SPACE) {
            i = CORRIDOR_SPACE;
        }

        // finish filling burrows
        for (; i < CORRIDOR_SPACE + BURROW_SPACE; i++) {
            space.at(i) = static_cast<Type>((i - CORRIDOR_SPACE) % 4 + 1);
        }
    }

    // helper functions
    Hash getHash() const {
        // temporary strogae
        Hash hash = { 0, 0 };

        // pack space into two integers
        for (size_t i = 0; i < CORRIDOR_SPACE; i++) {
            hash.first <<= 3;
            hash.first |= static_cast<size_t>(space.at(i));
        }

        for (size_t i = CORRIDOR_SPACE; i < CORRIDOR_SPACE + BURROW_SPACE; i++) {
            hash.second <<= 3;
            hash.second |= static_cast<size_t>(space.at(i));
        }

        return hash;
    }

    size_t getBurrowIndex(Type type,
                          size_t index) const {
        size_t target = static_cast<size_t>(type) - 1;
        target += (4 - index) * 4;
        target += CORRIDOR_SPACE;

        return target;
    }

    size_t getBurrowOccupants(Type type) const {
        if (type == Type::EMPTY) {
            return 0;
        }

        // temporary storage
        size_t delta = static_cast<size_t>(type) - 1;
        size_t start = CORRIDOR_SPACE + delta;

        // iterate over burrows
        for (size_t cursor = 0; cursor < 4; cursor++) {
            if (space.at(start + cursor * 4) != Type::EMPTY) {
                return (4 - cursor);
            }
        }

        return 0;
    }

    // utility functions
    vector<State> getBranches() const {
        vector<State> branches;

        // check for moving amphipods in
        for (size_t fromCorridor = 0; fromCorridor < CORRIDOR_SPACE; fromCorridor++) {
            // retrieve short-hands
            Type type = space.at(fromCorridor);
            if (type == Type::EMPTY) {
                continue;
            }

            // check for occupants in burrow
            size_t occupants = getBurrowOccupants(type);
            if (occupants == 4) {
                continue;
            }

            // setup for iteration
            bool blocked = false;

            // check current occupants
            for (size_t o = occupants; o > 0; o--) {
                size_t checkSpace = getBurrowIndex(type, o);

                if (space.at(checkSpace) != type) {
                    blocked = true;
                }
            }

            // check if blocked
            if (blocked) {
                continue;
            }

            // calculate target entrance
            size_t toCorridor = static_cast<size_t>(type) * 2;
            size_t min = std::min<size_t>(fromCorridor, toCorridor);
            size_t max = std::max<size_t>(fromCorridor, toCorridor);

            for (size_t check = min; check <= max; check++) {
                if (check == fromCorridor) {
                    continue;
                }

                if (space.at(check) != Type::EMPTY) {
                    blocked = true;
                }
            }

            // check if blocked
            if (blocked) {
                continue;
            }

            // calculate exact path length
            size_t pathLength = max - min;
            pathLength += (4 - occupants);

            // create branch
            State branch = *this;

            // swap types
            size_t finalSpace = getBurrowIndex(type, occupants + 1);
            swap(branch.space.at(fromCorridor), branch.space.at(finalSpace));

            // update energy & steps
            branch.energy += ENERGY_FOR_TYPE.at(type) * static_cast<int64_t>(pathLength);
            branch.steps += 1;

            // add branch
            branches.push_back(branch);
        }

        // check for moving amphipods out
        for (Type type : TYPES) {
            // check for number of occupants
            size_t occupants = getBurrowOccupants(type);
            if (occupants == 0) {
                continue;
            }

            // check the type of the top amphipod
            size_t fromIndex = getBurrowIndex(type, occupants);
            Type fromType = space.at(fromIndex);
            if (fromType == type) {
                // setup for iteration
                bool blocked = true;

                // check remaining occupants
                for (size_t o = occupants - 1; o > 0; o--) {
                    // retrieve short-hands
                    size_t targetIndex = getBurrowIndex(type, o);

                    // check if blocked
                    if (space.at(targetIndex) != type) {
                        blocked = false;
                    }
                }

                if (blocked) {
                    continue;
                }
            }

            // calculate entrance index
            size_t fromCorridor = static_cast<size_t>(type) * 2;

            // check corridor downside
            for (size_t i = fromCorridor; i > 0; i--) {
                // retrieve short-hands
                size_t targetIndex = i - 1;

                // skip entrances
                if (ENTRANCES.count(targetIndex) != 0) {
                    continue;
                }

                if (space.at(targetIndex) == Type::EMPTY) {
                    // create branch
                    State branch = *this;

                    // calculate exact path length
                    size_t pathLength = fromCorridor - targetIndex;
                    pathLength += (4 - occupants) + 1;

                    // swap types
                    swap(branch.space.at(fromIndex), branch.space.at(targetIndex));

                    // update energy & steps
                    branch.energy += ENERGY_FOR_TYPE.at(fromType) * static_cast<int64_t>(pathLength);
                    branch.steps += 1;

                    // add branch
                    branches.push_back(branch);
                } else {
                    break;
                }
            }

            // check corridor upside
            for (size_t i = fromCorridor + 1; i < CORRIDOR_SPACE; i++) {
                // retrieve short-hands
                size_t targetIndex = i;

                // skip entrances
                if (ENTRANCES.count(targetIndex) != 0) {
                    continue;
                }

                if (space.at(targetIndex) == Type::EMPTY) {
                    // create branch
                    State branch = *this;

                    // calculate exact path length
                    size_t pathLength = targetIndex - fromCorridor;
                    pathLength += (4 - occupants) + 1;

                    // swap types
                    swap(branch.space.at(fromIndex), branch.space.at(targetIndex));

                    // update energy & steps
                    branch.energy += ENERGY_FOR_TYPE.at(fromType) * static_cast<int64_t>(pathLength);
                    branch.steps += 1;

                    // add branch
                    branches.push_back(branch);
                } else {
                    break;
                }
            }
        }

        return branches;
    }

    // operator functions
    bool operator>(const State& other) const {
        return energy > other.energy;
    }
};

int64_t aStar(const string& startLine) {
    // create target hash
    State start(startLine);
    Hash target = State().getHash();

    // setup for iteration
    unordered_set<Hash> visited;
    priority_queue<State, vector<State>, greater<State>> queue;

    // add start state
    queue.push(start);

    // iterate until queue is empty
    while (!queue.empty()) {
        // retrieve short-hands
        State current = queue.top();
        queue.pop();

        // check if already visited
        Hash hash = current.getHash();
        if (visited.count(hash) != 0) {
            continue;
        }

        // check if target
        if (hash == target) {
            return current.energy;
        }

        // add to visited
        visited.insert(hash);

        // add branches
        vector<State> branches = current.getBranches();
        for (State branch : branches) {
            queue.push(branch);
        }
    }

    return -1;
}

int main() {
    // amphipods in order
    string firstAmphipods;

    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        for (char c : line) {
            if (((c >= 'A') &&
                 (c <= 'D')) ||
                (c == '.')) {
                firstAmphipods.push_back(c);
            }
        }
    }

    // solve original problem
    int64_t firstResult = aStar(firstAmphipods);

    // display result
    cout << "[first] energy required : " << firstResult << endl;

    // solver extended problem
    string secondAmphipods = firstAmphipods;
    secondAmphipods.insert(15, string("DCBADBAC"));
    int64_t secondResult = aStar(secondAmphipods);

    // display result
    cout << "[second] energy required: " << secondResult << endl;

    return 0;
}
