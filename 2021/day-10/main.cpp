// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

using namespace std;

const unordered_map<char, int64_t> ERROR_SCORE = {
    { ')', 3 },
    { ']', 57 },
    { '}', 1197 },
    { '>', 25137 }
};

const unordered_map<char, int64_t> COMPLETION_SCORE = {
    { ')', 1 },
    { ']', 2 },
    { '}', 3 },
    { '>', 4 }
};

char getPairOf(char c) {
    switch (c) {
    case '(':
        return ')';
        break;
    case ')':
        return '(';
        break;
    case '[':
    case '{':
    case '<':
        return static_cast<char>(c + 2);
        break;
    case ']':
    case '}':
    case '>':
        return static_cast<char>(c - 2);
        break;
    }

    return c;
}

class Chunk {
public:
    // attributes
    string value;
    int64_t errorScore;

    Chunk(const string& _value) :
        value(_value),
        errorScore(0) {}

    // validation functions
    void validate() {
        bool error = false;
        // iterate over value
        for (char c : value) {
            // handle relevant cases
            switch (c) {
            case ')':
            case ']':
            case '}':
            case '>':
                // check for matching open bracket
                if (brackets.top() == getPairOf(c)) {
                    // close the bracket
                    brackets.pop();
                } else {
                    // mark the error
                    errorScore = ERROR_SCORE.at(c);
                    error = true;
                }
                break;
            default:
                brackets.push(c);
                break;
            }

            // check for termination
            if (error) {
                break;
            }
        }
    }

    int64_t getCompletionScore() {
        // check for error
        if (errorScore > 0) {
            return 0;
        }

        // temporary storage
        int64_t completionScore = 0;

        // empty the stack in order
        while (!brackets.empty()) {
            // retrieve matching character
            char matchingBracket = getPairOf(brackets.top());
            brackets.pop();

            // fill out score
            completionScore *= 5;
            completionScore += COMPLETION_SCORE.at(matchingBracket);
        }

        return completionScore;
    }

private:
    // stack to store brackets
    stack<char> brackets;
};

int main() {
    // temporary storage
    string line;

    // stored chunks
    vector<Chunk> chunks;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line into a chunk
        Chunk chunk(line);

        // validate chunk, before storing
        chunk.validate();

        // add chunk to list
        chunks.push_back(chunk);
    }

    // keep a running tally
    int64_t totalErrorScore = 0;
    vector<int64_t> completionScores;

    // iterate over chunks
    for (Chunk& chunk : chunks) {
        // retrieve error scores
        totalErrorScore += chunk.errorScore;

        // complete the chunk
        int64_t completionScore = chunk.getCompletionScore();
        if (completionScore != 0) {
            completionScores.push_back(completionScore);
        }
    }

    // sort completion scores
    sort(completionScores.begin(), completionScores.end());

    // display results
    cout << "total syntax error score : " << totalErrorScore << endl;
    cout << "middle completion score : " << completionScores.at(completionScores.size() / 2) << endl;

    return 0;
}
