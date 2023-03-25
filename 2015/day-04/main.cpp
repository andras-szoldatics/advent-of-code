// C++ standard includes
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

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
    output << std::hex << std::uppercase << std::setfill('0');

    for (const auto& byte : result) {
        output << std::setw(2) << int(byte);
    }

    return output.str();
}

int main() {
    // temporary storage
    string line;

    // read input file
    ifstream input("input.txt");

    // read first & only relevant line
    getline(input, line);

    bool foundFirst = false;
    bool foundSecond = false;
    // iterate over possible numbers
    for (int64_t number = 1; number < 10'000'000; number++) {
        // calculate hash
        string hash = md5Hash(line, number);

        if (!foundFirst) {
            if (hash.substr(0, 5) == string("00000")) {
                cout << "found first number : " << number << endl;
                foundFirst = true;
            }
        }

        if (!foundSecond) {
            if (hash.substr(0, 6) == string("000000")) {
                cout << "found second number : " << number << endl;
                foundSecond = true;
            }
        }

        // check for both finds
        if ((foundFirst) && (foundSecond)) {
            break;
        }
    }

    return 0;
}
