#include <iostream>

using namespace std;

void dragonCurve(string& input) {
    // store original size
    size_t originalSize = input.size();

    // append separator
    input.push_back('0');

    for (size_t i = 0; i < originalSize; ++i) {
        // retrieve original character
        char c = input.at(originalSize - i - 1);

        // flip and append
        input.push_back(c == '0' ? '1' : '0');
    }
}

string checkSum(const string& input) {
    // checksum buffer
    string checksum;
    checksum.reserve(input.size() / 2);

    // compute checksum
    for (size_t i = 0; i < input.size(); i += 2) {
        // retrieve characters
        char c1 = input.at(i);
        char c2 = input.at(i + 1);

        // check for equality
        checksum.push_back(c1 == c2 ? '1' : '0');
    }

    // return checksum
    return checksum;
}

string coverTracks(const string& input,
                   size_t targetSize) {
    // temporary storage
    string copy = input;
    copy.reserve(targetSize * 2);

    // expand until target size is reached
    while (copy.size() < targetSize) {
        dragonCurve(copy);
    }

    // truncate to exact size
    copy.resize(targetSize);

    // compute checksum
    string checksum = checkSum(copy);
    while (checksum.size() % 2 == 0) {
        checksum = checkSum(checksum);
    }

    return checksum;
}

int main() {
    // set input
    string input("11101000110010100");

    // attempt to cover tracks
    string firstResult = coverTracks(input, 272);
    cout << "[first] checksum : " << firstResult << "\n";

    string secondResult = coverTracks(input, 35651584);
    cout << "[second] checksum : " << secondResult << "\n";

    return 0;
}
