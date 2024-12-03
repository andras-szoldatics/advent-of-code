#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class P {
public:
    int64_t x;
    int64_t y;

    P(int64_t a = 0,
      int64_t b = 0) :
        x(a),
        y(b) {}
};

class Polygon {
public:
    vector<P> points;

    Polygon() {
        // add default start
        points.emplace_back(0, 0);
    }

    // helper functions
    void moveUp(int64_t delta) {
        // retrieve last point
        const P& p = points.back();
        points.emplace_back(p.x, p.y + delta);
    }

    void moveDown(int64_t delta) {
        // retrieve last point
        const P& p = points.back();
        points.emplace_back(p.x, p.y - delta);
    }

    void moveLeft(int64_t delta) {
        // retrieve last point
        const P& p = points.back();
        points.emplace_back(p.x - delta, p.y);
    }

    void moveRight(int64_t delta) {
        // retrieve last point
        const P& p = points.back();
        points.emplace_back(p.x + delta, p.y);
    }

    // utility functions
    int64_t shoeLaceArea() {
        // temporary storage
        int64_t perimeter = 0;
        int64_t S1 = 0;
        int64_t S2 = 0;

        // iterate through points
        for (size_t i = 0; i < points.size() - 1; i++) {
            // retrieve short-hands
            const P& p1 = points.at(i);
            const P& p2 = (i + 1 == points.size()) ? points.at(0) : points.at(i + 1);

            // compute parts of formula
            S1 += p1.x * p2.y;
            S2 += p1.y * p2.x;

            // compute perimeter
            perimeter += abs(p1.x - p2.x) + abs(p1.y - p2.y);
        }

        // calculate area
        int64_t area = abs(S1 - S2) / 2;

        // since this is not a normal polygon, we need to add the perimeter, sort of
        return area + (perimeter / 2) + 1;
    }
};

int main() {
    // temporary storage
    string line;

    // polygon stroage
    Polygon first;
    Polygon second;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        string direction, tmp;

        // process line
        stringstream buffer(line);
        getline(buffer, direction, ' ');

        // retrieve deltas
        getline(buffer, tmp, ' ');
        int64_t delta = stoll(tmp);

        // draw polygon
        if (direction == string("U")) {
            first.moveUp(delta);
        } else if (direction == string("D")) {
            first.moveDown(delta);
        } else if (direction == string("L")) {
            first.moveLeft(delta);
        } else if (direction == string("R")) {
            first.moveRight(delta);
        }

        // process remainder of line
        getline(buffer, tmp, '#');
        getline(buffer, tmp, ')');
        direction = tmp.back();
        tmp.pop_back();

        // retrieve deltas
        delta = stoll(tmp, nullptr, 16);

        // draw polygon
        if (direction == string("3")) {
            second.moveUp(delta);
        } else if (direction == string("1")) {
            second.moveDown(delta);
        } else if (direction == string("2")) {
            second.moveLeft(delta);
        } else if (direction == string("0")) {
            second.moveRight(delta);
        }
    }

    // display resulting area
    cout << "[first] area: " << first.shoeLaceArea() << endl;
    cout << "[second] area: " << second.shoeLaceArea() << endl;

    return 0;
}
