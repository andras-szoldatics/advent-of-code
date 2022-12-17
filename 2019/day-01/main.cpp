// C++ standard includes
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    // temporary storage
    string line;

    // sum of fuel
    int64_t optimisticFuel = 0;
    int64_t realisticFuel = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        int64_t mass = stoll(line);

        // calculate initial fuel requirements
        int64_t fuel = (mass / 3) - 2;
        optimisticFuel += fuel;

        // rocket equation blows
        while (fuel >= 0) {
            // mark fuel requirements
            realisticFuel += fuel;

            // calculate new fuel requirements
            fuel = (fuel / 3) - 2;
        }
    }

    cout << "optimistic fuel value : " << optimisticFuel << endl;
    cout << "realistic fuel value : " << realisticFuel << endl;

    return 0;
}
