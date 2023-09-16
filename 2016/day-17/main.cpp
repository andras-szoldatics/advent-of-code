#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>

// 3rd-party includes
#include <openssl/evp.h>

using namespace std;

string md5Hash(const std::string& input) {
    // calculate hash for input
    EVP_MD_CTX* md5Context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);
    EVP_DigestUpdate(md5Context, input.c_str(), static_cast<unsigned long>(input.size()));
    uint resultSize = EVP_MD_size(EVP_md5());
    unsigned char result[resultSize];
    EVP_DigestFinal_ex(md5Context, result, &resultSize);

    // assemble output as string
    std::stringstream output;
    output << std::hex << std::setfill('0');

    for (const auto& byte : result) {
        output << std::setw(2) << int(byte);
    }

    return output.str();
}

class Step {
public:
    // simple attributes
    string passcode;
    int64_t row;
    int64_t col;

    // BFS attributes
    string path;

    Step(const string& _passcode,
         int64_t _row = 0,
         int64_t _col = 0,
         const string& _path = {}) :
        passcode(_passcode),
        row(_row),
        col(_col),
        path(_path) {}

    // utility functions
    vector<Step> generateNextSteps() {
        // temporary storage
        vector<Step> nextSteps;

        // generate hash for current path
        string hash = md5Hash(passcode + path);

        // check for open doors
        // UP
        if (openChar(hash.at(0)) && (row > 0)) {
            Step tmp(passcode, row - 1, col, path + "U");
            nextSteps.push_back(tmp);
        }

        // DOWN
        if (openChar(hash.at(1)) && (row < 3)) {
            Step tmp(passcode, row + 1, col, path + "D");
            nextSteps.push_back(tmp);
        }

        // LEFT
        if (openChar(hash.at(2)) && (col > 0)) {
            Step tmp(passcode, row, col - 1, path + "L");
            nextSteps.push_back(tmp);
        }

        // RIGHT
        if (openChar(hash.at(3)) && (col < 3)) {
            Step tmp(passcode, row, col + 1, path + "R");
            nextSteps.push_back(tmp);
        }

        return nextSteps;
    }

private:
    bool openChar(char c) {
        return (c >= 'b') && (c <= 'f');
    }
};

pair<string, string> findRoute(const string& passcode) {
    // temporary storage
    vector<string> paths;

    // setup queue for iteration
    Step firstStep(passcode);

    queue<Step> queue;
    queue.push(firstStep);

    // iterate over queue
    while (!queue.empty()) {
        // get next step
        Step step = queue.front();
        queue.pop();

        // check if we are at the goal
        if ((step.row == 3) && (step.col == 3)) {
            // mark path as found
            paths.push_back(step.path);

            // no more branches
            continue;
        }

        // generate next steps
        vector<Step> nextSteps = step.generateNextSteps();

        // add next steps to queue
        for (const auto& nextStep : nextSteps) {
            queue.push(nextStep);
        }
    }

    // set shortest and longest path
    if (!paths.empty()) {
        return { paths.front(), paths.back() };
    }

    return {};
}

int main() {
    // setup for input
    string input("njfxhljp");

    // find first route
    pair<string, string> result = findRoute(input);

    // print result
    cout << "[first] shortest path: " << result.first << endl;
    cout << "[second] longest path (length): " << result.second.size() << endl;

    return 0;
}
