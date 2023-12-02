#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Pull {
public:
    int64_t red;
    int64_t green;
    int64_t blue;

    Pull(int64_t r = 0,
         int64_t g = 0,
         int64_t b = 0) :
        red(r),
        green(g),
        blue(b) {}

    // utility functions
    bool isPossible(const Pull& bag) const {
        return (red <= bag.red) &&
               (green <= bag.green) &&
               (blue <= bag.blue);
    }

    int64_t getPower() const {
        return (red > 0 ? red : 1) *
               (green > 0 ? green : 1) *
               (blue > 0 ? blue : 1);
    }
};

class Game {
public:
    int64_t id;
    vector<Pull> pulls;

    Game() :
        id(0) {}

    // utility functions
    bool isPossible(const Pull& bag) const {
        // check all pulls
        for (const Pull& pull : pulls) {
            if (!pull.isPossible(bag)) {
                return false;
            }
        }

        return true;
    }

    int64_t getPower() const {
        // temporary storage
        Pull maximum;

        // find maximum
        for (const Pull& pull : pulls) {
            maximum.red = max(maximum.red, pull.red);
            maximum.green = max(maximum.green, pull.green);
            maximum.blue = max(maximum.blue, pull.blue);
        }

        return maximum.getPower();
    }
};

// parsing functions

Pull parsePull(const string& pull) {
    // temporary storage
    Pull result;

    // setup for parsing
    stringstream buffer(pull + ',');
    string tmp;

    // parse chunks
    getline(buffer, tmp, ',');

    while (!tmp.empty()) {
        // find last space and split
        size_t pos = tmp.rfind(' ');
        string number = tmp.substr(0, pos);
        string color = tmp.substr(pos + 1);

        // parse number
        int64_t value = stoll(number);
        if (color == string("red")) {
            result.red += value;
        } else if (color == string("green")) {
            result.green += value;
        } else if (color == string("blue")) {
            result.blue += value;
        }

        // get next chunk
        getline(buffer, tmp, ',');
    }

    return result;
}

Game parseGame(const string& line) {
    // temporary storage
    Game result;

    // setup for parsing
    stringstream buffer(line + ';');
    string tmp;

    // parse game id
    getline(buffer, tmp, ' ');
    getline(buffer, tmp, ':');
    result.id = stoll(tmp);

    // parse pulls
    getline(buffer, tmp, ';');

    while (!tmp.empty()) {
        // parse pull
        Pull pull = parsePull(tmp);
        result.pulls.push_back(pull);

        // get next pull
        getline(buffer, tmp, ';');
    }

    return result;
}

int main() {
    // temporary storage
    string line;

    // storage for games
    vector<Game> games;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse game
        Game game = parseGame(line);
        games.push_back(game);
    }

    // evaluate games
    int64_t firstResult = 0;
    int64_t secondResult = 0;

    const Pull BAG_LIMIT(12, 13, 14);
    for (const Game& game : games) {
        if (game.isPossible(BAG_LIMIT)) {
            firstResult += game.id;
        }

        secondResult += game.getPower();
    }

    // display result
    cout << "[first] total of possible games : " << firstResult << endl;
    cout << "[second] total power of games : " << secondResult << endl;

    return 0;
}
