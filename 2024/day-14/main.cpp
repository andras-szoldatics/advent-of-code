#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

// extend hash functions

namespace std {
template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        return hash<int64_t>() (p.at(0)) + hash<int64_t>() (p.at(1));
    }
};

}

const int64_t WIDTH = 101;
const int64_t HEIGHT = 103;

class Robot {
public:
    Position position;
    Position speed;

    Robot(const string& line) {
        // temporary storage
        stringstream buffer(line);
        string s;

        // parse first number
        getline(buffer, s, '=');
        getline(buffer, s, ',');
        position.at(0) = stoll(s);

        // parse second number
        getline(buffer, s, ' ');
        position.at(1) = stoll(s);

        // parse third number
        getline(buffer, s, '=');
        getline(buffer, s, ',');
        speed.at(0) = stoll(s);

        // parse fourth number
        getline(buffer, s);
        speed.at(1) = stoll(s);
    }

    // utility functions
    void normalize() {
        position.at(0) = (position.at(0) + WIDTH) % WIDTH;
        position.at(1) = (position.at(1) + HEIGHT) % HEIGHT;
    }

    void move() {
        position.at(0) += speed.at(0);
        position.at(1) += speed.at(1);

        normalize();
    }
};

size_t checkRoom(const vector<Robot> robots) {
    // temporary storage
    array<size_t, 4> quadrants = { 0, 0, 0, 0 };

    // iterate over robots
    for (const Robot& robot : robots) {
        if (robot.position.at(0) < WIDTH / 2) {
            if (robot.position.at(1) < HEIGHT / 2) {
                quadrants[0] += 1;
            } else if (robot.position.at(1) > HEIGHT / 2) {
                quadrants[2] += 1;
            }
        } else if (robot.position.at(0) > WIDTH / 2) {
            if (robot.position.at(1) < HEIGHT / 2) {
                quadrants[1] += 1;
            } else if (robot.position.at(1) > HEIGHT / 2) {
                quadrants[3] += 1;
            }
        }
    }

    return (quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3]);
}

bool checkEasterEgg(const vector<Robot>& robots) {
    // temporary storage
    unordered_set<Position> positions;

    // iterate over robots
    for (const Robot& robot : robots) {
        positions.insert(robot.position);
    }

    return (positions.size() == robots.size());
}

int main() {
    // temporary storage
    string line;

    // temporary storage
    vector<Robot> robots;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        Robot robot(line);

        robots.push_back(robot);
    }

    // setup for iteration
    optional<size_t> safetyFactor;
    optional<size_t> secretSteps;
    size_t step = 0;

    // simulate the steps
    while (!safetyFactor || !secretSteps) {
        // make 'em dance
        for (Robot& robot : robots) {
            robot.move();
        }

        // mark step
        step += 1;

        if (step == 100) {
            safetyFactor = checkRoom(robots);
        }

        if (checkEasterEgg(robots)) {
            secretSteps = step;
        }
    }

    // display result
    cout << "[first] safety factor: " << safetyFactor.value_or(0) << endl;
    cout << "[second] secret steps: " << secretSteps.value_or(0) << endl;

    return 0;
}
