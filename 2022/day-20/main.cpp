#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

typedef list<int64_t>::iterator Cursor;

class Mixer {
public:
    // current values
    list<int64_t> values;

    // original order of entries
    vector<Cursor> order;

    // data management functions
    void addValue(int64_t value) {
        // add to values
        values.push_back(value);

        // retrieve last cursor
        Cursor last = values.end();
        last--;

        // mark order
        order.push_back(last);
    }

    // utility functions
    Cursor moveCursor(Cursor cursor,
                      int64_t movement = 1) {
        // temporary storage
        Cursor current = cursor;

        // move the cursor one-by-one
        for (int64_t i = 0; i < movement; i++) {
            // advance cursor
            current++;

            // wrap-around
            if (current == values.end()) {
                current = values.begin();
            }
        }

        return current;
    }

    // cryptography functions
    Cursor mixIt() {
        // temporary storage
        Cursor zeroCursor = values.end();

        // iterate over the original order
        for (const Cursor& cursor : order) {
            // retrieve short-hands
            const int64_t& value = *cursor;

            // calculate offsets
            if (value > 0) {
                // temporary storage
                Cursor current = cursor;
                int64_t movement = value;

                // trim movement
                movement %= (values.size() - 1);

                // move cursor by movement
                current = moveCursor(current, movement + 1);

                // splice the list, using the list as target
                values.splice(current, values, cursor);
            } else if (value < 0) {
                // temporary storage
                Cursor current = cursor;
                int64_t movement = value;

                // shift movement around, until it becomes positive
                movement += ((values.size() - 1) * (-movement));
                movement %= (values.size() - 1);

                // move cursor by movement
                current = moveCursor(current, movement + 1);

                // splice the list, using the list as target
                values.splice(current, values, cursor);
            } else {
                // remember the zero, for later
                zeroCursor = cursor;
            }
        }

        return zeroCursor;
    }

    void applyDecryptionKey(const int64_t& key) {
        // iterate over values
        for (int64_t& value : values) {
            // apply key
            value *= key;
        }
    }

};

int main() {
    // temporary storage
    string line;

    // store values in a mixer
    Mixer firstMixer;
    Mixer secondMixer;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // parse number
        int64_t number = stoll(line);

        // add value
        firstMixer.addValue(number);
        secondMixer.addValue(number);
    }

    if (!firstMixer.values.empty()) {
        // execute first mix
        Cursor zeroCursor = firstMixer.mixIt();

        Cursor firstCursor = firstMixer.moveCursor(zeroCursor, 1000);
        Cursor secondCursor = firstMixer.moveCursor(zeroCursor, 2000);
        Cursor thirdCursor = firstMixer.moveCursor(zeroCursor, 3000);

        // display first coordinates
        cout << "[first] 1000th number : " << *firstCursor << endl;
        cout << "[first] 2000th number : " << *secondCursor << endl;
        cout << "[first] 3000th number : " << *thirdCursor << endl;
        cout << "[first] grove coordinates : " << (*firstCursor + *secondCursor + *thirdCursor) << endl << endl;
    }

    if (!secondMixer.values.empty()) {
        // apply decryption key
        secondMixer.applyDecryptionKey(811589153);

        Cursor zeroCursor;
        // execute the ten mixes
        for (size_t i = 0; i < 10; i++) {
            zeroCursor = secondMixer.mixIt();
        }

        Cursor firstCursor = secondMixer.moveCursor(zeroCursor, 1000);
        Cursor secondCursor = secondMixer.moveCursor(zeroCursor, 2000);
        Cursor thirdCursor = secondMixer.moveCursor(zeroCursor, 3000);

        // display first coordinates
        cout << "[second] 1000th number : " << *firstCursor << endl;
        cout << "[second] 2000th number : " << *secondCursor << endl;
        cout << "[second] 3000th number : " << *thirdCursor << endl;
        cout << "[second] grove coordinates : " << (*firstCursor + *secondCursor + *thirdCursor) << endl;
    }

    return 0;
}
