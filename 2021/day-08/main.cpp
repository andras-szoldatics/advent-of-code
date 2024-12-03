#include <cstdint>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

using namespace std;

const vector<string> wiresForNumbers = {
    "abcefg", // 6
    "cf", // 2
    "acdeg", // 5
    "acdfg", // 5
    "bcdf", // 4
    "abdfg", // 5
    "abdefg", // 6
    "acf", // 3
    "abcdefg", // 7
    "abcdfg" // 6
};

class Display {
public:
    // display attributes
    vector<string> signals;
    vector<string> numbersShown;

    // decoded values
    map<int64_t, set<string>> guessed;
    map<int64_t, string> decoded;

    int64_t shown;

    Display() :
        shown(0) {}

    void parseFromString(const string& line) {
        // split string into two
        size_t pipePosition = line.find('|');
        string firstPart = line.substr(0, pipePosition - 1);
        string lastPart = line.substr(pipePosition + 2);

        string value;
        // assign first buffer
        stringstream buffer(firstPart);

        // read values from buffer
        while (getline(buffer, value, ' ')) {
            // sort signals in wiring
            sort(value.begin(), value.end());

            // add values one-by-one
            signals.push_back(value);
        }

        // assign last buffer
        buffer = stringstream(lastPart);

        // read values from buffer
        while (getline(buffer, value, ' ')) {
            // sort signals in wiring
            sort(value.begin(), value.end());

            // add values one-by-one
            numbersShown.push_back(value);
        }
    }

    void guessNumbers() {
        // iterate over signals
        for (const string& signal : signals) {
            // assign values to signals
            for (size_t i = 0; i < wiresForNumbers.size(); i++) {
                if (wiresForNumbers.at(i).size() == signal.size()) {
                    // check for one-offs
                    if ((i == 1) ||
                        (i == 4) ||
                        (i == 7) ||
                        (i == 8)) {
                        // these are fixed numbers
                        decoded[i] = signal;
                    } else {
                        // these are questionable numbers
                        guessed[i].insert(signal);
                    }
                }
            }
        }

    }

    int64_t countSingularNumbersShown() {
        // temporary storage
        int64_t count = 0;

        // iterate over numbers showsn
        for (const string& numberShown : numbersShown) {
            for (const pair<const int64_t, string>& entry : decoded) {
                if (entry.second == numberShown) {
                    count++;
                }
            }
        }

        return count;
    }

    string getDelta(const string& first,
                    const string& second) {
        // temporary storage
        string tmp;

        // retrieve short-hands
        const string& longer = (first.size() > second.size() ? first : second);
        const string& shorter = (first.size() > second.size() ? second : first);

        // iterate over pairs of chars
        for (char l : longer) {
            bool found = false;

            for (char s : shorter) {
                if (l == s) {
                    found = true;
                }
            }

            if (!found) {
                tmp.push_back(l);
            }
        }

        return tmp;
    }

    string getCommon(const string& first,
                     const string& second) {
        // temporary storage
        string tmp;

        // iterate over pairs of chars
        for (char f : first) {
            for (char s : second) {
                if (f == s) {
                    tmp.push_back(f);
                }
            }
        }

        return tmp;
    }

    void decodeNumbers() {
        // iterate over possibilities for 9
        for (const string& entry : guessed.at(9)) {
            // get common part with 4
            string commonPart = getCommon(entry, decoded.at(4));

            // check if this is the four match
            if (commonPart.size() == 4) {
                // this is now decoded
                decoded[9] = entry;
                guessed.erase(9);

                // remove entry from competing values
                guessed.at(6).erase(entry);
                guessed.at(0).erase(entry);
                break;
            }
        }

        // iterate over possibilities for 0
        for (const string& entry : guessed.at(0)) {
            // get common part with 7
            string commonPart = getCommon(entry, decoded.at(7));

            // check if this is the four match
            if (commonPart.size() == 3) {
                // this is now decoded
                decoded[0] = entry;
                guessed.erase(0);

                // remove entry from competing values
                guessed.at(6).erase(entry);
                break;
            }
        }

        // check if 6 is clear at this point
        for (const string& entry : guessed.at(6)) {
            // this is now decoded
            decoded[6] = entry;
            guessed.erase(6);
            break;
        }

        // iterate over possibilities for 3
        for (const string& entry : guessed.at(3)) {
            // get common part with 7
            string commonPart = getCommon(entry, decoded.at(7));

            // check if this is the four match
            if (commonPart.size() == 3) {
                // this is now decoded
                decoded[3] = entry;
                guessed.erase(3);

                // remove entry from competing values
                guessed.at(2).erase(entry);
                guessed.at(5).erase(entry);
                break;
            }
        }

        // iterate over possibilities for 5
        for (const string& entry : guessed.at(5)) {
            // get common part with 6
            string commonPart = getCommon(entry, decoded.at(6));

            // check if this is the four match
            if (commonPart.size() == 5) {
                // this is now decoded
                decoded[5] = entry;
                guessed.erase(5);

                // remove entry from competing values
                guessed.at(2).erase(entry);
                break;
            }
        }

        // check if 2 is clear at this point
        for (const string& entry : guessed.at(2)) {
            // this is now decoded
            decoded[2] = entry;
            guessed.erase(2);
            break;
        }

        // calculate display numbers
        for (const string& numberShown : numbersShown) {
            // shift number
            shown *= 10;

            // check for decoded version
            for (const pair<const int64_t, string>& entry : decoded) {
                if (entry.second == numberShown) {
                    shown += entry.first;
                    break;
                }
            }
        }
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // list of displays
    vector<Display> displays;

    int64_t singularValues = 0;
    int64_t totalDisplayed = 0;
    // read line of display signals
    while (getline(input, line)) {
        // create new display
        Display display;
        display.parseFromString(line);

        // guess numbers
        display.guessNumbers();
        singularValues += display.countSingularNumbersShown();

        // decode numbers
        display.decodeNumbers();

        // show displayed number
        totalDisplayed += display.shown;

        // add to list
        displays.push_back(display);
    }

    cout << "unique values decoded : " << singularValues << endl;
    cout << "total of numbers displayed : " << totalDisplayed << endl;

    return 0;
}
