#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

// 3rd-party includes
#include <openssl/evp.h>

string md5Hash(const std::string& key,
               optional<int64_t> number = nullopt) {
    // assemble input
    string input = key;
    if (number) {
        input += to_string(number.value());
    }

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

string md5HashStretched(const std::string& key,
                        int64_t number) {
    // generate first hash
    string hash = md5Hash(key, number);

    // iterate over hash
    for (size_t i = 0; i < 2016; i++) {
        // generate next hash
        hash = md5Hash(hash);
    }

    return hash;
}

const unordered_map<char, int64_t> BIT_MASK = {
    { '0', 1 << 0 },
    { '1', 1 << 1 },
    { '2', 1 << 2 },
    { '3', 1 << 3 },
    { '4', 1 << 4 },
    { '5', 1 << 5 },
    { '6', 1 << 6 },
    { '7', 1 << 7 },
    { '8', 1 << 8 },
    { '9', 1 << 9 },
    { 'a', 1 << 10 },
    { 'b', 1 << 11 },
    { 'c', 1 << 12 },
    { 'd', 1 << 13 },
    { 'e', 1 << 14 },
    { 'f', 1 << 15 },
};

// get triplet bit mask
int64_t getTriplets(const string& hash) {
    // iterate over hash
    for (size_t i = 0; i < hash.size() - 2; i++) {
        // check if triplet
        if ((hash.at(i) == hash.at(i + 1)) && (hash.at(i) == hash.at(i + 2))) {
            // return bit mask
            return BIT_MASK.at(hash.at(i));
        }
    }

    // return 0 if no triplet
    return 0;
}

// get quintuplet bit mask
int64_t getQuintuplets(const string& hash) {
    // iterate over hash
    for (size_t i = 0; i < hash.size() - 4; i++) {
        // check if quintuplet
        if ((hash.at(i) == hash.at(i + 1)) && (hash.at(i) == hash.at(i + 2)) &&
            (hash.at(i) == hash.at(i + 3)) && (hash.at(i) == hash.at(i + 4))) {
            // return bit mask
            return BIT_MASK.at(hash.at(i));
        }
    }

    // return 0 if no quintuplet
    return 0;
}

int64_t generatePad(const string& salt,
                    const function<string(const string&, int64_t)>& hashFunction) {
    // temporary storage
    vector<int64_t> keys;

    // setup for iteration
    vector<int64_t> triplets;
    vector<int64_t> quintuplets;

    // generate first 1000 hashes
    for (int64_t i = 0; i < 1000; i++) {
        // get next hash
        string hash = hashFunction(salt, i);

        // get triplet bit mask
        int64_t tripletBitMask = getTriplets(hash);
        int64_t quintupletBitMask = getQuintuplets(hash);

        // add triplet bit mask to list
        triplets.push_back(tripletBitMask);
        quintuplets.push_back(quintupletBitMask);
    }

    // setup for iteration
    int64_t currentKey = 0;

    while (keys.size() < 64) {
        // generate the hashes 1000 away
        string nextHash = hashFunction(salt, currentKey + 1000);

        // get bit masks
        int64_t tripletBitMask = getTriplets(nextHash);
        int64_t quintupletBitMask = getQuintuplets(nextHash);

        // add to vectors
        triplets.push_back(tripletBitMask);
        quintuplets.push_back(quintupletBitMask);

        // check if currentkey is a triplet
        int64_t currentTriple = triplets.at(currentKey);
        if (currentTriple != 0) {
            for (size_t i = 1; i <= 1000; i++) {
                // retrieve short-hands
                size_t ix = static_cast<size_t>(currentKey + i);

                // check if quintuplet matches
                if ((quintuplets.at(ix) & currentTriple) != 0) {
                    // add key to list
                    keys.push_back(currentKey);
                    break;
                }
            }
        }

        // increment current key
        currentKey += 1;
    }

    return keys.back();
}

int main() {
    // generate first pad
    int64_t firstResult = generatePad("zpqevtbw", md5Hash);
    cout << "[first] last key: " << firstResult << endl;

    // generate second pad
    cout << "[second] this will take a while..." << endl;
    int64_t secondResult = generatePad("zpqevtbw", md5HashStretched);
    cout << "[second] last key: " << secondResult << endl;

    return 0;
}
