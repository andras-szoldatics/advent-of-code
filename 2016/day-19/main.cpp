#include <cstdint>
#include <iostream>
#include <list>

using namespace std;

size_t runFirstGame(list<size_t>& game) {
    if (game.empty()) {
        return 0;
    }

    // retrieve iterator to first elf
    list<size_t>::iterator elf = game.begin();

    while (game.size() > 1) {
        // move iterator to next elf
        elf++;

        // wrap-around
        if (elf == game.end()) {
            elf = game.begin();
        }

        // remove elf
        elf = game.erase(elf);

        // wrap-around
        if (elf == game.end()) {
            elf = game.begin();
        }
    }

    return game.front();
}

size_t runSecondGame(list<size_t>& game) {
    if (game.empty()) {
        return 0;
    }

    // retrieve iterator to first elf
    list<size_t>::iterator elf = game.begin();

    // advance iterator to half-way point
    advance(elf, game.size() / 2);

    while (game.size() > 1) {
        // remove elf
        elf = game.erase(elf);

        // wrap-around
        if (elf == game.end()) {
            elf = game.begin();
        }

        // move iterator to next elf
        if (game.size() % 2 == 0) {
            elf++;

            // wrap-around
            if (elf == game.end()) {
                elf = game.begin();
            }
        }
    }

    return game.front();
}

int main() {
    // problem input
    size_t elves = 3001330;

    // setup lists for processing
    list<size_t> firstGame;
    list<size_t> secondGame;

    // setup lists for input
    for (size_t i = 0; i < elves; i++) {
        // create new elf
        firstGame.push_back(i + 1);
        secondGame.push_back(i + 1);
    }

    // run games
    size_t firstWinner = runFirstGame(firstGame);
    cout << "[first] winning elf: " << firstWinner << endl;

    size_t secondWinner = runSecondGame(secondGame);
    cout << "[second] winning elf: " << secondWinner << endl;

    return 0;
}
