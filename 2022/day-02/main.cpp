#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

enum class Move : int64_t {
    UNDEFINED = 0,
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3
};

// utility functions for moves

Move getMoveABC(char c) {
    switch (c) {
    case 'A':
        return Move::ROCK;
        break;
    case 'B':
        return Move::PAPER;
        break;
    case 'C':
        return Move::SCISSORS;
        break;
    default:
        return Move::UNDEFINED;
    }
}

Move getMoveXYZ(char c) {
    switch (c) {
    case 'X':
        return Move::ROCK;
        break;
    case 'Y':
        return Move::PAPER;
        break;
    case 'Z':
        return Move::SCISSORS;
        break;
    default:
        return Move::UNDEFINED;
    }
}

Move getWinningMove(Move other) {
    switch (other) {
    case Move::ROCK:
        return Move::PAPER;
        break;
    case Move::PAPER:
        return Move::SCISSORS;
        break;
    case Move::SCISSORS:
        return Move::ROCK;
        break;
    default:
        (void) nullptr; //NO-OP;
    }

    return Move::UNDEFINED;
}

Move getEvenMove(Move other) {
    return other;
}

Move getLosingMove(Move other) {
    switch (other) {
    case Move::ROCK:
        return Move::SCISSORS;
        break;
    case Move::PAPER:
        return Move::ROCK;
        break;
    case Move::SCISSORS:
        return Move::PAPER;
        break;
    default:
        (void) nullptr; //NO-OP;
    }

    return Move::UNDEFINED;
}

class Game {
public:
    Move elf;
    Move me;

    Game() :
        elf(Move::UNDEFINED),
        me(Move::UNDEFINED) {}

    // parsing functions
    void parseFirst(const string& line) {
        // parse moves
        elf = getMoveABC(line.front());
        me = getMoveXYZ(line.back());
    }

    void parseSecond(const string& line) {
        // parse first move
        elf = getMoveABC(line.front());

        // select appropriate response
        switch (line.back()) {
        case 'X':
            me = getLosingMove(elf);
            break;
        case 'Y':
            me = getEvenMove(elf);
            break;
        case 'Z':
            me = getWinningMove(elf);
            break;
        default:
            (void) nullptr; // NO-OP
        }
    }

    // scoring functions
    ino64_t getScore() {
        int64_t score = static_cast<int64_t>(me);

        if (elf == me) {
            score += 3;
        } else if (getWinningMove(elf) == me) {
            score += 6;
        }

        return score;
    }
};

int main() {
    // temporary storage
    string line;

    // keep a running total
    int64_t totalFirstScore = 0;
    int64_t totalSecondScore = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Game firstGame;
        Game secondGame;

        // process line by line
        firstGame.parseFirst(line);
        int64_t scoreFirst = firstGame.getScore();

        secondGame.parseSecond(line);
        int64_t scoreSecond = secondGame.getScore();

        // add score to total
        totalFirstScore += scoreFirst;
        totalSecondScore += scoreSecond;
    }

    // display results
    cout << "first score : " << totalFirstScore << endl;
    cout << "second score : " << totalSecondScore << endl;

    return 0;
}
