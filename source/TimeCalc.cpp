//
// Created by Robert Ostoja-Lniski on 1/1/21.
//

#include "../include/TimeCalc.h"

TimeCalc::~TimeCalc() {}

void EmulationTimeCalc::writeKeyTime(unsigned int trials) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    uint64_t write_keyTime = 0;
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            system("rm -rf ~/.keyPartitionV2/*");
            uint64_t dummy;
            std::string initStr = "null";
            std::string pass = "dummydummy123";
            auto initRet = write_key(initStr.c_str(), initStr.size(), pass.c_str(), pass.size(), &dummy, KEY_TYPE_CUSTOM);
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
        std::string pass = "dummydummy123";
        auto start = std::chrono::high_resolution_clock::now();
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        auto singleWrite = std::chrono::high_resolution_clock::now() - start;

        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        write_keyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();

    }

    std::cout << "Write key avg time (" << testedSize << ") " << write_keyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}
void EmulationTimeCalc::readKeyTime(unsigned int trials) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    uint64_t read_keyTime = 0;
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {
            system("rm -rf ~/.keyPartitionV2/*");
            uint64_t dummy;
            std::string initStr = "null";
            std::string pass = "dummydummy123";
            auto initRet = write_key(initStr.c_str(), initStr.size(), pass.c_str(), pass.size(), &dummy, KEY_TYPE_CUSTOM);
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
        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        char *buf;
        buf = (char* )malloc(testedSize);
        auto start = std::chrono::high_resolution_clock::now();
        auto readRet = read_key(buf, id, pass.c_str(), pass.size(), testedSize);
        auto singleWrite = std::chrono::high_resolution_clock::now() - start;

        if(readRet != 0) {
            throw std::runtime_error("Error while testing emulation read");
        }

        free(buf);

        read_keyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();

    }

    std::cout << "Read key avg time (" << testedSize << ") " << read_keyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}
void EmulationTimeCalc::removeKeyTime(unsigned int trials) {

    if(trials < 64) {
        throw std::runtime_error("Use at least 64 trials");
    }

    if(trials >= 254) {
        throw std::runtime_error("Use no more than 254 trials - bigger size not supported");
    }
    // create 1/2 trials of keys of 4096

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    uint64_t remove_keyTime = 0;
    uint64_t ids[200];
    for(int i = 1; i <= trials; i++) {

        if(i % 127 == 0) {

            uint64_t keyNum;
            auto keyNumRet = get_key_num(&keyNum);
            if (keyNumRet < 0) {
                throw std::runtime_error("Error when getting key num");
            }

            while(keyNum > 0) {

                auto start = std::chrono::high_resolution_clock::now();
                auto removeRet = remove_key(ids[keyNum]);
                auto singleRemove = std::chrono::high_resolution_clock::now() - start;

                if(removeRet != 0) {
                    throw std::runtime_error("Error while testing emulation remove");
                }

                remove_keyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();

                keyNumRet = get_key_num(&keyNum);
                if (keyNumRet) {
                    throw std::runtime_error("Error when getting key num");
                }
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

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[i], KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }
    }

    std::cout << "Remove key avg time (" << testedSize << ") " << remove_keyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}
void EmulationTimeCalc::getKeySizeTime(unsigned int trials) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t id;
    uint64_t getKeySizeTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm -rf ~/.keyPartitionV2/* ");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        uint64_t size;

        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = get_key_size(id, &size);
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        getKeySizeTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Get key size time (" << testedSize << ") " << getKeySizeTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

void EmulationTimeCalc::getKeyNumTime(unsigned int trials) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t id;
    uint64_t getKeySizeTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm -rf ~/.keyPartitionV2/* ");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        uint64_t keyNum;
        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = get_key_num(&keyNum);
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        getKeySizeTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Get key num size time (" << testedSize << ") " << getKeySizeTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

uint64_t EmulationTimeCalc::unusedMapRowOptimisation(unsigned int trials) {

    auto randomLetter = []() -> char {
        std::default_random_engine eng(std::random_device{}());
        std::uniform_int_distribution<> dist(0, 'z' - 'a');

        return (char) ('a' + dist(eng));
    };

    uint64_t optimisationTime = 0;

    for(int j = 0; j < trials; j++) {



        system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
        uint64_t ids[200];
        for (int i = 1; i <= 127; i++) {

            if (i % 127 == 0) {
                uint64_t keyNum;
                auto keyNumRet = get_key_num(&keyNum);
                if (keyNumRet < 0) {
                    throw std::runtime_error("Error when get key num");
                }

                while (keyNum > 0) {

                    auto removeRet = remove_key(ids[keyNum]);
                    if (removeRet != 0) {
                        throw std::runtime_error("Error while testing testing optimisation");
                    }

                    keyNumRet = get_key_num(&keyNum);
                    // omits half of keys
                    keyNum -= 3;
                }
                break;
            }

            std::string key(testedSize, 0);
            std::generate_n(key.begin(), testedSize, randomLetter);

            std::string pass = "dummydummy123";
            auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[i], KEY_TYPE_CUSTOM);
            if (ret != 0) {
                throw std::runtime_error("Error while testing optimisation of emulation");
            }
        }


        for (int i = 0; i < 30; i++) {

            std::string key(testedSize, 0);
            std::generate_n(key.begin(), testedSize, randomLetter);

            char *buf;
            buf = (char *) malloc(testedSize);
            uint64_t id;
            std::string pass = "dummydummy123";
            auto start = std::chrono::high_resolution_clock::now();
            auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
            auto singleWrite = std::chrono::high_resolution_clock::now() - start;
            if (ret != 0) {
                throw std::runtime_error("Error while testing emulation read");
            }

            free(buf);
            optimisationTime += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();
        }
        system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
//        std::cout << "... " << (j + 1) * 100 / trials << "%" << std::endl;
    }
    std::cout << "Map free slot optimisation (" << testedSize << ") " << (optimisationTime / (30 * trials)) << " microseconds" << std::endl;
    return optimisationTime/30;
}

void EmulationTimeCalc::defragmentationOptimisation(unsigned int trials, unsigned int scenario_id) {

    uint64_t time = 0;

    for(int i = 0; i < trials; i ++) {

        system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
        uint64_t ids[200];
        for (int j = 0; j < 126; j++) {

            auto randomLetter = []() -> char {
                std::default_random_engine eng(std::random_device{}());
                std::uniform_int_distribution<> dist(0, 'z' - 'a');

                return (char) ('a' + dist(eng));
            };
            std::string key(testedSize, 0);
            std::generate_n(key.begin(), testedSize, randomLetter);

            std::string pass = "dummydummy123";
            auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[j], KEY_TYPE_CUSTOM);
            if (ret != 0) {
                throw std::runtime_error("Error while testing emulation");
            }
        }

        for (int j = 0; j < 100; j++) {
            auto removeRet = remove_key(ids[j]);
            if (removeRet != 0) {
                throw std::runtime_error("Error while testing emulation remove");
            }
        }

        for (int j = 100; j < 126; j++) {

            if (scenario_id == 1) {

                auto start = std::chrono::high_resolution_clock::now();
                auto removeRet = remove_key(ids[j]);
                auto singleRemove = std::chrono::high_resolution_clock::now() - start;

                if (removeRet != 0) {
                    throw std::runtime_error("Error while testing emulation read");
                }

                time += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();

            } else if (scenario_id == 2) {

                char *buf;
                buf = (char *) malloc(testedSize);
                std::string pass = "dummydummy123";
                auto start = std::chrono::high_resolution_clock::now();
                auto readRet = read_key(buf, ids[j], pass.c_str(), pass.size(), testedSize);
                auto singleWrite = std::chrono::high_resolution_clock::now() - start;

                if (readRet != 0) {
                    throw std::runtime_error("Error while testing emulation read");
                }

                free(buf);

                auto removeRet = remove_key(ids[j]);

                time += std::chrono::duration_cast<std::chrono::microseconds>(singleWrite).count();

            } else if (scenario_id == 3) {

                auto removeRet = remove_key(ids[j]);
                if (removeRet != 0) {
                    throw std::runtime_error("Error while testing emulation remove key");
                }

                uint64_t keyNum;
                auto start = std::chrono::high_resolution_clock::now();
                auto getNumRet = get_key_num(&keyNum);
                auto singleRead = std::chrono::high_resolution_clock::now() - start;

                if (getNumRet < 0) {
                    throw std::runtime_error("Error while testing emulation get key num");
                }

                time += std::chrono::duration_cast<std::chrono::microseconds>(singleRead).count();

            } else {
                throw std::runtime_error("Not known scenario");
            }
        }

        std::cout << "... " << (i + 1) * 100 / trials << "%" << std::endl;
    }

    std::cout << "Defragmentation optimisation " << scenario_id << " trial: " << trials << " (" << testedSize << ") " << time / (27 * (uint64_t)trials) << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

void EmulationTimeCalc::setTestedSize(uint64_t size) {
    testedSize = size;
}

void EmulationTimeCalc::getKeyModeTime(unsigned int trials) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t id;
    uint64_t getKeyModTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm -rf ~/.keyPartitionV2/* ");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        int mod;

        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = get_mode(id, &mod);
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        getKeyModTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Get key mode time (" << testedSize << ") " << getKeyModTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

int dummy(int);

void EmulationTimeCalc::setKeyModeTime(unsigned int trials) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t id;
    uint64_t setKeyModTime = 0;
    for(int i = 0; i < trials; i++) {

        if(i != 0 && i % 100 == 0) {
            system("rm -rf ~/.keyPartitionV2/* ");
        }

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &id, KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

        auto randomMod = []() -> int
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 10000);

            return dist(eng) % 778;
        };

        auto newMode = randomMod();
        auto start = std::chrono::high_resolution_clock::now();
        auto getSizeRet = set_mode(id, newMode);
//        auto dummyRet = dummy(randomMod());
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;

        setKeyModTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();
    }

    std::cout << "Set key mode time (" << testedSize << ") " << setKeyModTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

void EmulationTimeCalc::test(unsigned int trials) {
    writeKeyTime(trials);
    readKeyTime(trials);
    removeKeyTime(trials);
    getKeySizeTime(trials);
    getKeyModeTime(trials);
    setKeyModeTime(trials);
    getKeyNumTime(trials);
}

int dummy(int newMode) {
    if(newMode > 777) {
        return -1;
    }
    int hDigit = (newMode / 100);
    int dDigit = (newMode / 10) % 10;
    int digit = newMode % 10;

    if(!(hDigit <= 7 && dDigit <= 7 && digit <= 7)) {
        return -1;
    }

    return 0;
}

void EmulationTimeCalc::cacheTest(unsigned int trials) {

    if(trials < 64) {
        throw std::runtime_error("Use at least 64 trials");
    }

    // create 1/2 trials of keys of 4096

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::vector<uint64_t> ids(trials);
    for(int i = 0; i < 32; i++) {

        auto randomLetter = []() -> char
        {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char)('a' +  dist(eng));
        };
        std::string key(testedSize,0);
        std::generate_n( key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[i], KEY_TYPE_CUSTOM);
        if(ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }
    }

    uint64_t remove_keyTime = 0;
    for (int i = 0; i < trials; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        auto volatile removeRet = remove_key(10000000000 + trials);
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;
        remove_keyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();;
    }

    std::cout << "Cache remove (" << testedSize << ") " << remove_keyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

void EmulationTimeCalc::freedSlotTest(unsigned int trials) {

    if (trials < 64) {
        throw std::runtime_error("Use at least 64 trials");
    }

    // create 1/2 trials of keys of 4096

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::vector<uint64_t> ids(trials);
    int size = 32;
    for (int i = 0; i < size; i++) {

        auto randomLetter = []() -> char {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char) ('a' + dist(eng));
        };
        std::string key(testedSize, 0);
        std::generate_n(key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[i], KEY_TYPE_CUSTOM);
        if (ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }

    }

    uint64_t remove_keyTime = 0;
    for (int i = 0; i < trials; i++) {

        auto volatile removeRet = remove_key(ids[size - 1]);

        auto randomLetter = []() -> char {
            std::default_random_engine eng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, 'z' - 'a');

            return (char) ('a' + dist(eng));
        };
        std::string key(testedSize, 0);
        std::generate_n(key.begin(), testedSize, randomLetter);

        std::string pass = "dummydummy123";

        auto start = std::chrono::high_resolution_clock::now();
        auto ret = write_key(key.c_str(), key.size(), pass.c_str(), pass.size(), &ids[size - 1], KEY_TYPE_CUSTOM);
        if (ret != 0) {
            throw std::runtime_error("Error while testing emulation");
        }
        auto singleRemove = std::chrono::high_resolution_clock::now() - start;
        remove_keyTime += std::chrono::duration_cast<std::chrono::microseconds>(singleRemove).count();;
    }

    std::cout << "Freed slot (" << testedSize << ") " << remove_keyTime / trials << " microseconds" << std::endl;

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
