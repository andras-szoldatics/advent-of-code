// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

const int64_t INVALID_ID = -1;
const int64_t INVALID_VALUE = -1;

class Bot {
public:
    // identifier
    int64_t id;

    // current state of bot
    array<int64_t, 2> values;

    // connections
    variant<nullptr_t, int64_t, size_t> lowTarget;
    variant<nullptr_t, int64_t, size_t> highTarget;

    // constructor
    Bot(int64_t id = INVALID_VALUE) :
        id(id),
        values({ INVALID_VALUE, INVALID_VALUE }),
        lowTarget(nullptr),
        highTarget(nullptr) {}

    // helper functions
    void setBotLow(int64_t bot) {
        lowTarget = bot;
    }

    void setBotHigh(int64_t bot) {
        highTarget = bot;
    }

    void setOutputLow(size_t output) {
        lowTarget = output;
    }

    void setOutputHigh(size_t output) {
        highTarget = output;
    }

    // utility functions
    bool isReadyToSort() const {
        // check if bot has two chips
        bool hasTwoChips = ((values.front() != INVALID_VALUE) &&
                            (values.back() != INVALID_VALUE));

        // check if bot has two valid targets
        bool hasTwoTargets = ((lowTarget.index() != 0) &&
                              (highTarget.index() != 0));

        // return result
        return (hasTwoChips && hasTwoTargets);
    }

    void addChip(int64_t value) {
        if (values.front() == INVALID_VALUE) {
            values.front() = value;
        } else if (values.back() == INVALID_VALUE) {
            values.back() = value;
        }
    }

    bool hasFirstChips() const {
        // check for specific chip ids
        bool hasFirstChips = (((values.front() == 17) && (values.back() == 61)) ||
                              ((values.front() == 61) && (values.back() == 17)));

        return hasFirstChips;
    }

    void sortChips(unordered_map<int64_t, Bot>& bots,
                   vector<int64_t>& outputs,
                   queue<int64_t>& readyBots) {
        if (!isReadyToSort()) {
            return;
        }

        // sort chips according to relative values
        int64_t lowValue = min(values.front(), values.back());
        int64_t highValue = max(values.front(), values.back());

        // handle low value's target
        if (lowTarget.index() == 1) {
            // retrieve short-hands
            int64_t lowBot = get<1>(lowTarget);

            if (bots.count(lowBot) != 0) {
                // pass down chip
                bots.at(lowBot).addChip(lowValue);

                // check if other bot is ready
                if (bots.at(lowBot).isReadyToSort()) {
                    readyBots.push(lowBot);
                }
            }
        } else if (lowTarget.index() == 2) {
            // retrieve short-hands
            size_t lowOutput = get<2>(lowTarget);

            if (lowOutput < outputs.size()) {
                // drop into output bin
                outputs.at(lowOutput) = lowValue;
            }
        }

        // handle high value's target
        if (highTarget.index() == 1) {
            // retrieve short-hands
            int64_t highBot = get<1>(highTarget);

            if (bots.count(highBot) != 0) {
                // pass down chip
                bots.at(highBot).addChip(highValue);

                // check if other bot is ready
                if (bots.at(highBot).isReadyToSort()) {
                    readyBots.push(highBot);
                }
            }
        } else if (highTarget.index() == 2) {
            // retrieve short-hands
            size_t highOutput = get<2>(highTarget);

            if (highOutput < outputs.size()) {
                // drop into output bin
                outputs.at(highOutput) = highValue;
            }
        }

        // clear values
        values.front() = INVALID_VALUE;
        values.back() = INVALID_VALUE;
    }
};

int64_t parseBotLine(unordered_map<int64_t, Bot>& bots,
                     vector<int64_t>& outputs,
                     stringstream& buffer) {
    // temporary storage
    int64_t botId = INVALID_ID;
    string word;

    // read bot id
    getline(buffer, word, ' ');
    botId = stoll(word);

    // check if bot already exists
    if (bots.count(botId) == 0) {
        bots[botId] = Bot(botId);
    }

    // read low target
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    if (word == "bot") {
        // read bot id
        getline(buffer, word, ' ');
        int64_t lowBot = stoll(word);

        // check if bot already exists
        if (bots.count(lowBot) == 0) {
            bots[lowBot] = Bot(lowBot);
        }

        // set low target
        bots.at(botId).setBotLow(lowBot);
    } else if (word == "output") {
        // read output id
        getline(buffer, word, ' ');
        size_t lowOutput = stoll(word);

        // check for output bin size
        if (outputs.size() <= lowOutput) {
            outputs.resize(lowOutput + 1, INVALID_VALUE);
        }

        // set low target
        bots.at(botId).setOutputLow(lowOutput);
    }

    // read high target
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    if (word == "bot") {
        // read bot id
        getline(buffer, word, ' ');
        int64_t highBot = stoll(word);

        // check if bot already exists
        if (bots.count(highBot) == 0) {
            bots[highBot] = Bot(highBot);
        }

        // set high target
        bots.at(botId).setBotHigh(highBot);
    } else if (word == "output") {
        // read output id
        getline(buffer, word, ' ');
        size_t highOutput = stoll(word);

        // check for output bin size
        if (outputs.size() <= highOutput) {
            outputs.resize(highOutput + 1, INVALID_VALUE);
        }

        // set high target
        bots.at(botId).setOutputHigh(highOutput);
    }

    return botId;
}

int64_t parseValueLine(unordered_map<int64_t, Bot>& bots,
                       stringstream& buffer) {
    // temporary storage
    int64_t botId = INVALID_ID;
    string word;

    // read chip value
    getline(buffer, word, ' ');
    int64_t value = stoll(word);

    // read bot id
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    getline(buffer, word, ' ');
    botId = stoll(word);

    // check if bot already exists
    if (bots.count(botId) == 0) {
        bots[botId] = Bot(botId);
    }

    // add chip to bot
    bots.at(botId).addChip(value);

    return botId;
}

int main() {
    // temporary storage
    string line;

    // storage for bots and outputs
    unordered_map<int64_t, Bot> bots;
    vector<int64_t> outputs;

    queue<int64_t> readyBots;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // assign line to buffer
        stringstream buffer(line);

        // temporary storage
        int64_t botId = INVALID_ID;
        string word;

        // read first word
        getline(buffer, word, ' ');

        // check if line is bot instruction
        if (word == "bot") {
            botId = parseBotLine(bots, outputs, buffer);
        } else if (word == "value") {
            botId = parseValueLine(bots, buffer);
        }

        if (botId != INVALID_ID) {
            // check if this bot is ready
            if (bots.at(botId).isReadyToSort()) {
                readyBots.push(botId);
            }
        }
    }

    // setup for iteration
    int64_t firstBotId = INVALID_ID;

    // iterate until all bots are done sorting
    while (!readyBots.empty()) {
        // retrieve bot id
        int64_t botId = readyBots.front();
        readyBots.pop();

        // retrieve short-hands
        Bot& bot = bots.at(botId);

        // check if this bot has the relevant chips
        if (firstBotId == INVALID_ID) {
            if (bot.hasFirstChips()) {
                firstBotId = botId;
            }
        }

        // sort chips
        bot.sortChips(bots, outputs, readyBots);
    }

    // calculate product of outputs
    int64_t secondProduct = 1;
    for (size_t i = 0; i < outputs.size(); i++) {
        secondProduct *= outputs.at(i);

        if (i == 2) {
            break;
        }
    }

    // display results
    cout << "[first] bot id that had the chips : " << firstBotId << endl;
    cout << "[second] product of the first three outputs : " << secondProduct << endl;

    return 0;
}
