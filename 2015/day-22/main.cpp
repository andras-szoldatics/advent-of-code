#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Boss {
public:
    // simple attributes
    int64_t HP;
    int64_t damage;

    Boss() :
        HP(14),
        damage(8) {}
};

class Wizard {
public:
    // simple attributes
    int64_t HP;
    int64_t mana;

    Wizard() :
        HP(50),
        mana(500) {}
};

class Spell {
public:
    // simple attributes
    int64_t cost;
    int64_t damage;
    int64_t armor;
    int64_t heal;
    int64_t mana;
    int64_t turns;

    string name;

    Spell() :
        cost(0),
        damage(0),
        armor(0),
        heal(0),
        mana(0),
        turns(0) {}
    Spell(int64_t _cost,
          int64_t _damage,
          int64_t _armor,
          int64_t _heal,
          int64_t _mana,
          int64_t _turns,
          const string& _name) :
        cost(_cost),
        damage(_damage),
        armor(_armor),
        heal(_heal),
        mana(_mana),
        turns(_turns),
        name(_name) {}
};

// define available spells
const vector<Spell> SPELLBOOK = {
    { 53, 4, 0, 0, 0, 1, "Magic Missile" },
    { 73, 2, 0, 2, 0, 1, "Drain" },
    { 113, 0, 7, 0, 0, 6, "Shield" },
    { 173, 3, 0, 0, 0, 6, "Poison" },
    { 229, 0, 0, 0, 101, 5, "Recharge" },
};

class State {
public:
    // participants
    Wizard wizard;
    Boss boss;

    // active spells
    unordered_map<string, Spell> spells;

    // running tally
    int64_t manaSpent;

    State(const Wizard& _wizard,
          const Boss& _boss) :
        wizard(_wizard),
        boss(_boss),
        manaSpent(0) {}

    // operator functions
    bool operator>(const State& other) const {
        return (manaSpent > other.manaSpent);
    }

    // helper functions
    bool isLost() {
        return (wizard.HP <= 0);
    }

    bool isWon() {
        return (boss.HP <= 0);
    }

    // utility functions
    void castSpell(const Spell& spell) {
        // mark spell cost
        wizard.mana -= spell.cost;
        manaSpent += spell.cost;

        // add spell to map
        spells[spell.name] = spell;
    }

    void tickSpells() {
        // temporary storage
        unordered_set<string> expired;

        // iterate over spells
        for (pair<const string, Spell>& entry : spells) {
            // apply effects that need to be applied
            boss.HP -= entry.second.damage;
            wizard.HP += entry.second.heal;
            wizard.mana += entry.second.mana;

            // decrease duration
            entry.second.turns -= 1;
            if (entry.second.turns == 0) {
                expired.insert(entry.first);
            }
        }

        // iterate over expired spells
        for (const string& entry : expired) {
            // remove spell from map
            spells.erase(entry);
        }
    }
};

int64_t battle(const Wizard& wizard,
               const Boss& boss,
               int64_t effect) {
    // setup initial state
    State initial(wizard, boss);

    // setup queue for iteration
    int64_t minimumMana = numeric_limits<int64_t>::max();

    queue<State> queue;
    queue.push(initial);

    // process queue, until stop
    while (!queue.empty()) {
        // retrieve first entry from queue
        State current = queue.front();
        queue.pop();

        // pre-filter variation
        if (minimumMana <= current.manaSpent) {
            continue;
        }

        // PLAYER'S TURN
        // apply effect
        current.wizard.HP += effect;
        if (current.isLost()) {
            // no need for further processing
            continue;
        }

        // tick spells
        current.tickSpells();
        if (current.isWon()) {
            if (minimumMana > current.manaSpent) {
                minimumMana = current.manaSpent;
            }

            // no need for further processing
            continue;
        }

        // select spell for player
        for (const Spell& spell : SPELLBOOK) {
            // check for available mana
            if (spell.cost > current.wizard.mana) {
                continue;
            }

            // check for same spell active
            if (current.spells.count(spell.name) != 0) {
                continue;
            }

            // split current into variation
            State variation = current;

            // cast spell, add it to map
            variation.castSpell(spell);

            // post-filter variation
            if (minimumMana <= variation.manaSpent) {
                continue;
            }

            // BOSS'S TURN
            // tick spells
            variation.tickSpells();
            if (variation.isWon()) {
                if (minimumMana > variation.manaSpent) {
                    minimumMana = variation.manaSpent;
                }

                // no need for further processing
                continue;
            }

            if (!variation.isWon()) {
                // boss can make an attack here
                int64_t damage = variation.boss.damage;
                if (variation.spells.count("Shield")) {
                    damage -= variation.spells.at("Shield").armor;
                    damage = max(damage, 1l);
                }

                // hit to the face
                variation.wizard.HP -= damage;
            }

            // add this variation to the queue
            if (!variation.isLost()) {
                queue.push(variation);
            }
        }
    }

    return minimumMana;
}

int main() {
    // temporary storage
    string line;

    // setup characters
    Wizard wizard;
    Boss boss;

    // read input file
    ifstream input("input.txt");

    // parse boss' hit points
    getline(input, line, ':');
    getline(input, line);
    boss.HP = stoll(line);

    getline(input, line, ':');
    getline(input, line);
    boss.damage = stoll(line);

    // execute battles
    int64_t firstManaSpent = battle(wizard, boss, 0);
    int64_t secondManaSpent = battle(wizard, boss, -1);

    // display results
    cout << "[first] least amount of mana spent : " << firstManaSpent << endl;
    cout << "[second] least amount of mana spent : " << secondManaSpent << endl;

    return 0;
}
