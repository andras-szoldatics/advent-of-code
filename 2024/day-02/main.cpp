#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

typedef vector<int64_t> Report;
typedef vector<Report> Reports;

bool isSafe(const Report& report,
            size_t skipped = numeric_limits<size_t>::max()) {
    uint64_t plus = 0;
    uint64_t minus = 0;

    for (size_t i = 1; i < report.size(); i++) {
        // retrieve short-hands
        size_t f = i - 1;
        size_t s = i;

        // shift values
        if (f == skipped) {
            if (f > 0) {
                f -= 1;
            } else {
                continue;
            }
        }

        if (s == skipped) {
            if (s < report.size() - 1) {
                s += 1;
            } else {
                continue;
            }
        }

        // retrieve short-hands
        int64_t first = report.at(f);
        int64_t second = report.at(s);
        int64_t delta = second - first;

        // check for delta
        if ((delta <= 3) &&
            (delta >= 1)) {
            plus += 1;
        } else if ((delta <= -1) &&
                   (delta >= -3)) {
            minus += 1;
        } else {
            return false;
        }

        if ((plus > 0) &&
            (minus > 0)) {
            return false;
        }
    }

    return true;
}

bool isAlright(const Report& report) {
    // iterate over skipped level
    for (size_t skipped = 0; skipped < report.size(); skipped++) {
        bool alright = isSafe(report, skipped);

        if (alright) {
            return true;
        }
    }

    return false;
}

int main() {
    // temporary storage
    string line;
    string tmp;

    Reports reports;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Report report;

        // assign line to buffer
        stringstream buffer(line);

        // parse numbers
        while (getline(buffer, tmp, ' ')) {
            int64_t value = stoll(tmp);
            report.push_back(value);
        }

        // add report to reports
        reports.push_back(report);
    }

    // temporary storage
    uint64_t first = 0;
    uint64_t second = 0;

    // check safety
    for (const Report& report : reports) {
        if (isSafe(report)) {
            first += 1;
            second += 1;
        } else if (isAlright(report)) {
            second += 1;
        }
    }

    cout << "[first] # of safe reports: " << first << endl;
    cout << "[second] # of safe reports: " << second << endl;

    return 0;
}
