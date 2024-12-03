#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read first & only relevant line
    getline(input, line);

    // first & second part output
    int64_t currentFloor = 0;
    size_t basementPosition = 0;

    // process line
    for (size_t i = 0; i < line.size(); i++) {
        // retrieve short-hands
        char instruction = line.at(i);

        // follow instructions
        if (instruction == '(') {
            currentFloor++;
        } else if (instruction == ')') {
            currentFloor--;
        }

        // check if our current floor is -1
        if ((basementPosition == 0) &&
            (currentFloor == -1)) {
            // set index as position
            basementPosition = (i + 1);
        }
    }

    // display final floor
    cout << "final floor : " << currentFloor << endl;
    cout << "basement @ position : " << basementPosition << endl;

    return 0;
}
