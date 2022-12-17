// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int64_t getPriority(char c) {
    if ((c >= 'a') &&
        (c <= 'z')) {
        return (c - 'a' + 1);
    }

    if ((c >= 'A') &&
        (c <= 'Z')) {
        return (c - 'A' + 27);
    }

    return 0;
}

int main() {
    // temporary storage
    string line;

    // keep a running total
    int64_t firstSum = 0;
    int64_t secondSum = 0;

    // keep a list of rucksacks
    vector<string> rucksacks;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // add line to buffer
        rucksacks.push_back(line);
    }

    // iterate over rucksacks one-by-one
    for (const string& rucksack : rucksacks) {
        // retrive two halves of rucksack
        string firstHalf = rucksack.substr(0, rucksack.size() / 2);
        string secondHalf = rucksack.substr(rucksack.size() / 2);

        // sort halves
        sort(firstHalf.begin(), firstHalf.end());
        sort(secondHalf.begin(), secondHalf.end());

        // retrieve intersection
        string intersection;
        string::iterator it = set_intersection(firstHalf.begin(),
                                               firstHalf.end(),
                                               secondHalf.begin(),
                                               secondHalf.end(),
                                               intersection.begin());
        intersection.erase(it, intersection.end());

        // check for intersection
        if (!intersection.empty()) {
            firstSum += getPriority(intersection.front());
        }
    }

    // iterate over rucksacks three-by-three
    for (size_t i = 2; i < rucksacks.size(); i += 3) {
        // retrieve short-hands
        string& first = rucksacks.at(i - 2);
        string& second = rucksacks.at(i - 1);
        string& third = rucksacks.at(i);

        // sort rucksacks
        sort(first.begin(), first.end());
        sort(second.begin(), second.end());
        sort(third.begin(), third.end());

        // create first intersection
        string firstIntersection;
        string::iterator firstIt = set_intersection(first.begin(),
                                                    first.end(),
                                                    second.begin(),
                                                    second.end(),
                                                    firstIntersection.begin());
        firstIntersection.erase(firstIt, firstIntersection.end());

        // create second intersection
        string secondIntersection;
        string::iterator secondIt = set_intersection(second.begin(),
                                                     second.end(),
                                                     third.begin(),
                                                     third.end(),
                                                     secondIntersection.begin());
        secondIntersection.erase(secondIt, secondIntersection.end());

        // create final intersection
        string finalIntersection;
        string::iterator finalIt = set_intersection(firstIntersection.begin(),
                                                    firstIntersection.end(),
                                                    secondIntersection.begin(),
                                                    secondIntersection.end(),
                                                    finalIntersection.begin());
        finalIntersection.erase(finalIt, finalIntersection.end());

        // check for intersection
        if (!finalIntersection.empty()) {
            secondSum += getPriority(finalIntersection.front());
        }
    }

    // display results
    cout << "first sum of items : " << firstSum << endl;
    cout << "second sum of items : " << secondSum << endl;

    return 0;
}
