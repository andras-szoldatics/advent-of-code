#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

class Aunt {
public:
    // simple attributes
    string id;

    // optional attributes
    unordered_map<string, int64_t> attributes;

    Aunt(const string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // skip first word
        getline(buffer, tmp, ' ');

        // read aunt id
        getline(buffer, id, ':');

        // skip to next word
        getline(buffer, tmp, ' ');

        string key, value;
        // repeated parsing of key - value pairs
        while (getline(buffer, key, ':')) {
            // parse value for key
            getline(buffer, value, ',');

            // assign value
            attributes[key] = stoll(value);

            // skip to next word
            getline(buffer, tmp, ' ');
        }
    }

    // utility functions
    bool exactMatchFor(const Aunt& other) const {
        // iterate over other key - value pairs
        for (const pair<const string, int64_t>& entry : other.attributes) {
            // check for key match
            if (attributes.count(entry.first) != 0) {
                // check for value match
                if (attributes.at(entry.first) != entry.second) {
                    return false;
                }
            }
        }

        return true;
    }

    bool rangeMatchFor(const Aunt& other) const {
        // iterate over other key - value pairs
        for (const pair<const string, int64_t>& entry : other.attributes) {
            // check for key match
            if (attributes.count(entry.first) != 0) {
                if ((entry.first == string("cats")) ||
                    (entry.first == string("trees"))) {
                    // check for less or equal
                    if (attributes.at(entry.first) <= entry.second) {
                        return false;
                    }
                } else if ((entry.first == string("pomeranians")) ||
                           (entry.first == string("goldfish"))) {
                    // check for greater or equal
                    if (attributes.at(entry.first) >= entry.second) {
                        return false;
                    }
                } else {
                    // check for value match
                    if (attributes.at(entry.first) != entry.second) {
                        return false;
                    }
                }
            }
        }

        return true;
    }
};

int main() {
    // temporary storage
    string line;

    // list of reindeer
    vector<Aunt> aunts;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create reindeer
        Aunt tmp(line);

        // add reindeer to list
        aunts.push_back(tmp);
    }

    // define analyzed aunt values
    Aunt analyzed("Sue ANALYZED: children: 3, cats: 7, samoyeds: 2, pomeranians: 3, akitas: 0, vizslas: 0, goldfish: 5, trees: 3, cars: 2, perfumes: 1");

    // setup for iteration
    string firstAunt("ANALYZED");
    string secondAunt("ANALYZED");

    // iterate over the list of aunts
    for (const Aunt& aunt : aunts) {
        if (aunt.exactMatchFor(analyzed)) {
            firstAunt = aunt.id;
        }

        if (aunt.rangeMatchFor(analyzed)) {
            secondAunt = aunt.id;
        }
    }

    // diplay results
    cout << "[first] found match for analyzed data : " << firstAunt << endl;
    cout << "[second] found match for analyzed data : " << secondAunt << endl;

    return 0;
}
