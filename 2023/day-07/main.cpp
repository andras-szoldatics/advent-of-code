#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

enum class CamelPoker : int8_t {
    FiveOfAKind = 7,
    FourOfAKind = 6,
    FullHouse = 5,
    ThreeOfAKind = 4,
    TwoPair = 3,
    OnePair = 2,
    HighCard = 1,
    Bust = 0
};

class Hand {
public:
    vector<int64_t> cards;
    CamelPoker value;
    int64_t bet;

    Hand(const string& line) :
        value(CamelPoker::Bust) {
        // parse line
        stringstream buffer(line);
        string tmp;

        getline(buffer, tmp, ' ');
        for (char c : tmp) {
            // parse cards
            int64_t card = parseCard(c);
            if (card > 0) {
                cards.push_back(card);
            }
        }

        getline(buffer, tmp);
        bet = stoll(tmp);

        // evaluate current hand
        evaluateStandard();
    }

    // utility functions
    void evaluateStandard() {
        // re-work hand into a map
        unordered_map<int64_t, size_t> cardCount;
        for (int64_t card : cards) {
            cardCount[card] += 1;
        }

        // reduce values into vector
        vector<size_t> matches;
        for (const pair<const int64_t, size_t>& entry : cardCount) {
            if (entry.second > 1) {
                matches.push_back(entry.second);
            }
        }

        // classify matches
        classifyMatches(matches);
    }

    void evaluateAsJokers() {
        // temporary storage
        size_t jokers = 0;

        // re-work hand into a map
        unordered_map<int64_t, size_t> cardCount;
        for (int64_t& card : cards) {
            // change 11 cards to 0
            if (card == 11) {
                card = 0;
            }

            cardCount[card] += 1;
        }

        // reduce values into vector
        vector<size_t> matches;
        for (const pair<const int64_t, size_t>& entry : cardCount) {
            if (entry.first == 0) {
                jokers = entry.second;
            } else if (entry.second > 1) {
                matches.push_back(entry.second);
            }
        }

        // accomodate jokers
        if (matches.empty()) {
            // add jokers matches
            matches.push_back(jokers + 1);
        } else {
            // sort matches
            sort(matches.begin(), matches.end());

            // increase best match by jokers
            matches.back() += jokers;
        }

        // classify matches
        classifyMatches(matches);
    }

    // operator functions
    bool operator<(const Hand& other) const {
        if (value == other.value) {
            return cards < other.cards;
        }

        return value < other.value;
    }

private:
    // utility functions
    static int64_t parseCard(char c) {
        switch (c) {
        case 'A':
            return 14;
        case 'K':
            return 13;
        case 'Q':
            return 12;
        case 'J':
            return 11;
        case 'T':
            return 10;
        default:
            if (isdigit(c)) {
                return static_cast<int64_t>(c - '0');
            }
        }

        return 0;
    }

    void classifyMatches(const vector<size_t>& matches) {        
        // classify matches
        if (matches.empty()) {
            value = CamelPoker::HighCard;
        } else if (matches.size() == 1) {
            switch (matches.front()) {
            case 5:
                value = CamelPoker::FiveOfAKind;
                break;
            case 4:
                value = CamelPoker::FourOfAKind;
                break;
            case 3:
                value = CamelPoker::ThreeOfAKind;
                break;
            case 2:
                value = CamelPoker::OnePair;
                break;
            }
        } else if (matches.size() == 2) {
            if ((matches.front() == 3) ||
                (matches.back() == 3)) {
                value = CamelPoker::FullHouse;
            } else {
                value = CamelPoker::TwoPair;
            }
        }
    }
};

int main() {
    // temporary storage
    string line;

    // store hands of poker
    vector<Hand> firstHands;
    vector<Hand> secondHands;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse line as a hand
        Hand originalHand(line);
        firstHands.push_back(originalHand);

        // try to substitute jokers
        Hand jokerHand(line);
        jokerHand.evaluateAsJokers();
        secondHands.push_back(jokerHand);
    }

    // sort hands
    sort(firstHands.begin(), firstHands.end());
    sort(secondHands.begin(), secondHands.end());

    // setup for iteration
    int64_t firstWinnings = 0;
    for (size_t i = 0; i < firstHands.size(); i++) {
        firstWinnings += (firstHands.at(i).bet * static_cast<int64_t>(i + 1));
    }

    // display results
    cout << "[first] total winnings: " << firstWinnings << endl;

    // setup for iteration
    int64_t secondWinnings = 0;
    for (size_t i = 0; i < secondHands.size(); i++) {
        secondWinnings += (secondHands.at(i).bet * static_cast<int64_t>(i + 1));
    }

    // display results
    cout << "[second] total winnings: " << secondWinnings << endl;

    return 0;
}
