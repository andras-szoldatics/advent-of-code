// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

using namespace std;

typedef nullptr_t Empty;

typedef int64_t Value;

class Packet;
typedef vector<Packet> Packets;

class Packet {
public:
    // storage of values
    string originalEntry;
    variant<Empty, Value, Packets> content;

    Packet(const string& entry) :
        originalEntry(entry) {
        // check entry
        if (entry.empty()) {
            // empty packet
            content = Empty();
        } else if (entry.find('[') == string::npos) {
            // value packet
            content = stoll(entry);
        } else {
            // temporary storage
            Packets packets;
            string trimmedEntry = entry;

            // move inside brackets
            trimmedEntry.pop_back();
            trimmedEntry.erase(0, 1);

            // check if anything remains
            if (trimmedEntry.empty()) {
                // create empty packet
                Packet empty(trimmedEntry);

                // add packet to list
                packets.push_back(empty);
            } else {
                // temporary storage
                vector<string> tmps;
                tmps.push_back({});

                int64_t brackets = 0;
                // iterate over entry
                for (char c : trimmedEntry) {
                    // check for new sub-packet
                    if ((brackets == 0) &&
                        (c == ',')) {
                        tmps.push_back({});
                    } else {
                        tmps.back().push_back(c);
                    }

                    // check bracketing
                    if (c == '[') {
                        brackets++;
                    } else if (c == ']') {
                        brackets--;
                    }
                }

                for (const string& tmp : tmps)  {
                    // parse packets one-by-one
                    Packet packet(tmp);
                    packets.push_back(packet);
                }
            }

            // list packet
            content = packets;
        }
    }

    // compare functions functions
    int64_t comperator(const Packet& rhs) const {
        try {
            // this packet is empty
            if ((content.index() == 0) &&
                (rhs.content.index() == 0)) {
                return 0;
            } else if (content.index() == 0) {
                return -1;
            } else if (rhs.content.index() == 0) {
                return 1;
            }

            // this packet is a value
            if (content.index() == 1) {
                // retrieve short-hands
                const Value& left = get<Value>(content);

                if (rhs.content.index() == 1) {
                    // value vs value
                    const Value& right = get<Value>(rhs.content);

                    if (left == right) {
                        return 0;
                    } else if (left < right) {
                        return -1;
                    } else {
                        return 1;
                    }
                }

                if (rhs.content.index() == 2) {
                    // value vs array
                    const Packets& right = get<Packets>(rhs.content);

                    const Packet& firstRight = right.front();
                    int64_t subResult = comperator(firstRight);
                    // check sub result
                    if (subResult != 0) {
                        return subResult;
                    } else {
                        // check list size
                        if (right.size() >= 2) {
                            return -1;
                        } else {
                            return 0;
                        }
                    }
                }
            }

            // this packet is an array
            if (content.index() == 2) {
                // retrieve short-hands
                const Packets& left = get<Packets>(content);

                if (rhs.content.index() == 1) {
                    // array vs value
                    const Packet& firstLeft = left.front();
                    int64_t subResult = firstLeft.comperator(rhs);
                    // check sub result
                    if (subResult != 0) {
                        return subResult;
                    } else {
                        // check list size
                        if (left.size() >= 2) {
                            return 1;
                        } else {
                            return 0;
                        }
                    }
                }

                if (rhs.content.index() == 2) {
                    // array vs array
                    const Packets& right = get<Packets>(rhs.content);

                    // determine smaller size
                    size_t smallerSize = min(left.size(), right.size());

                    // iterate over smaller size
                    for (size_t i = 0; i < smallerSize; i++) {
                        // get subresult
                        int64_t subResult = left.at(i).comperator(right.at(i));

                        // if definite answer, use it
                        if (subResult != 0) {
                            return subResult;
                        }
                    }

                    // everything matched, judge the size
                    if (left.size() < right.size()) {
                        return -1;
                    } else if (left.size() > right.size()) {
                        return 1;
                    } else {
                        return 0;
                    }
                }
            }
        } catch (...) {
            return 0;
        }

        return 0;
    }

    // operator functions
    bool operator<(const Packet& rhs) const {
        return (comperator(rhs) == -1);
    }
};

int main() {
    // temporary storage
    string line;

    // storage of packets
    Packets packets;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // filter empty lines
        if (line.empty()) {
            continue;
        }

        // assemble packet
        Packet packet(line);

        // add to storage
        packets.push_back(packet);
    }

    int64_t goodPairs = 0;
    // iterate over packet pairs
    for (size_t i = 1; i < packets.size(); i += 2) {
        // retrieve short-hands
        const Packet& firstPacket = packets.at(i - 1);
        const Packet& secondPacket = packets.at(i);

        // check packet order
        if (firstPacket < secondPacket) {
            goodPairs += ((i + 1) / 2);
        }
    }

    // display results
    cout << "sum of index pairs : " << goodPairs << endl;

    // add two packets to the list
    Packet firstSplit("[[2]]");
    packets.push_back(firstSplit);

    Packet secondSplit("[[6]]");
    packets.push_back(secondSplit);

    // sort the list
    sort(packets.begin(), packets.end());

    int64_t firstIndex = 0;
    int64_t secondIndex = 0;
    // iterate over packets to look for our decoder keys
    for (size_t i = 0; i < packets.size(); i++) {
        // retrieve short-hands
        const Packet& packet = packets.at(i);

        // check for splits
        if (firstSplit.originalEntry == packet.originalEntry) {
            firstIndex = (i + 1);
        }

        if (secondSplit.originalEntry == packet.originalEntry) {
            secondIndex = (i + 1);
        }
    }

    // display results
    cout << "decoder key for distress signal : " << (firstIndex * secondIndex) << endl;

    return 0;
}
