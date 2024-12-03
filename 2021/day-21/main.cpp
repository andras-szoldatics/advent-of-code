#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

const int64_t TABLE_SIZE = 10;

class FixedDie {
public:
    int64_t delta(int64_t numberOfGoes) {
        int64_t total = (numberOfGoes * 9) - 3;

        return total % TABLE_SIZE;
    }
};

class Player {
public:
    int64_t position;
    int64_t score;

    Player(int64_t _position) :
        position(_position),
        score(0) {}

    // utility functions
    void move(int64_t delta) {
        position += delta;

        while (position > TABLE_SIZE) {
            position -= TABLE_SIZE;
        }

        score += position;
    }

    // operator functions
    bool operator==(const Player& other) const {
        return (position == other.position) &&
               (score == other.score);
    }
};

int64_t playFirstGame(const Player& player1,
                      const Player& player2) {
    // crate copy of players
    array<Player, 2> players = { player1, player2 };

    // create die
    FixedDie die;

    // setup for iteration
    int64_t numberOfGoes = 0;
    size_t currentPlayer = 1;

    while (players.at(currentPlayer).score < 1000) {
        // switch players
        currentPlayer = (1 - currentPlayer);

        // one more go
        numberOfGoes += 1;

        // roll the die
        int64_t delta = die.delta(numberOfGoes);

        // move current player
        players.at(currentPlayer).move(delta);
    }

    return players.at(1 - currentPlayer).score * (numberOfGoes * 3);
}

class DiracState {
public:
    array<Player, 2> players;
    uint8_t currentPlayer;

    bool operator==(const DiracState& other) const {
        return (players == other.players) &&
               (currentPlayer == other.currentPlayer);
    }
};

namespace std {
template<>
struct hash<DiracState> {
    size_t operator()(const DiracState& state) const {
        size_t hash = state.players.at(0).position;

        hash <<= 5;
        hash += state.players.at(0).score;

        hash <<= 4;
        hash += state.players.at(1).position;

        hash <<= 5;
        hash += state.players.at(1).score;

        hash <<= 2;
        hash += state.currentPlayer;

        return hash;
    }
};

}

const array<pair<int64_t, int64_t>, 7> DIRAC_DICE = { {
    { 3, 1 },
    { 4, 3 },
    { 5, 6 },
    { 6, 7 },
    { 7, 6 },
    { 8, 3 },
    { 9, 1 }
} };

typedef pair<int64_t, int64_t> Wins;
typedef unordered_map<DiracState, Wins> DiracCache;

Wins countDiracWins(const DiracState& state,
                    DiracCache& cache) {
    // check if first player won
    if (state.players.at(0).score >= 21) {
        return { 1, 0 };
    }

    // check if second player won
    if (state.players.at(1).score >= 21) {
        return { 0, 1 };
    }

    // setup for iteration
    Wins totalWins = { 0, 0 };

    // iterate over dice
    for (const pair<int64_t, int64_t>& roll : DIRAC_DICE) {
        // make a copy of current state
        DiracState nextState = state;

        // switch to next player
        nextState.currentPlayer = (1 - nextState.currentPlayer);

        // move current player
        nextState.players.at(nextState.currentPlayer).move(roll.first);

        Wins nextWins = { 0, 0 };
        // check if state is already cached
        if (cache.count(nextState) != 0) {
            // retrieve wins from cache
            nextWins = cache.at(nextState);
        } else {
            // calculate wins for next state
            nextWins = countDiracWins(nextState, cache);

            // cache wins, for next time
            cache[nextState] = nextWins;
        }

        // account for frequency
        totalWins.first += nextWins.first * roll.second;
        totalWins.second += nextWins.second * roll.second;
    }

    return totalWins;
}

const size_t TOTAL_STATES = static_cast<size_t>(22 * 10 * 22 * 10 * 2);

int64_t playSecondGame(const Player& player1,
                       const Player& player2) {
    // setup dirac state
    DiracState state = {
        { player1, player2 },
        1,
    };

    // setup cache
    DiracCache cache;
    cache.reserve(TOTAL_STATES);

    // start recursion
    Wins wins = countDiracWins(state, cache);

    return max(wins.first, wins.second);
}

int main() {
    // temporary storage
    string line, tmp;

    // read input file
    ifstream input("input.txt");

    // create first player
    getline(input, line);
    tmp = line.substr(line.find(':') + 1);
    Player player1(stoll(tmp));

    // create second player
    getline(input, line);
    tmp = line.substr(line.find(':') + 1);
    Player player2(stoll(tmp));

    // play first game
    int64_t firstResult = playFirstGame(player1, player2);

    // display results
    cout << "[first] result of first game: " << firstResult << endl;

    // play second game
    int64_t secondResult = playSecondGame(player1, player2);

    // display results
    cout << "[second] result of second game: " << secondResult << endl;

    return 0;
}
