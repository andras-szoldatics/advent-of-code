// C++ standard includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

class Ingredient {
public:
    // simple attributes
    string name;

    int64_t capacity;
    int64_t durability;
    int64_t flavor;
    int64_t texture;
    int64_t calories;

    Ingredient(const std::string& line) {
        // assign line to buffer
        stringstream buffer(line);
        string tmp;

        // parse name
        getline(buffer, name, ':');

        // skip next word
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse capacity
        getline(buffer, tmp, ',');
        capacity = stoll(tmp);

        // skip next word
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse durability
        getline(buffer, tmp, ',');
        durability = stoll(tmp);

        // skip next word
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse flavor
        getline(buffer, tmp, ',');
        flavor = stoll(tmp);

        // skip next word
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse texture
        getline(buffer, tmp, ',');
        texture = stoll(tmp);

        // skip next word
        getline(buffer, tmp, ' ');
        getline(buffer, tmp, ' ');

        // parse calories
        getline(buffer, tmp);
        calories = stoll(tmp);
    }
};

typedef vector<Ingredient> Ingredients;
typedef vector<int64_t> Recipe;

int64_t scoreRecipe(const Ingredients& ingredients,
                    const Recipe& recipe,
                    bool checkCalories = false) {
    // temporary storage
    int64_t capacity = 0;
    int64_t durability = 0;
    int64_t flavor = 0;
    int64_t texture = 0;
    int64_t calories = 0;

    // iterate over recipe elements
    for (size_t i = 0; i < recipe.size(); i++) {
        // retrieve short-hands
        const Ingredient& ingredient = ingredients.at(i);
        int64_t amount = recipe.at(i);

        // add to sub-values
        capacity += (amount * ingredient.capacity);
        durability += (amount * ingredient.durability);
        flavor += (amount * ingredient.flavor);
        texture += (amount * ingredient.texture);
        calories += (amount * ingredient.calories);
    }

    // check for easy returns
    if (capacity <= 0) {
        return 0;
    }
    if (durability <= 0) {
        return 0;
    }
    if (flavor <= 0) {
        return 0;
    }
    if (texture <= 0) {
        return 0;
    }

    // check calories, when needed
    if ((checkCalories) &&
        (calories != 500)) {
        return 0;
    }

    // return final score
    return (capacity * durability * flavor * texture);
}

int64_t workOnRecipe(const Ingredients& ingredients,
                     const Recipe& currentRecipe,
                     bool checkCalories = false) {
    // temporary storage
    int64_t bestScore = numeric_limits<int64_t>::min();
    int64_t remainingAmount = 100;

    // return score, when recipe is finalized
    if (ingredients.size() == currentRecipe.size()) {
        return scoreRecipe(ingredients, currentRecipe, checkCalories);
    }

    // iterate over current recipe to determine remaining amount
    for (int64_t amount : currentRecipe) {
        remainingAmount -= amount;
    }

    // check how to extend recipe
    if (currentRecipe.size() + 1 == ingredients.size()) {
        // extend with all the remaining amount
        Recipe newRecipe = currentRecipe;
        newRecipe.push_back(remainingAmount);

        // recipe is finished, we can evaluate it
        return scoreRecipe(ingredients, newRecipe, checkCalories);
    }

    // there are many choices to make here
    for (int64_t chosenAmount = 0; chosenAmount <= remainingAmount; chosenAmount++) {
        // extend with the chosen amount
        Recipe newRecipe = currentRecipe;
        newRecipe.push_back(chosenAmount);

        // finish partial recipe below
        int64_t currentScore = workOnRecipe(ingredients, newRecipe, checkCalories);

        if (bestScore < currentScore) {
            bestScore = currentScore;
        }
    }

    // sub-choices evaluated, return best score
    return bestScore;
}

int main() {
    // temporary storage
    string line;

    // list of reindeer
    vector<Ingredient> ingredients;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create reindeer
        Ingredient tmp(line);

        // add reindeer to list
        ingredients.push_back(tmp);
    }

    // calculate results
    int64_t firstResult = workOnRecipe(ingredients, {});
    int64_t secondResult = workOnRecipe(ingredients, {}, true);

    // diplay results
    cout << "[first] best score possible : " << firstResult << endl;
    cout << "[second] best score possible : " << secondResult << endl;

    return 0;
}
