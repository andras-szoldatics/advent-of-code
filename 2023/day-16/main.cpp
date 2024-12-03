#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef array<int64_t, 2> Position;

enum class Section : uint8_t {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Ray {
public:
    Position position;
    Section section;

    Ray(Position p = { 0, 0 },
        Section s = Section::LEFT) :
        position(p),
        section(s) {}

    // helper functions
    Ray reverse() const {
        // temporary storage
        Section tmp;

        switch (section) {
        case Section::UP:
            tmp = Section::DOWN;
            break;
        case Section::DOWN:
            tmp = Section::UP;
            break;
        case Section::LEFT:
            tmp = Section::RIGHT;
            break;
        case Section::RIGHT:
            tmp = Section::LEFT;
            break;
        }

        return Ray(position, tmp);
    }

    // operator functions
    bool operator==(const Ray& rhs) const {
        return (position == rhs.position) &&
               (section == rhs.section);
    }
};

// extend hash function to Position, Ray

namespace std {

template<>
struct hash<Position> {
    size_t operator()(const Position& p) const {
        size_t bitfield = hash<int64_t>() (p.front());
        bitfield += hash<int64_t>() (p.back());
        return bitfield;
    }
};

template<>
struct hash<Ray> {
    size_t operator()(const Ray& r) const {
        size_t bitfield = hash<Position>() (r.position);
        bitfield += hash<uint8_t>() (static_cast<uint8_t>(r.section));
        return bitfield;
    }
};

} // namespace std

class Cave {
public:
    unordered_map<Position, char> map;
    Position extent;

    Cave() :
        extent({ 0, 0 }) {}

    // helper functions
    bool isInside(const Position& p) const {
        if (p.front() < 0) {
            return false;
        }

        if (p.back() < 0) {
            return false;
        }

        if (p.front() >= extent.front()) {
            return false;
        }

        if (p.back() >= extent.back()) {
            return false;
        }

        return true;
    }

    // utility functions
    vector<Ray> splitRay(const Ray& ray) const {
        if (map.count(ray.position) == 0) {
            return { ray.reverse() };
        }

        // retrieve short-hands
        char c = map.at(ray.position);
        if (c == '|') {
            if ((ray.section == Section::UP) ||
                (ray.section == Section::DOWN)) {
                return { ray.reverse() };
            } else {
                // split into two rays
                return { Ray(ray.position, Section::UP),
                         Ray(ray.position, Section::DOWN) };
            }
        }

        if (c == '-') {
            if ((ray.section == Section::LEFT) ||
                (ray.section == Section::RIGHT)) {
                return { ray.reverse() };
            } else {
                // split into two rays
                return { Ray(ray.position, Section::LEFT),
                         Ray(ray.position, Section::RIGHT) };
            }
        }

        if (c == '/') {
            switch (ray.section) {
            case Section::UP:
                return { Ray(ray.position, Section::LEFT) };
            case Section::DOWN:
                return { Ray(ray.position, Section::RIGHT) };
            case Section::LEFT:
                return { Ray(ray.position, Section::UP) };
            case Section::RIGHT:
                return { Ray(ray.position, Section::DOWN) };
                break;
            }
        }

        if (c == '\\') {
            switch (ray.section) {
            case Section::UP:
                return { Ray(ray.position, Section::RIGHT) };
            case Section::DOWN:
                return { Ray(ray.position, Section::LEFT) };
            case Section::LEFT:
                return { Ray(ray.position, Section::DOWN) };
            case Section::RIGHT:
                return { Ray(ray.position, Section::UP) };
                break;
            }
        }

        return {};
    }

    optional<Ray> followRay(const Ray& ray) const {
        // temporary storage
        Position position = ray.position;
        Section section = ray.section;

        switch (ray.section) {
        case Section::UP:
            position.front() -= 1;
            break;
        case Section::DOWN:
            position.front() += 1;
            break;
        case Section::LEFT:
            position.back() -= 1;
            break;
        case Section::RIGHT:
            position.back() += 1;
            break;
        }

        if (isInside(position)) {
            Ray ray(position, section);
            return ray.reverse();
        }

        return nullopt;
    }
};

size_t bounceLight(const Cave& cave,
                   const Ray& start) {
    // setup for iteration
    stack<Ray> simulation;
    simulation.push(start);

    // keep track of visited positions
    unordered_set<Position> visitedSquares;
    unordered_set<Ray> visitedRays;

    // iterate until queue is empty
    while (!simulation.empty()) {
        // retrieve next ray
        Ray ray = simulation.top();
        simulation.pop();

        // check if ray has been visited
        if (visitedRays.count(ray) != 0) {
            continue;
        } else {
            visitedRays.insert(ray);
        }

        // mark if square has been visited
        visitedSquares.insert(ray.position);

        // first - split the ray
        vector<Ray> rays = cave.splitRay(ray);

        // second - follow the rays
        for (const Ray& entry : rays) {
            optional<Ray> next = cave.followRay(entry);
            if (next.has_value()) {
                simulation.push(next.value());
            }
        }
    }

    return visitedSquares.size();
}

size_t findBestLight(const Cave& cave) {
    // temporary storage
    size_t bestResult = 0;

    // try to shoot DOWN from each position
    for (int64_t x = 0; x < cave.extent.front(); x++) {
        Ray start({ 0, x }, Section::UP);
        size_t result = bounceLight(cave, start);
        bestResult = max(bestResult, result);
    }

    // try to shoot RIGHT from each position
    for (int64_t y = 0; y < cave.extent.back(); y++) {
        Ray start({ y, 0 }, Section::LEFT);
        size_t result = bounceLight(cave, start);
        bestResult = max(bestResult, result);
    }

    // try to shoot UP from each position
    for (int64_t x = 0; x < cave.extent.front(); x++) {
        Ray start({ cave.extent.back() - 1, x }, Section::DOWN);
        size_t result = bounceLight(cave, start);
        bestResult = max(bestResult, result);
    }

    // try to shoot LEFT from each position
    for (int64_t y = 0; y < cave.extent.back(); y++) {
        Ray start({ y, cave.extent.front() - 1 }, Section::RIGHT);
        size_t result = bounceLight(cave, start);
        bestResult = max(bestResult, result);
    }

    return bestResult;
}

int main() {
    // temporary storage
    string line;

    // cave storage
    Cave cave;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // process line
        Position cursor = { cave.extent.front(), 0 };
        for (char c : line) {
            if (c != '.') {
                cave.map[cursor] = c;
            }

            // move cursor
            cursor.back() += 1;
        }

        // extend cave
        cave.extent.front() += 1;
        cave.extent.back() = max(cave.extent.back(), static_cast<int64_t>(line.size()));
    }

    // run simulation
    Ray start({ 0, 0 }, Section::LEFT);
    size_t result = bounceLight(cave, start);

    // display result
    cout << "[first] number of tiles energized: " << result << endl;

    // try to find best start
    size_t bestResult = findBestLight(cave);

    // display result
    cout << "[second] number of tiles energized: " << bestResult << endl;

    return 0;
}
