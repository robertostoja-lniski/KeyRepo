//
// Created by robert on 27.05.2020.
//

#ifndef KEYREPO_KERNELEMULATION_H
#define KEYREPO_KERNELEMULATION_H

#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <random>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

class KernelEmulation {

private:
    struct MapNode {
        uint64_t id;
        uint64_t offset {0};
        MapNode() {
            std::random_device dev;
            std::mt19937 gen(dev());
            std::uniform_int_distribution<uint64_t> uDist(0, UINT64_MAX);
            id = uDist(gen);
        }
        friend std::ostream &operator<<(std::ostream &os, const MapNode &node) {
            os << "\nid: " << node.id << " offset: " << node.offset;
            return os;
        }
    };

    struct PartitionInfo {
        uint64_t numberOfKeys {0};
        uint64_t fileContentSize;
        uint64_t mapSize {128};

        PartitionInfo() {
            fileContentSize = mapSize * sizeof(MapNode);
        }

        friend std::ostream &operator<<(std::ostream &os, const PartitionInfo &info) {
            os << "number of keys: " << info.numberOfKeys << '\n'
               << "offset to add next key " << info.fileContentSize << '\n'
               << "max number of nodes " << info.mapSize << '\n';
            return os;
        }
    };

    struct KeyNode {
        uint32_t keySize {0};
        std::string keyContent;

        friend std::ostream &operator<<(std::ostream &os, const KeyNode &node) {
            os << node.keySize << node.keyContent;
            return os;
        }
    };

    struct KeyPartitionNode {
        uint32_t keySize {0};
        char data[4096];
    };

    const std::string partition = "/home/robert/.keyPartition";
    const std::string tmpKeyStorage = "/tmp/tmpKeyBeforePart.pem";
    int initFileIfNotDefined() {
        if(std::experimental::filesystem::exists(partition)) {
            return 1;
        }
        auto partitionInfo = std::make_unique<PartitionInfo>();
        auto fileSize = sizeof(PartitionInfo) + sizeof(KeyNode) * partitionInfo->mapSize;
        //Open file
        std::ofstream os(partition.c_str());
        os << "";
        os.close();

        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }
        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }

        if(fileSize != getFileSize(partition.c_str())) {
            return 1;
        }

        void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
        if(mappedPartition == MAP_FAILED) {
            close(fd);
            return 1;
        }

        auto partitionPtr = partitionInfo.get();
        auto size = sizeof(*partitionPtr);
        auto mappedPart = mappedPartition;
        memcpy(mappedPartition, partitionInfo.get(), sizeof(*partitionInfo.get()));
        MapNode* mapPosition = (MapNode* )((PartitionInfo* )mappedPartition + 1);
        for(int i = 0; i < partitionInfo->mapSize; i++) {
            auto mapData = std::make_unique<MapNode>();
            memcpy(mapPosition, mapData.get(), sizeof(*mapData));
            mapPosition = mapPosition + 1;
        }

        int ret = munmap(mappedPartition, fileSize);
        assert(ret == 0);
        close(fd);
    }

    int writeKeyToTemporaryFile(RSA* r) {
        FILE *fp = fopen(tmpKeyStorage.c_str(), "wb");
        if(fp == nullptr) {
            return 1;
        }
        auto success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
        if(!success) {
            return 1;
        }
        fclose(fp);
    }

    KeyNode  generateKeyNodeFromKeyInFile() {
        KeyNode keyNode;
        std::string line;
        std::ifstream keyTmpFile(tmpKeyStorage);
        if (keyTmpFile.is_open()) {
            while (getline(keyTmpFile,line)) {
                keyNode.keyContent += line;
            }
            keyTmpFile.close();
        }
        // when serialised there will be no access to bytes as std::string
        keyNode.keySize = keyNode.keyContent.size();
        return keyNode;
    }

    size_t getFileSize(const char* filename) {
        struct stat st;
        stat(filename, &st);
        return st.st_size;
    }

    uint64_t addKeyNodeToPartition(KeyNode keyNodeToAdd) {
        auto fileSize = getFileSize(partition.c_str()) + keyNodeToAdd.keySize + sizeof(keyNodeToAdd.keySize);
        //Open file
        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }
        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }

        if(fileSize != getFileSize(partition.c_str())) {
            return 1;
        }
        std::cout << std::endl << "file size is " << fileSize << std::endl;
        void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
        if(mappedPartition == MAP_FAILED) {
            close(fd);
            return 1;
        }

        auto id = addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd);

        int ret = munmap(mappedPartition, fileSize);
        assert(ret == 0);
        close(fd);

        return id;
    }

    uint64_t addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode keyNodeToAdd) {

        auto partitionInfo = (PartitionInfo* )mappedPartition;
        auto keys = partitionInfo->numberOfKeys;
        auto offsetToAdd = partitionInfo->fileContentSize;
        auto mapSize = partitionInfo->mapSize;

        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t id;
        for(int i = 0; i < mapSize; i++) {
            auto offset = currentElementInMap->offset;
            if(offset == 0) {
                id = currentElementInMap->id;
                break;
            }
            currentElementInMap = currentElementInMap + 1;
        }
        auto foundId = currentElementInMap->id;
        auto elementDataToUpdate = std::make_unique<MapNode>();
        elementDataToUpdate->offset = offsetToAdd;
        elementDataToUpdate->id = foundId;
        memcpy(currentElementInMap, elementDataToUpdate.get(), sizeof(MapNode));
        KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offsetToAdd);
        memcpy(keyPlaceToAdd->data, keyNodeToAdd.keyContent.c_str(), keyNodeToAdd.keySize);
        auto partitionInfoToUpdate = std::make_unique<PartitionInfo>();
        partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
        partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
        partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + keyNodeToAdd.keySize;
        memcpy(partitionInfo, partitionInfoToUpdate.get(), sizeof(PartitionInfo));

        std::cout << "partition after adding a key: " << *partitionInfo << std::endl;
        std::cout << "id of added key is: " << id << std::endl;
        return id;
    }

    std::string getKeyValByPartitionPointer(void* mappedPartition, uint64_t id) {

        auto partitionInfo = (PartitionInfo* )mappedPartition;
        auto mapSize = partitionInfo->mapSize;

        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t offset;
        for(int i = 0; i < mapSize; i++) {
            auto currentId = currentElementInMap->id;
            if(currentId == id) {
                offset = currentElementInMap->offset;
                break;
            }
            currentElementInMap = currentElementInMap + 1;
        }
//        std::cout << "offset is " << offset << std::endl;
        KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offset);
        return keyPlaceToAdd->data;
    }

    uint64_t readIdFromFile(std::string filepath) {
        std::string line;
        std::ifstream myfile (filepath);
        if (myfile.is_open()) {
            getline (myfile,line);
            myfile.close();
            uint64_t id;
            std::istringstream iss(line);
            iss >> id;
            return id;
        }
        return 0;
    }

    std::string getPrvKeyById(uint64_t id) {
        auto fileSize = getFileSize(partition.c_str());
        //Open file
        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return "";
        }
        void* mappedPartition = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
        if(mappedPartition == MAP_FAILED) {
            close(fd);
            return "";
        }

        std::string key = getKeyValByPartitionPointer(mappedPartition, id);

        int ret = munmap(mappedPartition, fileSize);
        assert(ret == 0);
        close(fd);

        return key;
    }

    void print(std::string str) {
        int charsInALine = 30;
        for(int i = 0; i < str.size(); i++) {
            std::cout << str[i];
            if(i != 0 && i % charsInALine == 0) {
                std::cout << std::endl;
            }
        }
    }

    std::string getPathToTmpPrvKeyStorage(std::string key) {
        std::string header = "-----BEGIN RSA PRIVATE KEY-----";
        std::string bottom = "-----END RSA PRIVATE KEY-----";
        int charsInRow = 64;
        std::string filepath = "/tmp/prvKey.pem";
        std::ofstream os;
        os.open(filepath);
        os << header << "\n";
        std::cout << header << "\n";
        for(int i = header.size(); i < key.size() - header.size() - bottom.size();) {
            std::string nextLine;
            for(int j = 0; j < charsInRow; j++) {
                nextLine += key[i];
                i++;
            }
            os << nextLine << "\n";
            std::cout << nextLine << "\n";
        }
        os << bottom << "\n";
        std::cout << bottom << "\n";
        os.close();
        return filepath;
    }

public:
    KernelEmulation() {
        initFileIfNotDefined();
    }

    uint64_t write(RSA* r) {
        writeKeyToTemporaryFile(r);
        auto keyNode = generateKeyNodeFromKeyInFile();
        std::cout << "Kernel will add a key to partition with value" << std::endl;
//        print(keyNode.keyContent);
        return addKeyNodeToPartition(keyNode);
    }
    std::string read(std::string filepath) {
        uint64_t id = readIdFromFile(filepath);
        std::cout << "Kernel Emualtion will give key with id: " <<  id << std::endl;
        std::string prvKey = getPrvKeyById(id);
        std::cout << "Kernel Emulation found a key with value" << std::endl;
//        print(prvKey);
        return getPathToTmpPrvKeyStorage(prvKey);
    }

};
#endif //KEYREPO_KERNELEMULATION_H
