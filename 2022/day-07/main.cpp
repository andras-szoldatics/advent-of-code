// C++ standard includes
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace std;

class File {
public:
    // attributes
    string name;
    size_t size;

    File(const string& _name,
         size_t _size) :
        name(_name),
        size(_size) {}
};

class Directory {
public:
    // attributes
    string name;
    size_t totalSize;

    // relations
    shared_ptr<Directory> parent;

    std::unordered_map<string, shared_ptr<Directory>> directories;
    std::unordered_map<string, shared_ptr<File>> files;

    Directory(const string& _name) :
        name(_name) {}

    // utility functions
    void countFileSize(size_t size) {
        // add size to total
        totalSize += size;

        // propagate towards parents
        if (parent) {
            parent->countFileSize(size);
        }
    }
};

int main() {
    // temporary storage
    string line;

    // directory tree root
    shared_ptr<Directory> rootDirectory = make_shared<Directory>("/");

    // list of directories for iteration
    vector<shared_ptr<Directory>> listOfDirectories;
    listOfDirectories.push_back(rootDirectory);

    shared_ptr<Directory> currentDirectory;
    // read input file
    ifstream input("input.txt");
    while (getline(input, line)) {
        // check for specific commands
        if (line.front() == '$') {
            // check for simple commands
            if (line == string("$ cd /")) {
                // switch to root directory
                currentDirectory = rootDirectory;
            } else if (line == string("$ cd ..")) {
                // switch to parent directory
                currentDirectory = currentDirectory->parent;
            } else if (line == string("$ ls")) {
                // NO-OP
                (void) nullptr;
            } else if (line.substr(0, 5) == string("$ cd ")) {
                // we entered a new directory, get name
                string name = line.substr(5);

                if (currentDirectory->directories.count(name) == 0) {
                    // create directory
                    shared_ptr<Directory> directory = make_shared<Directory>(name);
                    directory->parent = currentDirectory;

                    // add directory, if necessary
                    currentDirectory->directories[name] = directory;
                    listOfDirectories.push_back(directory);
                }

                // switch to new directory
                currentDirectory = currentDirectory->directories.at(name);
            }
        } else {
            // parse file names & sizes
            if (line.substr(0, 3) != "dir") {
                // split line into size & name
                size_t splitPosition = line.find(' ');
                string sizeString = line.substr(0, splitPosition);
                size_t size = stoull(sizeString);
                string fileName = line.substr(splitPosition + 1);

                // create file under name
                currentDirectory->files[fileName] = make_shared<File>(fileName, size);

                // count file size
                currentDirectory->countFileSize(size);
            }
        }
    }

    // keep a running count
    const size_t TOTAL_DISK_SPACE = 70000000;
    const size_t BIG_THRESHOLD = 30000000 - (TOTAL_DISK_SPACE - rootDirectory->totalSize);
    const size_t SMALL_THRESHOLD = 100000;

    size_t smallTotalSize = 0;
    shared_ptr<Directory> smallestDirectory;

    // display directores that are valid for this threshold
    for (const shared_ptr<Directory>& directory : listOfDirectories) {
        if (directory->totalSize >= BIG_THRESHOLD) {
            if ((smallestDirectory == nullptr) ||
                (smallestDirectory->totalSize > directory->totalSize)) {
                // select new directory
                smallestDirectory = directory;
            }
        } else if (directory->totalSize <= SMALL_THRESHOLD) {
            smallTotalSize += directory->totalSize;
        }
    }

    // display total size of small directories
    cout << "total size of small directories : " << smallTotalSize << endl;

    // display selected directory
    cout << "smallest directory above threshold : " << smallestDirectory->name << ", size : " << smallestDirectory->totalSize << endl;

    return 0;
}
