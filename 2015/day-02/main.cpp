#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

class Box {
public:
    // box dimensions
    int64_t height;
    int64_t width;
    int64_t length;

    Box(const std::string& line) {
        // temporary storage
        string tmp;

        // assign line to buffer
        stringstream buffer(line);

        // process box length
        getline(buffer, tmp, 'x');
        length = stoll(tmp);

        // process box width
        getline(buffer, tmp, 'x');
        width = stoll(tmp);

        // process box height
        getline(buffer, tmp, 'x');
        height = stoll(tmp);
    }

    // utility functions
    int64_t getSurfaceArea() {
        int64_t surface = 0;

        // calculate area
        surface += (length * width);
        surface += (width * height);
        surface += (height * length);
        surface *= 2;

        return surface;
    }

    int64_t getPaperSlack() {
        int64_t maximum = max({ length, width, height });
        int64_t minimum = min({ length, width, height });

        int64_t median = (length + width + height);
        median -= maximum;
        median -= minimum;

        // as it turns out, slack is this
        return (minimum * median);
    }

    int64_t getVolume() {
        return (length * width * height);
    }

    int64_t getRibbonBow() {
        int64_t maximum = max({ length, width, height });
        int64_t minimum = min({ length, width, height });

        int64_t median = (length + width + height);
        median -= maximum;
        median -= minimum;

        // as it turns out, slack is this
        return (minimum + median) * 2;
    }
};

int main() {
    // temporary storage
    string line;

    // keep a running tally
    int64_t paperArea = 0;
    int64_t ribbonLength = 0;

    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // temporary storage
        Box box(line);

        // add paper for this box
        paperArea += box.getSurfaceArea();
        paperArea += box.getPaperSlack();

        // add ribbon for this box
        ribbonLength += box.getVolume();
        ribbonLength += box.getRibbonBow();
    }

    // display result for first part
    cout << "wrapping paper required : " << paperArea << endl;
    cout << "ribbon required : " << ribbonLength << endl;

    return 0;
}
