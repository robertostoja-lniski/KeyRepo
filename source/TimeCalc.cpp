//
// Created by Robert Ostoja-Lniski on 1/1/21.
//

#include "../include/TimeCalc.h"

TimeCalc::~TimeCalc() {}

void EmulationTimeCalc::writeKeyTime(unsigned int trials) {

    system("mv ~/.keyPartition ~/.keyPartition.old");
    uint64_t writeKeyTime = 0;
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            system("rm ~/.keyPartition");
            uint64_t dummy;
            std::string initStr = "null";
            auto initRet = writeKey(initStr.c_str(), initStr.size(), &dummy);
            if(initRet != 0) {
                throw std::runtime_error("Error in time calc while initialising");
            }
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(4096,0);
        std::generate_n( key.begin(), 4096, randomLetter);

        uint64_t id;
        auto start = std::chrono::high_resolution_clock::now();
        auto ret = writeKey(key.c_str(), key.size(), &id);
        auto singleWrite = std::chrono::high_resolution_clock::now() - start;

        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        writeKeyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();

    }

    std::cout << "Write key avg time (4096) " << writeKeyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");

}
void EmulationTimeCalc::readKeyTime(unsigned int trials) {

    system("mv ~/.keyPartition ~/.keyPartition.old");
    uint64_t readKeyTime = 0;
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            system("rm ~/.keyPartition");
            uint64_t dummy;
            std::string initStr = "null";
            auto initRet = writeKey(initStr.c_str(), initStr.size(), &dummy);
            if(initRet != 0) {
                throw std::runtime_error("Error in time calc while initialising");
            }
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        uint64_t id;
        auto ret = writeKey(key.c_str(), key.size(), &id);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        char *buf;
        buf = (char* )malloc(testedSize);
        auto start = std::chrono::high_resolution_clock::now();
        auto readRet = readKey(id, buf, testedSize);
        auto singleWrite = std::chrono::high_resolution_clock::now() - start;

        if(readRet != 0) {
            throw std::runtime_error("Error while testing emulation read");
        }

        free(buf);

        readKeyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();

    }

    std::cout << "Read key avg time (4096) " << readKeyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");

}
void EmulationTimeCalc::removeKeyTime(unsigned int trials) {

    if(trials < 128) {
        throw std::runtime_error("Use at least 128 trials");
    }

    if(trials >= 254) {
        throw std::runtime_error("Use no more than 254 trials - bigger size not supported");
    }
    // create 1/2 trials of keys of 4096

    system("mv ~/.keyPartition ~/.keyPartition.old");
    uint64_t removeKeyTime = 0;
    uint64_t ids[200];
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            auto keyNum = getCurrentKeyNumFromEmulation();
            while(keyNum > 0) {

                auto start = std::chrono::high_resolution_clock::now();
                auto removeRet = removeKey(ids[keyNum]);
                auto singleRemove = std::chrono::high_resolution_clock::now() - start;

                if(removeRet != 0) {
                    throw std::runtime_error("Error while testing emulation remove");
                }

                removeKeyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();

                keyNum = getCurrentKeyNumFromEmulation();
            }
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        auto ret = writeKey(key.c_str(), key.size(), &ids[i]);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }
    }

    std::cout << "Remove key avg time (4096) " << removeKeyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");

}
void EmulationTimeCalc::getKeySizeTime(unsigned int trials) {

    system("mv ~/.keyPartition ~/.keyPartition.old");

    uint64_t id;
    uint64_t getKeySizeTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm ~/.keyPartition");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        auto ret = writeKey(key.c_str(), key.size(), &id);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        uint64_t size;

        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = getKeySize(id, &size);
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        getKeySizeTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Get key size time (4096) " << getKeySizeTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");
}

void EmulationTimeCalc::getCurrentKeyNumFromEmulationTime(unsigned int trials) {

    system("mv ~/.keyPartition ~/.keyPartition.old");

    uint64_t id;
    uint64_t getKeySizeTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm ~/.keyPartition");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        auto ret = writeKey(key.c_str(), key.size(), &id);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        uint64_t size;

        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = getCurrentKeyNumFromEmulation();
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        getKeySizeTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Get key num size time (4096) " << getKeySizeTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");
}

void EmulationTimeCalc::unusedMapRowOptimisation(unsigned int trials) {

    if(trials < 128) {
        throw std::runtime_error("Use at least 128 trials");
    }

    if(trials >= 254) {
        throw std::runtime_error("Use no more than 254 trials - bigger size not supported");
    }

    auto randomLetter = []() -> char
    {
        std::default_random_engine eng(std::random_device{}());
        std::uniform_int_distribution<> dist(0, 'z' - 'a');

        return (char)('a' +  dist(eng));
    };

    system("mv ~/.keyPartition ~/.keyPartition.old");
    uint64_t optimisationTime = 0;
    uint64_t ids[200];
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            auto keyNum = getCurrentKeyNumFromEmulation();
            while(keyNum > 0) {

                auto removeRet = removeKey(ids[keyNum]);
                if(removeRet != 0) {
                    throw std::runtime_error("Error while testing testing optimisation");
                }

                keyNum = getCurrentKeyNumFromEmulation();
                // omits half of keys
                keyNum -= 3;
            }
            break;
        }

        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        auto ret = writeKey(key.c_str(), key.size(), &ids[i]);
        if(ret != 0) {
            throw std::runtime_error("Error while testing optimisation of emulation");
        }
    }


    for(int i = 0; i < 30; i++) {

        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        char *buf;
        buf = (char* )malloc(testedSize);
        uint64_t id;
        auto start = std::chrono::high_resolution_clock::now();
        auto ret = writeKey(key.c_str(), key.size(), &id);
        auto singleWrite = std::chrono::high_resolution_clock::now() - start;
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation read");
        }

        free(buf);
        optimisationTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();
    }

    std::cout << "Map free slot optimisation (4096) " << optimisationTime / 30 << " microseconds" << std::endl;

    system("mv ~/.keyPartition.old ~/.keyPartition");
}
