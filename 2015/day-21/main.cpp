#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

class Stats {
public:
    // simple attributes
    int64_t cost;
    int64_t damage;
    int64_t armor;

    string name;

    Stats(int64_t _cost,
          int64_t _damage,
          int64_t _armor,
          const string& _name) :
        cost(_cost),
        damage(_damage),
        armor(_armor),
        name(_name) {}
};

class Character :
    public Stats {
public:
    // simple attributes
    int64_t HP;

    Character(const string& _name) :
        Stats(0, 0, 0, _name),
        HP(100) {}

    // utility functions
    void equip(const Stats& gear) {
        // add gear stats
        cost += gear.cost;
        damage += gear.damage;
        armor += gear.armor;
    }

    bool canBeatOther(const Character& other) {
        // calculate DPR for each character
        int64_t selfDPR = max(damage - other.armor, 1l);
        int64_t otherDPR = max(other.damage - armor, 1l);

        // calculate rounds to win for myself
        int64_t selfVictory = (other.HP / selfDPR);
        if (other.HP % selfDPR != 0) {
            selfVictory += 1;
        }

        // calculate rounds to win for the other
        int64_t otherVictory = (HP / otherDPR);
        if (HP % otherDPR != 0) {
            otherVictory += 1;
        }

        return (selfVictory <= otherVictory);
    }
};

// constants for shop's gear
const Stats NOTHING = { 0, 0, 0, "Nothing" };

const vector<Stats> WEAPONS = {
    { 8, 4, 0, "Dagger" },
    { 10, 5, 0, "Shortsword" },
    { 25, 6, 0, "Warhammer" },
    { 40, 7, 0, "Longsword" },
    { 74, 8, 0, "Greataxe" },
};

const vector<Stats> ARMOR = {
    { 13, 0, 1, "Leather" },
    { 31, 0, 2, "Chainmail" },
    { 53, 0, 3, "Splintmail" },
    { 75, 0, 4, "Bandedmail" },
    { 102, 0, 5, "Platemail" },
};

const vector<Stats> RINGS = {
    { 25, 1, 0, "Damage +1" },
    { 50, 2, 0, "Damage +2" },
    { 100, 3, 0, "Damage +3" },
    { 20, 0, 1, "Defense +1" },
    { 40, 0, 2, "Defense +2" },
    { 80, 0, 3, "Defense +3" },
};

int main() {
    // temporary storage
    string line;

    // setup characters
    Character player("PC");
    Character boss("BBEG");

    // read input filef
    ifstream input("input.txt");

    // parse boss' hit points
    getline(input, line, ':');
    getline(input, line);
    boss.HP = stoll(line);

    // parse boss' damage
    getline(input, line, ':');
    getline(input, line);
    boss.damage = stoll(line);

    // parse boss' armor
    getline(input, line, ':');
    getline(input, line);
    boss.armor = stoll(line);

    // keep a running tally
    int64_t firstCost = numeric_limits<int64_t>::max();
    int64_t secondCost = numeric_limits<int64_t>::min();

    // iterate through possible gear combinations
    for (size_t w = 1; w <= WEAPONS.size(); w++) {
        // retrieve short-hands
        const Stats& weapon = WEAPONS.at(w - 1);

        for (size_t a = 0; a <= ARMOR.size(); a++) {
            // retrieve short-hands
            const Stats& armor = (a == 0 ? NOTHING : ARMOR.at(a - 1));

            for (size_t r1 = 0; r1 <= RINGS.size(); r1++) {
                // retrieve short-hands
                const Stats& ring1 = (r1 == 0 ? NOTHING : RINGS.at(r1 - 1));

                for (size_t r2 = r1; r2 <= RINGS.size(); r2++) {
                    // retrieve short-hands
                    const Stats& ring2 = (r2 == 0 ? NOTHING : RINGS.at(r2 - 1));

                    // create possible setup
                    Character setup = player;
                    setup.equip(weapon);
                    setup.equip(armor);
                    setup.equip(ring1);
                    setup.equip(ring2);

                    // check against boss
                    if (setup.canBeatOther(boss)) {
                        // check against cost
                        if (firstCost > setup.cost) {
                            firstCost = setup.cost;
                        }
                    } else {
                        // check against cost
                        if (secondCost < setup.cost) {
                            secondCost = setup.cost;
                        }
                    }
                }
            }
        }
    }

    // display results
    cout << "[first] least amount of gold to beat the boss : " << firstCost << endl;
    cout << "[second] most amount of gold to lose to the boss : " << secondCost << endl;

    return 0;
}
