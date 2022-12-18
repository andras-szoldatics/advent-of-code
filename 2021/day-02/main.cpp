// C++ standard includes
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    // temporary storage
    string line;

    // submarine attributes
    int64_t horizontal = 0;
    int64_t firstDepth = 0;
    int64_t secondDepth = 0;
    int64_t secondAim = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // split line in two
        size_t firstSpace = line.find(' ');
        string command = line.substr(0, firstSpace);
        string tmp = line.substr(firstSpace + 1);

        // parse number
        int64_t number = stoll(tmp);

        // execute command
        if (command == "forward") {
            // increase horizontal
            horizontal += number;

            // align the second attribute
            secondDepth += (secondAim * number);
        } else if (command == "down") {
            // align the first attribute
            firstDepth += number;

            // align the second attribute
            secondAim += number;
        } else if (command == "up") {
            // allign the first attribute
            firstDepth -= number;

            // align the second attribute
            secondAim -= number;
        }
    }

    // display positions
    cout << "[first] final position : " << (horizontal * firstDepth) << endl;
    cout << "[second] final position : " << (horizontal * secondDepth) << endl;

    return 0;
}
