#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

// 3rd-party includes
#include <openssl/evp.h>

using namespace std;

string md5Hash(const std::string& key,
               int64_t number) {
    // assemble input
    string input = key;
    input += to_string(number);

    // calculate hash for input
    EVP_MD_CTX* md5Context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);
    EVP_DigestUpdate(md5Context, input.c_str(), static_cast<unsigned long>(input.size()));
    uint resultSize = EVP_MD_size(EVP_md5());
    unsigned char result[resultSize];
    EVP_DigestFinal_ex(md5Context, result, &resultSize);

    // assemble output as string
    std::stringstream output;
    output << std::hex << std::setfill('0');

    for (const auto& byte : result) {
        output << std::setw(2) << int(byte);
    }

    return output.str();
}

class Decoder {
public:
    // attributes
    string id;

    // state
    int64_t index;

    Decoder(const string& line) :
        id(line),
        index(0) {}

    // helper functions
    string nextHash() {
        // temporary storage
        string foundHash;

        while (foundHash.empty()) {
            // temporary storage
            string tmp = md5Hash(id, index);

            // check for validity
            if (tmp.substr(0, 5) == string("00000")) {
                // mark found hash
                foundHash = tmp;
            }

            // shift index
            index += 1;
        }

        return foundHash;
    }

    // utility functions
    string decodeFirst() {
        // reset index
        index = 0;

        // temporary storage
        string password(8, '_');

        // iterate over password width
        for (size_t i = 0; i < 8; i++) {
            // cycle until next hash is available
            string hash = nextHash();

            // retrieve password part
            password.at(i) = hash.at(5);

            // display progress
            cout << "$: " << password << endl;
        }

        return password;
    }

    string decodeSecond() {
        // reset index
        index = 0;

        // temporary storage
        unordered_map<int64_t, char> characters;
        string password(8, '_');

        while (characters.size() < 8) {
            // cycle until next hash is available
            string hash = nextHash();

            // retrieve password part
            char position = hash.at(5);
            char character = hash.at(6);

            if ((position >= '0') &&
                (position <= '7')) {
                // calculate key for map
                int64_t key = (position - '0');

                if (characters.count(key) == 0) {
                    // add character into map
                    characters[key] = character;

                    // replace character in password
                    password.at(key) = character;

                    // display progress
                    cout << "$: " << password << endl;
                }
            }
        }

        return password;
    }
};

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read first & only relevant line
    getline(input, line);

    // setup decoder
    Decoder decoder(line);

    // execute first step
    string passwordFirst = decoder.decodeFirst();
    cout << "[first] decoded password : " << passwordFirst << endl;

    // execute second step
    string passwordSecond = decoder.decodeSecond();
    cout << "[second] decoded password : " << passwordSecond << endl;

    return 0;
}
