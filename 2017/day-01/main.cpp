// C++ standard includes
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
    const size_t halfAhead = (line.size() / 2);

    // running captcha
    int64_t firstCaptcha = 0;
    int64_t secondCaptcha = 0;

    // process line
    for (size_t current = 0; current < line.size(); current++) {
        // get next index
        size_t next = (current + 1);
        if (next >= line.size()) {
            next -= line.size();
        }

        // check if the two characters match
        if (line.at(current) == line.at(next)) {
            // add to first captcha
            int64_t number = (line.at(current) - '0');
            firstCaptcha += number;
        }

        // get next index
        next = (current + halfAhead);
        if (next >= line.size()) {
            next -= line.size();
        }

        // check if the two characters match
        if (line.at(current) == line.at(next)) {
            // add to second captcha
            int64_t number = (line.at(current) - '0');
            secondCaptcha += number;
        }
    }

    cout << "first CAPTCHA : " << firstCaptcha << endl;
    cout << "second CAPTCHA : " << secondCaptcha << endl;

    return 0;
}
