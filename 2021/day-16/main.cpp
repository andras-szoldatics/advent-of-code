#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <vector>

using namespace std;

// helper functions

vector<bool> convertFromHex(const string& hex) {
    vector<bool> bits;

    // convert hexadecimal to binary
    for (char c : hex) {
        // temporary storage
        uint8_t value;

        // convert to integer
        if ((c >= '0') &&
            (c <= '9')) {
            value = c - '0';
        } else if ((c >= 'A') &&
                   (c <= 'F')) {
            value = c - 'A' + 10;
        }

        for (int i = 3; i >= 0; i--) {
            bits.push_back(value & (1 << i));
        }
    }

    return bits;
}

uint64_t getInteger(const vector<bool>& bits,
                    size_t startIndex,
                    size_t width) {
    // temporary storage
    uint64_t value = 0;

    // iterate over bits
    for (size_t i = 0; i < width; i++) {
        // shift value
        value <<= 1;

        // add bit
        value |= bits.at(startIndex + i);
    }

    return value;
}

class Packet {
public:
    // header
    uint64_t version;
    uint64_t typeId;

    // contained data
    optional<uint64_t> data;
    vector<Packet> subPackets;

    Packet() :
        version(0),
        typeId(0) {}

    // parse functions
    size_t readPacket(const vector<bool>& bits,
                      size_t start = 0) {
        // temporary storage
        size_t cursor = start;

        // 3 bits as version
        version = getInteger(bits, cursor, 3);
        cursor += 3;

        // 3 bits as type
        typeId = getInteger(bits, cursor, 3);
        cursor += 3;

        // check for literal value
        if (typeId == 4) {
            cursor += readLiteralValue(bits, cursor);
        } else {
            cursor += readSubPackets(bits, cursor);
        }

        return (cursor - start);
    }

    // utility functions
    uint64_t getTotalVersion() {
        // temporary storage
        uint64_t sum = version;

        // add sub-packets
        for (Packet& p : subPackets) {
            sum += p.getTotalVersion();
        }

        return sum;
    }

    uint64_t getValue() {
        // check for easy return
        if (data) {
            return *data;
        }

        // retrieve values
        vector<uint64_t> values(subPackets.size(), 0);
        for (size_t i = 0; i < subPackets.size(); i++) {
            values.at(i) = subPackets.at(i).getValue();
        }

        uint64_t value = 0;
        switch (typeId) {
        case 0: // sum operator
            value = accumulate(values.begin(), values.end(), 0ull);
            break;
        case 1: // product operator
            value = accumulate(values.begin(), values.end(), 1ull,
                               multiplies<uint64_t>());
            break;
        case 2: // minimum operator
            value = *min_element(values.begin(), values.end());
            break;
        case 3: // maximum operator
            value = *max_element(values.begin(), values.end());
            break;
        case 5: // greater than operator
            value = (values.at(0) > values.at(1) ? 1 : 0);
            break;
        case 6: // less than operator
            value = (values.at(0) < values.at(1) ? 1 : 0);
            break;
        case 7: // equal to operator
            value = (values.at(0) == values.at(1) ? 1 : 0);
            break;
        }

        return value;
    }

private:
    // utility funcitons
    size_t readLiteralValue(const vector<bool>& bits,
                            size_t start = 6) {
        // temporary storage
        bool inBlock = true;
        uint64_t tmp = 0;
        size_t cursor = start;

        // read data
        while (inBlock) {
            // check if this is the last block
            inBlock = bits.at(cursor);
            cursor += 1;

            // gather bits into integer
            tmp <<= 4;
            tmp |= getInteger(bits, cursor, 4);
            cursor += 4;
        }

        // store data
        data = tmp;

        return (cursor - start);
    }

    size_t readSubPackets(const vector<bool>& bits,
                          size_t start = 6) {
        // temporary storage
        size_t cursor = start;

        // read type
        bool type = bits.at(cursor);
        cursor += 1;

        // separate types
        if (type) {
            // read number of sub-packets
            uint64_t number = getInteger(bits, cursor, 11);
            cursor += 11;

            for (uint64_t p = 0; p < number; p++) {
                // add new sub-packet
                subPackets.emplace_back();
                cursor += subPackets.back().readPacket(bits, cursor);
            }
        } else {
            // read bit width of sub-packets
            uint64_t width = getInteger(bits, cursor, 15);
            cursor += 15;

            while (width > 0) {
                // add new sub-packet
                subPackets.emplace_back();

                uint64_t readBits = subPackets.back().readPacket(bits, cursor);
                if (readBits <= width) {
                    width -= readBits;
                } else {
                    width = 0;
                }

                // shift cursor
                cursor += readBits;
            }
        }

        return (cursor - start);
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");
    getline(input, line);

    // convert to bits
    vector<bool> bits = convertFromHex(line);

    // initialize as packet
    Packet packet;
    packet.readPacket(bits);

    // display version sum
    uint64_t totalVersion = packet.getTotalVersion();
    cout << "[first] version sum of packet: " << totalVersion << endl;

    // display value
    uint64_t finalValue = packet.getValue();
    cout << "[second] value of packet: " << finalValue << endl;

    return 0;
}
