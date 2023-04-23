// C++ standard includes
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class AddressIP7 {
public:
    // different parts
    vector<string> simple;
    vector<string> bracketed;

    AddressIP7(const string& line) {
        // assign line to buffer
        stringstream buffer(line);

        string tmp;
        // parse address
        while (getline(buffer, tmp, '[')) {
            // add tmp as simple part
            simple.push_back(tmp);

            // read bracketed part
            if (getline(buffer, tmp, ']')) {
                bracketed.push_back(tmp);
            }
        }
    }

    // helper functions
    bool hasABBA(const string& part) const {
        // iterate over the string
        for (size_t i = 0; (i + 3) < part.size(); i++) {
            // check for ABBA pattern
            if ((part.at(i) == part.at(i + 3)) &&
                (part.at(i + 1) == part.at(i + 2)) &&
                (part.at(i) != part.at(i + 1))) {
                // found a match
                return true;
            }
        }

        // no match found
        return false;
    }

    vector<string> getListOfABA(const string& part) const {
        // temporary storage
        vector<string> result;

        // iterate over the string
        for (size_t i = 0; (i + 2) < part.size(); i++) {
            // check for ABA pattern
            if ((part.at(i) == part.at(i + 2)) &&
                (part.at(i) != part.at(i + 1))) {
                // found a match
                result.push_back(part.substr(i, 3));
            }
        }

        return result;
    }

    bool hasMatchingBAB(const vector<string>& entries,
                        const string& part) const {
        // iterate over the entries
        for (const string& entry : entries) {
            // generate corresponding BAB
            string filter = { entry.at(1), entry.at(0), entry.at(1) };

            // check for match
            if (part.find(filter) != string::npos) {
                // found a match
                return true;
            }
        }

        return false;
    }

    // utility functions
    bool isValidForTSL() const {
        // check for ABBA on bracketed parts
        for (const string& part : bracketed) {
            if (hasABBA(part)) {
                return false;
            }
        }

        // check for ABBA on simple parts
        for (const string& part : simple) {
            if (hasABBA(part)) {
                return true;
            }
        }

        // default to invalid
        return false;
    }

    bool isValidForSSL() const {
        // iterate over simple parts
        for (const string& part : simple) {
            // get all ABA entries
            vector<string> entries = getListOfABA(part);

            // check for match in bracketed parts
            for (const string& bracket : bracketed) {
                if (hasMatchingBAB(entries, bracket)) {
                    return true;
                }
            }
        }

        // default to invalid
        return false;
    }
};

void firstUnitTest() {
    AddressIP7 address1("abba[mnop]qrst");
    assert(address1.isValidForTSL());

    AddressIP7 address2("abcd[bddb]xyyx");
    assert(!address2.isValidForTSL());

    AddressIP7 address3("aaaa[qwer]tyui");
    assert(!address3.isValidForTSL());

    AddressIP7 address4("ioxxoj[asdfgh]zxcvbn");
    assert(address4.isValidForTSL());
}

void secondUnitTest() {
    AddressIP7 address1("aba[bab]xyz");
    assert(address1.isValidForSSL());

    AddressIP7 address2("xyx[xyx]xyx");
    assert(!address2.isValidForSSL());

    AddressIP7 address3("aaa[kek]eke");
    assert(address3.isValidForSSL());

    AddressIP7 address4("zazbz[bzb]cdb");
    assert(address4.isValidForSSL());
}

int main() {
    // temporary storage
    string line;

    firstUnitTest();
    secondUnitTest();

    // vector of addresses
    vector<AddressIP7> addresses;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create address from line
        AddressIP7 address(line);

        // add address to vector
        addresses.push_back(address);
    }

    // setup for iteration
    size_t firstCount = 0;
    size_t secondCount = 0;

    // iterate over the addresses
    for (const AddressIP7& address : addresses) {
        // check if address is valid
        if (address.isValidForTSL()) {
            // increase count
            firstCount += 1;
        }

        if (address.isValidForSSL()) {
            // increase count
            secondCount += 1;
        }
    }

    // display results
    cout << "[first] number of valid TLS addresses: " << firstCount << endl;
    cout << "[second] number of valid SSL addresses: " << secondCount << endl;

    return 0;
}
