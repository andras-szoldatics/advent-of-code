#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

typedef long double EntryD;
typedef vector<EntryD> VectorD;
typedef vector<VectorD> MatrixD;

class ThreeDee {
public:
    EntryD x;
    EntryD y;
    EntryD z;

    ThreeDee(EntryD d0,
             EntryD d1,
             EntryD d2) :
        x(d0),
        y(d1),
        z(d2) {}

    // operator functions
    ThreeDee operator-(const ThreeDee& other) const {
        return ThreeDee(x - other.x,
                        y - other.y,
                        z - other.z);
    }
};

class HailStone {
public:
    ThreeDee position;
    ThreeDee velocity;

    HailStone(EntryD p0,
              EntryD p1,
              EntryD p2,
              EntryD v0,
              EntryD v1,
              EntryD v2) :
        position(p0, p1, p2),
        velocity(v0, v1, v2) {}
};

HailStone parse(const string& line) {
    // assign line to buffer
    stringstream buffer(line);

    // temporary storage
    string tmp;

    // parse numbers
    getline(buffer, tmp, ',');
    EntryD p0 = stod(tmp);

    getline(buffer, tmp, ',');
    EntryD p1 = stod(tmp);

    getline(buffer, tmp, '@');
    EntryD p2 = stod(tmp);

    getline(buffer, tmp, ',');
    EntryD v0 = stod(tmp);

    getline(buffer, tmp, ',');
    EntryD v1 = stod(tmp);

    getline(buffer, tmp, ',');
    EntryD v2 = stod(tmp);

    // return parsed stone
    return HailStone(p0, p1, p2, v0, v1, v2);
}

// helper functions

EntryD crossX(const ThreeDee& a,
              const ThreeDee& b) {
    return (a.y * b.z) -
           (a.z * b.y);
}

EntryD crossY(const ThreeDee& a,
              const ThreeDee& b) {
    return (a.z * b.x) -
           (a.x * b.z);
}

EntryD crossZ(const ThreeDee& a,
              const ThreeDee& b) {
    return (a.x * b.y) -
           (a.y * b.x);
}

// utility functions

optional<ThreeDee> getTwoDeeIntersection(const HailStone& s1,
                                         const HailStone& s2) {
    // calculate determinant using X and Y components
    EntryD determinant = (s1.velocity.x * s2.velocity.y) -
        (s1.velocity.y * s2.velocity.x);

    // check if determinant is 0
    if (determinant == 0.0) {
        return nullopt;
    }

    // calculate time of intersection
    EntryD t1 = ((s2.position.x - s1.position.x)) * s2.velocity.y -
        ((s2.position.y - s1.position.y) * s2.velocity.x);
    t1 /= determinant;

    if (t1 < 0.0) {
        return nullopt;
    }

    // calculate alternate time of intersection
    EntryD t2 = ((s2.position.x - s1.position.x)) * s1.velocity.y -
        ((s2.position.y - s1.position.y) * s1.velocity.x);
    t2 /= determinant;

    if (t2 < 0.0) {
        return nullopt;
    }

    // calculate intersection
    EntryD x = s1.position.x + s1.velocity.x * t1;
    EntryD y = s1.position.y + s1.velocity.y * t1;

    // return intersection
    return ThreeDee(x, y, 0);
}

const EntryD MIN_VALUE = 200'000'000'000'000;
const EntryD MAX_VALUE = 400'000'000'000'000;

int64_t countIntersection(const vector<HailStone>& stones) {
    // setup for iteration
    int64_t intersections = 0;

    // iterate over all pairs of stones
    for (size_t i = 0; i < stones.size(); ++i) {
        // retrieve short-hands
        const HailStone& s1 = stones.at(i);

        for (size_t j = i + 1; j < stones.size(); ++j) {
            // retrieve short-hands
            const HailStone& s2 = stones.at(j);

            // get intersection of two stones
            optional<ThreeDee> intersection = getTwoDeeIntersection(s1, s2);

            // check if intersection exists
            if (intersection) {
                // check for interval
                if ((intersection->x < MIN_VALUE) ||
                    (intersection->x > MAX_VALUE) ||
                    (intersection->y < MIN_VALUE) ||
                    (intersection->y > MAX_VALUE)) {
                    continue;
                }

                // mark as intersection
                intersections += 1;
            }
        }
    }

    // return intersections
    return intersections;
}

/*
 * implement a linear solver, and use it to find the intersection, sigh
 */

MatrixD doGaussJordanInversion(const MatrixD& matrix) {
    // get dimensions
    size_t numberOfRows = matrix.size();
    size_t numberOfCols = matrix.at(0).size();
    size_t extendedSize = numberOfCols + numberOfRows;

    // create new matrix
    MatrixD extended = matrix;
    for (size_t r = 0; r < numberOfRows; ++r) {
        extended.at(r).resize(numberOfCols + numberOfRows);
        extended.at(r).at(numberOfCols + r) = 1.0;
    }

    // perform Gauss-Gordan elimination
    for (size_t iteration = 0; iteration < numberOfRows; iteration++) {
        // check for pivot value
        EntryD value = extended.at(iteration).at(iteration);

        // perform row reduction
        for (size_t c = iteration; c < extendedSize; c++) {
            extended.at(iteration).at(c) /= value;
        }

        // eliminate other rows
        for (size_t c = 0; c < numberOfCols; c++) {
            if (iteration != c) {
                EntryD factor = extended.at(c).at(iteration);

                for (size_t e = 0; e < extendedSize; e++) {
                    extended.at(c).at(e) -= extended.at(iteration).at(e) * factor;
                }
            }
        }
    }

    // retrieve result from extension
    VectorD resultRow(numberOfCols, 0.0);
    MatrixD result(numberOfRows, resultRow);
    for (size_t i = 0; i < numberOfRows; ++i) {
        for (size_t j = 0; j < numberOfCols; ++j) {
            result.at(i).at(j) = extended.at(i).at(numberOfCols + j);
        }
    }

    // return result
    return result;
}

VectorD vectorProduct(const MatrixD& matrix,
                      const VectorD& vector) {
    // get dimensions
    size_t numberOfRows = matrix.size();
    size_t numberOfCols = matrix.at(0).size();

    // create new vector
    VectorD result(numberOfRows, 0.0);

    // perform multiplication
    for (size_t r = 0; r < numberOfRows; ++r) {
        for (size_t c = 0; c < numberOfCols; ++c) {
            result.at(r) += matrix.at(r).at(c) * vector.at(c);
        }
    }

    // return result
    return result;
}

int64_t analyzeStones(const vector<HailStone>& stones,
                      size_t firstIndex,
                      size_t middleIndex,
                      size_t lastIndex) {
    // select the first three stones
    const HailStone& a = stones.at(firstIndex);
    const HailStone& b = stones.at(middleIndex);
    const HailStone& c = stones.at(lastIndex);

    ThreeDee ab_pos = a.position - b.position;
    ThreeDee ab_vel = a.velocity - b.velocity;

    ThreeDee ac_pos = a.position - c.position;
    ThreeDee ac_vel = a.velocity - c.velocity;

    // assemble magic matrix
    MatrixD magicMatrix = {
        { ab_vel.y, -ab_vel.x, 0.0, -ab_pos.y, ab_pos.x, 0.0 },
        { ac_vel.y, -ac_vel.x, 0.0, -ac_pos.y, ac_pos.x, 0.0 },
        { -ab_vel.z, 0.0, ab_vel.x, ab_pos.z, 0.0, -ab_pos.x },
        { -ac_vel.z, 0.0, ac_vel.x, ac_pos.z, 0.0, -ac_pos.x },
        { 0.0, ab_vel.z, -ab_vel.y, 0.0, -ab_pos.z, ab_pos.y },
        { 0.0, ac_vel.z, -ac_vel.y, 0.0, -ac_pos.z, ac_pos.y }
    };

    // assemble magic vector
    VectorD magicVector = {
        crossZ(b.velocity, b.position) - crossZ(a.velocity, a.position),
        crossZ(c.velocity, c.position) - crossZ(a.velocity, a.position),
        crossY(b.velocity, b.position) - crossY(a.velocity, a.position),
        crossY(c.velocity, c.position) - crossY(a.velocity, a.position),
        crossX(b.velocity, b.position) - crossX(a.velocity, a.position),
        crossX(c.velocity, c.position) - crossX(a.velocity, a.position)
    };

    // perform Gauss-Jordan elimination
    MatrixD inverse = doGaussJordanInversion(magicMatrix);
    VectorD product = vectorProduct(inverse, magicVector);

    // calculate result
    EntryD result = 0;
    result += round(product.at(0));
    result += round(product.at(1));
    result += round(product.at(2));

    // return result
    return static_cast<int64_t>(result);
}

/*
 * we did it, I guess?
 */

int main() {
    // temporary storage line
    string line;

    // store hailstones
    vector<HailStone> stones;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // create new stone
        HailStone stone = parse(line);
        stones.emplace_back(stone);
    }

    // setup for iteration
    int64_t firstIntersections = countIntersection(stones);

    // display result
    cout << "[first] total intersections: " << firstIntersections << endl;

    unordered_map<int64_t, int64_t> resultMap;
    optional<int64_t> secondResult;

    // allocate third of a size
    size_t thirdSize = stones.size() / 3;

    // average out error, maybe
    for (size_t i1 = 0; i1 < thirdSize; i1++) {
        for (size_t i2 = thirdSize; i2 < thirdSize * 2; i2++) {
            for (size_t i3 = thirdSize * 2; i3 < stones.size(); i3++) {
                int64_t result = analyzeStones(stones, i1, i2, i3);
                resultMap[result] += 1;
            }
        }
    }

    // locate most common result
    for (const pair<const int64_t, int64_t>& pair : resultMap) {
        if ((!secondResult) ||
            (pair.second > resultMap.at(secondResult.value()))) {
            secondResult = pair.first;
        }
    }

    // display result
    cout << "[second] most probable result: " << secondResult.value() << " @ " << resultMap.at(secondResult.value()) << endl;

    return 0;
}
