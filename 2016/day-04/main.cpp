#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

typedef pair<size_t, char> OrderedEntry;

bool comparison(const OrderedEntry& rhs,
                const OrderedEntry& lhs) {
    if (rhs.first > lhs.first) {
        return true;
    }

    if (rhs.first < lhs.first) {
        return false;
    }

    return (rhs.second < lhs.second);
}

class Room {
public:
    // simple attributes
    string codedName;
    int64_t sectorId;
    string checksum;

    // calculated attributes
    string realName;

    Room(const string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // read first part
        getline(buffer, tmp, '[');
        size_t lastDash = tmp.find_last_of('-');
        codedName = tmp.substr(0, lastDash);
        tmp = tmp.substr(lastDash + 1);
        sectorId = stoll(tmp);

        // read checksum
        getline(buffer, checksum, ']');
    }

    // utility functions
    string calculateCheckSum() const {
        // temporary storage
        unordered_map<char, size_t> characters;
        vector<pair<size_t, char>> ordered;

        // iterate over name
        for (char c : codedName) {
            if (c != '-') {
                // count characters
                if (characters.count(c) != 0) {
                    characters.at(c) += 1;
                } else {
                    characters[c] = 1;
                }
            }
        }

        // iterate over final pairs
        for (const pair<const char, size_t>& character : characters) {
            // temporary storage
            OrderedEntry entry = make_pair(character.second, character.first);

            // add to vector
            ordered.push_back(entry);
        }

        // sort vector
        sort(ordered.begin(), ordered.end(), comparison);

        // assemble checksum
        string checkSum;

        for (const OrderedEntry& entry : ordered) {
            // add char value as it is already in order
            checkSum.push_back(entry.second);

            // check early termination
            if (checkSum.size() == 5) {
                break;
            }
        }

        return checkSum;
    }

    void decodeName() {
        // wipe state
        realName.clear();

        // iterate over coded name
        for (char c : codedName) {
            // separate cases
            if (c == '-') {
                realName.push_back(' ');
            } else {
                // determine shift and execute it
                int64_t shift = (sectorId % 26);
                while (shift-- > 0) {
                    c++;
                }

                // wrap-around values
                c -= 'a';
                c %= 26;
                c += 'a';

                // add to real name
                realName.push_back(c);
            }
        }
    }
};

int main() {
    // temporary storage
    string line, tmp;

    // all the rooms in order
    vector<Room> rooms;

    // total of real rooms
    int64_t sumOfSectorIds = 0;
    int64_t sectorIdOfTarget = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assemble room from line
        Room room(line);

        // keep a running tally
        if (room.calculateCheckSum() == room.checksum) {
            // add sector id to total
            sumOfSectorIds += room.sectorId;

            // decode name of room
            room.decodeName();

            // check for specific room
            if (room.realName == string("northpole object storage")) {
                sectorIdOfTarget = room.sectorId;
            }
        }

        // add to list
        rooms.push_back(room);
    }

    // display results
    cout << "[first] total sector IDs of real rooms : " << sumOfSectorIds << endl;
    cout << "[second] sector ID of North Pole objects room : " << sectorIdOfTarget << endl;

    return 0;
}
