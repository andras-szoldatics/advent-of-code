#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Connection {
public:
    bool up;
    bool down;
    bool left;
    bool right;

    Connection() :
        up(false),
        down(false),
        left(false),
        right(false) {}
};

typedef vector<Connection> Connections;
typedef array<size_t, 2> Position;

template<>
struct std::hash<Position> {
    std::size_t operator()(const array<size_t, 2>& a) const {
        std::size_t h1 = std::hash<size_t>() (a[0]);
        std::size_t h2 = std::hash<size_t>() (a[1]);

        return (h1 + h2);
    }
};

class Field {
public:
    vector<Connections> pipes;
    unordered_set<Position> mainLine;

    // utility functions
    int64_t findMainLine(Position start) {
        // temporary storage
        int64_t longestPath = 0;

        // execute simple BFS
        queue<Position> queue;
        queue.push(start);

        // store the distance
        unordered_map<Position, int64_t> distance;
        distance[start] = 0;

        while (!queue.empty()) {
            // retrieve the current position
            Position current = queue.front();
            queue.pop();

            // mark as part of the main line
            mainLine.insert(current);

            // retrieve the current distance
            int64_t currentDistance = distance.at(current);
            if (longestPath < currentDistance) {
                longestPath = currentDistance;
            }

            // retrieve the connections
            Connection currentConnection = getConnection(current);

            // check for up next step - UP
            if (currentConnection.up) {
                // retrieve the next position
                Position next = { current.front() - 1, current.back() };
                Connection nextConnection = getConnection(next);

                // check if this next position is connected backwards
                if (nextConnection.down) {
                    // check if the next position is not visited
                    if (distance.count(next) == 0) {
                        // store the next position
                        distance[next] = currentDistance + 1;

                        // store the next position
                        queue.push(next);
                    }
                } else if (current == start) {
                    // fix start connection
                    pipes.at(current.front()).at(current.back()).up = false;
                }
            }

            // check for down next step - DOWN
            if (currentConnection.down) {
                // retrieve the next position
                Position next = { current.front() + 1, current.back() };
                Connection nextConnection = getConnection(next);

                // check if this next position is connected backwards
                if (nextConnection.up) {
                    // check if the next position is not visited
                    if (distance.count(next) == 0) {
                        // store the next position
                        distance[next] = currentDistance + 1;

                        // store the next position
                        queue.push(next);
                    }
                } else if (current == start) {
                    // fix start connection
                    pipes.at(current.front()).at(current.back()).down = false;
                }
            }

            // check for left next step - LEFT
            if (currentConnection.left) {
                // retrieve the next position
                Position next = { current.front(), current.back() - 1 };
                Connection nextConnection = getConnection(next);

                // check if this next position is connected backwards
                if (nextConnection.right) {
                    // check if the next position is not visited
                    if (distance.count(next) == 0) {
                        // store the next position
                        distance[next] = currentDistance + 1;

                        // store the next position
                        queue.push(next);
                    }
                } else if (current == start) {
                    // fix start connection
                    pipes.at(current.front()).at(current.back()).left = false;
                }
            }

            // check for right next step - RIGHT
            if (currentConnection.right) {
                // retrieve the next position
                Position next = { current.front(), current.back() + 1 };
                Connection nextConnection = getConnection(next);

                // check if this next position is connected backwards
                if (nextConnection.left) {
                    // check if the next position is not visited
                    if (distance.count(next) == 0) {
                        // store the next position
                        distance[next] = currentDistance + 1;

                        // store the next position
                        queue.push(next);
                    }
                } else if (current == start) {
                    // fix start connection
                    pipes.at(current.front()).at(current.back()).right = false;
                }
            }
        }

        return longestPath;
    }

    int64_t findEnclosedTiles() {
        // temporary storage
        int64_t enclosedTiles = 0;
        Position tile;

        // check all the tiles
        for (tile.front() = 0; tile.front() < pipes.size(); tile.front()++) {
            // we go to the left and count the number of up & down connections, on main pipeline
            int64_t pipesUp = 0;
            int64_t pipesDown = 0;

            // check the tile on this row
            for (tile.back() = 0; tile.back() < pipes.at(tile.front()).size(); tile.back()++) {
                // check if this tile is on the main line
                if (mainLine.count(tile) != 0) {
                    // retrieve the connections
                    Connection connection = getConnection(tile);

                    // check for up connection
                    if (connection.up) {
                        pipesUp++;
                    }

                    // check for down connection
                    if (connection.down) {
                        pipesDown++;
                    }
                } else {
                    // calculate inside / outside
                    int64_t matchingPairs = min(pipesUp, pipesDown);
                    if (matchingPairs % 2 == 1) {
                        enclosedTiles += 1;
                    }
                }
            }
        }

        return enclosedTiles;
    }

    // helper functions
    Connection getConnection(Position position) {
        // check against the boundaries
        if (position.front() >= pipes.size()) {
            return Connection();
        }

        if (position.back() >= pipes.at(position.front()).size()) {
            return Connection();
        }

        return pipes.at(position.front()).at(position.back());
    }
};

int main() {
    // temporary storage
    string line;

    // store the pipes
    Field field;
    Position start;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        vector<Connection> connections(line.size());

        // process lien as connections
        for (size_t i = 0; i < line.size(); i++) {
            // retrieve short-hands
            char c = line[i];

            switch (c) {
            case 'S':
                // mark start position
                start = { field.pipes.size(), i };

                // mark connections - 4-way
                connections[i].up = true;
                connections[i].down = true;
                connections[i].left = true;
                connections[i].right = true;
                break;
            case '|':
                // mark connections - 2-way
                connections[i].up = true;
                connections[i].down = true;
                break;
            case '-':
                // mark connections - 2-way
                connections[i].left = true;
                connections[i].right = true;
                break;
            case 'L':
                // mark connections - 2-way
                connections[i].up = true;
                connections[i].right = true;
                break;
            case 'J':
                // mark connections - 2-way
                connections[i].up = true;
                connections[i].left = true;
                break;
            case '7':
                // mark connections - 2-way
                connections[i].down = true;
                connections[i].left = true;
                break;
            case 'F':
                // mark connections - 2-way
                connections[i].down = true;
                connections[i].right = true;
                break;
            default:
                // NO-OP
                break;
            }
        }

        // store the connections
        field.pipes.push_back(connections);
    }

    // find longest path
    int64_t longestPath = field.findMainLine(start);

    // display result
    cout << "[first] longest path: " << longestPath << endl;

    // find enclosed tiles
    int64_t enclosedTiles = field.findEnclosedTiles();

    // display result
    cout << "[second] enclosed tiles: " << enclosedTiles << endl;

    return 0;
}
