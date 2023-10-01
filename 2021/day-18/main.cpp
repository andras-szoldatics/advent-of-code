#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stack>
#include <vector>

using namespace std;

// Advent of Code - 2021, day 18

class Snailfish;
typedef shared_ptr<Snailfish> SnailfishPtr;

typedef pair<SnailfishPtr, size_t> SnailfishNode;
typedef vector<SnailfishNode> SnailfishNodes;

class Snailfish {
public:
    // node attributes
    optional<int64_t> valueInt;

    // tree attributes
    SnailfishPtr left;
    SnailfishPtr right;

    Snailfish(int64_t _number) :
        valueInt(_number) {}

    Snailfish(const string& buffer = {}) {
        // check for empty string
        if (buffer.empty()) {
            return;
        }

        // check for encapsulation
        bool encapsulated = (buffer.front() == '[') && (buffer.back() == ']');

        if (!encapsulated) {
            // parse as int
            valueInt = stoll(buffer);
        } else {
            // find the index that is in the middle
            int64_t level = 0;
            optional<size_t> middle;

            // iterate over the string
            for (size_t i = 1; i < buffer.size() - 1; i++) {
                // check for encapsulation
                if (buffer[i] == '[') {
                    level++;
                } else if (buffer[i] == ']') {
                    level--;
                }

                // check if we are at the middle
                if ((level == 0) &&
                    (buffer[i] == ',')) {
                    // set the middle
                    middle = i;

                    break;
                }
            }

            // check if we found the middle
            if (middle.has_value()) {
                // split string
                string leftString = buffer.substr(1, middle.value() - 1);
                string rightString = buffer.substr(middle.value() + 1, buffer.size() - middle.value() - 2);

                // parse the left and right
                left = make_shared<Snailfish>(leftString);
                right = make_shared<Snailfish>(rightString);
            }
        }
    }

    // helper functions
    Snailfish deepCopy() const {
        // create new instance
        Snailfish copy;

        // copy attributes
        copy.valueInt = valueInt;
        copy.left = left ? make_shared<Snailfish>(left->deepCopy()) : nullptr;
        copy.right = right ? make_shared<Snailfish>(right->deepCopy()) : nullptr;

        return copy;
    }

    string asString() const {
        // check if we are a leaf
        if (!left && !right) {
            // return the value
            return to_string(valueInt.value());
        } else {
            // return the encapsulated string
            return "[" + left->asString() + "," + right->asString() + "]";
        }
    }

    int64_t getMagnitude() const {
        // check if we are a leaf
        if (!left && !right) {
            // return the value
            return valueInt.value_or(0);
        } else {
            // return the sum of the left and right
            return 3 * left->getMagnitude() + 2 * right->getMagnitude();
        }
    }

    vector<SnailfishNode> walkTree() {
        // temporary storage
        vector<SnailfishNode> tmp;

        // setup queue
        stack<SnailfishNode> stack;

        // add the first elements
        stack.push({ right, 0 });
        stack.push({ left, 0 });

        // iterate over the stack
        while (!stack.empty()) {
            // get the top element
            SnailfishNode node = stack.top();
            stack.pop();

            if (node.first) {
                // add the left and right
                stack.push({ node.first->right, node.second + 1 });
                stack.push({ node.first->left, node.second + 1 });

                // add to vector
                tmp.push_back(node);
            }
        }

        return tmp;
    }

    // utility functions
    bool explodeNumber() {
        // generate nodes
        SnailfishNodes nodes = walkTree();
        size_t center = numeric_limits<size_t>::max();

        // iterate over the nodes
        for (size_t i = 0; i < nodes.size(); i++) {
            // retrieve short-hands
            const SnailfishNode& node = nodes.at(i);

            // check level
            if ((node.first->right) &&
                (node.first->left) &&
                (node.second >= 3)) {
                center = i;
                break;
            }
        }

        if (center < nodes.size()) {
            // retrieve original node
            SnailfishNode node = nodes.at(center);

            // retrieve left and right value
            int64_t leftValue = node.first->left->valueInt.value_or(0);
            int64_t rightValue = node.first->right->valueInt.value_or(0);

            // modify node
            node.first->valueInt = 0;
            node.first->left = nullptr;
            node.first->right = nullptr;

            // find node to add left value to
            for (size_t delta = 1; delta <= center; delta++) {
                // retrieve left node
                SnailfishNode leftNode = nodes.at(center - delta);
                if (leftNode.first->valueInt) {
                    leftNode.first->valueInt = leftNode.first->valueInt.value_or(0) + leftValue;

                    break;
                }
            }

            // find node to add right value to
            for (size_t delta = center + 3; delta < nodes.size(); delta++) {
                // retrieve right node
                SnailfishNode rightNode = nodes.at(delta);
                if (rightNode.first->valueInt) {
                    rightNode.first->valueInt = rightNode.first->valueInt.value_or(0) + rightValue;

                    break;
                }
            }

            return true;
        }

        return false;
    }

    bool splitNumber() {
        // generate nodes
        SnailfishNodes nodes = walkTree();

        // iterate over the nodes
        for (const SnailfishNode& node : nodes) {
            // retrieve number
            int64_t number = node.first->valueInt.value_or(0);

            if (number >= 10) {
                // split the number
                int64_t leftNumber = number / 2;
                int64_t rightNumber = number - leftNumber;

                // modify node
                node.first->valueInt.reset();
                node.first->left = make_shared<Snailfish>(leftNumber);
                node.first->right = make_shared<Snailfish>(rightNumber);

                return true;
            }
        }

        return false;
    }

    void reduceNumber() {
        // setup for iteration
        bool changed = true;

        while (changed) {
            changed = explodeNumber();

            if (!changed) {
                changed = splitNumber();
            }
        }
    }

    // operator functions
    Snailfish operator+(const Snailfish& rhs) const {
        // create a new tree
        Snailfish tree;

        tree.left = make_shared<Snailfish>(deepCopy());
        tree.right = make_shared<Snailfish>(rhs.deepCopy());

        // reduce the number
        tree.reduceNumber();

        return tree;
    }
};

int main() {
    // temporary storage
    string line;

    // storage
    vector<Snailfish> fishies;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // fill vector with fishies
        fishies.emplace_back(line);
    }

    // calculate final sum
    Snailfish firstTotal = fishies.front().deepCopy();
    for (size_t i = 1; i < fishies.size(); i++) {
        firstTotal = firstTotal + fishies.at(i);
    }

    // display final sum
    cout << "[first] exact result: " << firstTotal.asString() << endl;
    cout << "[first] total magnitude: " << firstTotal.getMagnitude() << endl;

    // find largest magnitude pairs
    int64_t largestMagnitude = 0;
    Snailfish largestResult;

    for (size_t lhs = 0; lhs < fishies.size(); lhs++) {
        for (size_t rhs = 0; rhs < fishies.size(); rhs++) {
            // skip same pair
            if (lhs == rhs) {
                continue;
            }

            // calculate magnitude
            Snailfish result = fishies.at(lhs) + fishies.at(rhs);
            int64_t magnitude = result.getMagnitude();

            // check if we have a new largest magnitude
            if (largestMagnitude < magnitude) {
                largestResult = result;
                largestMagnitude = magnitude;
            }
        }
    }

    // display largest magnitude
    cout << "[second] exact result: " << largestResult.asString() << endl;
    cout << "[second] largest magnitude: " << largestMagnitude << endl;

    return 0;
}
