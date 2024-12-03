#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>

using namespace std;

class Card {
public:
    // card id
    size_t id;

    // numbers to match
    vector<int64_t> first;
    vector<int64_t> second;

    // number of copies available
    int64_t copies;

    Card() :
        id(0),
        copies(1) {}

    // utility functions
    int64_t getCopies() const {
        // temporary storage
        int64_t copies = 0;

        // iterate over pairs of numbers
        for (int64_t f : first) {
            for (int64_t s : second) {
                // add to result
                if (f == s) {
                    copies += 1;
                }
            }
        }

        return copies;
    }

    int64_t getScore() const {
        // temporary storage
        int64_t copies = getCopies();

        if (copies == 0) {
            return 0;
        }

        return 1 << (copies - 1);
    }
};

vector<int64_t> splitAndConvert(const string& group) {
    // temporary storage
    vector<int64_t> result;
    string remainder = group;

    while (!remainder.empty()) {
        // find next space
        size_t pos = remainder.find(' ');
        string part = remainder.substr(0, pos);

        if (pos != string::npos) {
            // remove part from remainder
            remainder = remainder.substr(pos + 1);
        } else {
            // remove part from remainder
            remainder.clear();
        }

        if (part.empty()) {
            continue;
        }

        // try to convert to integer
        try {
            int64_t number = stoll(part);
            result.push_back(number);
        } catch (...) {
            // NO-OP
        }
    }

    return result;
}

Card parseCard(const std::string& line) {
    // assing to buffer
    stringstream buffer(line);
    string tmp;

    // temporary storage
    Card result;

    // read id
    getline(buffer, tmp, ' ');
    getline(buffer, tmp, ':');
    result.id = stoull(tmp);

    // read first group of numbers
    getline(buffer, tmp, '|');
    result.first = splitAndConvert(tmp);

    // read second group of numbers
    getline(buffer, tmp);
    result.second = splitAndConvert(tmp);

    return result;
}

int main() {
    // temporary storage
    string line;

    // setup for iteration
    unordered_map<size_t, Card> cards;

    int64_t firstTotal = 0;
    int64_t secondTotal = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse game
        Card card = parseCard(line);

        // store card information
        cards[card.id] = card;

        // calculate score
        firstTotal += card.getScore();
    }

    // display progress
    cout << "[first] total of card score : " << firstTotal << endl;

    // process winnings
    for (size_t c = 1; c <= cards.size(); c++) {
        // retrieve card
        Card& card = cards.at(c);

        // add current copies to total
        secondTotal += card.copies;

        // generate more copies
        int64_t copies = card.getCopies();
        size_t fromIndex = card.id + 1;
        size_t toIndex = card.id + copies;

        // iterate over cards further down the line
        for (size_t i = fromIndex; i <= toIndex; i++) {
            // retrieve card
            Card& other = cards.at(i);

            // add copies
            other.copies += card.copies;
        }
    }

    // display progress
    cout << "[second] total cards won : " << secondTotal << endl;

    return 0;
}
