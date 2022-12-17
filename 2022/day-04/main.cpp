// C++ standard includes
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

typedef array<int64_t, 2> Interval;

class Assignment {
public:
    Interval first;
    Interval second;

    Assignment() {
        fill(first.begin(), first.end(), 0);
        fill(second.begin(), second.end(), 0);
    }

    // parsing functions
    void parse(const string& line) {
        // temporary storage
        string tmp;

        // assign line to a buffer
        stringstream buffer(line);

        // parse first interval
        getline(buffer, tmp, ',');
        first = parseInterval(tmp);

        // parse second interval
        getline(buffer, tmp, ',');
        second = parseInterval(tmp);
    }

    // evaluation functions
    bool isContained() {
        // check for containment
        if ((first.at(0) >= second.at(0)) &&
            (first.at(1) <= second.at(1))) {
            return true;
        }

        if ((second.at(0) >= first.at(0)) &&
            (second.at(1) <= first.at(1))) {
            return true;
        }

        return false;
    }

    bool isOverlapping() {
        // check for disjunct values
        if ((first.at(0) > second.at(1)) ||
            (first.at(1) < second.at(0))) {
            return false;
        }

        return true;
    }

private:
    Interval parseInterval(const string& part) {
        // temporary storage
        Interval interval;
        string tmp;

        // assign part to a buffer
        stringstream buffer(part);

        // parse first number
        getline(buffer, tmp, '-');
        interval.at(0) = stoll(tmp);

        // parse second number
        getline(buffer, tmp, '-');
        interval.at(1) = stoll(tmp);

        return interval;
    }
};

int main() {
    // temporary storage
    string line;

    // keep a running total
    int64_t fullyContained = 0;
    int64_t partialOverlap = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Assignment assignment;

        // parse intervals
        assignment.parse(line);

        // check intervals
        if (assignment.isContained()) {
            fullyContained++;
        }

        if (assignment.isOverlapping()) {
            partialOverlap++;
        }
    }

    // display results
    cout << "fully contained assignments : " << fullyContained << endl;
    cout << "partially overlapping assignments : " << partialOverlap << endl;

    return 0;
}
