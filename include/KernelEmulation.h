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
            os << node.id << node.offset;
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
            os << info.numberOfKeys << '\n' << info.fileContentSize << '\n' << info.mapSize << '\n';
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
        char* data {nullptr};
    };

    const std::string partition = "/home/robert/.keyPartition";
    const std::string tmpKeyStorage = "/home/robert/.key";
    void initFileIfNotDefined() {
        if(std::experimental::filesystem::exists(partition)) {
            return;
        }
        auto partitionInfo = std::make_unique<PartitionInfo>();
        auto fileSize = sizeof(PartitionInfo) + sizeof(KeyNode) * partitionInfo->mapSize;
        //Open file
        std::ofstream os(partition.c_str());
        os << "";
        os.close();

        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            throw std::runtime_error("Cannot open partition for truncation");
        }
        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            throw std::runtime_error("Cannot open partition for mapping");
        }

        if(fileSize != getFileSize(partition.c_str())) {
            throw std::runtime_error("Truncation failed");
        }

        void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
        if(mappedPartition == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Cannot map file");
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

    void writeKeyToTemporaryFile(RSA* r) {
        FILE *fp = fopen(tmpKeyStorage.c_str(), "wb");
        if(fp == nullptr) {
            throw std::runtime_error("Cannot create file for emulation");
        }
        auto success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
        if(!success) {
            throw std::runtime_error("Cannot get prv key");
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
        auto fileSize = getFileSize(partition.c_str()) + keyNodeToAdd.keySize * 2;
        //Open file
        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            throw std::runtime_error("Cannot open partition for truncation");
        }
        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            throw std::runtime_error("Cannot open partition for mapping");
        }

        if(fileSize != getFileSize(partition.c_str())) {
            throw std::runtime_error("Truncation failed");
        }

        void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
        if(mappedPartition == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Cannot map file");
        }

        addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd);

        int ret = munmap(mappedPartition, fileSize);
        assert(ret == 0);
        close(fd);
    }

    uint64_t addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode keyNodeToAdd) {

        auto partitionInfo = (PartitionInfo* )mappedPartition;
        auto keys = partitionInfo->numberOfKeys;
        auto content = partitionInfo->fileContentSize;
        auto mapSize = partitionInfo->mapSize;

        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t id;
        for(int i = 0; i < mapSize; i++) {
            auto offset = currentElementInMap->offset;
            if(offset == 0) {
                break;
//                std::cout << id << std::endl;
            }
            currentElementInMap = currentElementInMap + 1;
        }
        auto foundId = currentElementInMap->id;
        auto elementDataToUpdate = std::make_unique<MapNode>();
        elementDataToUpdate->offset = content;
        elementDataToUpdate->id = foundId;
        memcpy(currentElementInMap, elementDataToUpdate.get(), sizeof(MapNode));
//        currentElementInMap->offset = content;
//        partitionInfo->fileContentSize = content + keyNodeToAdd.keySize + sizeof(keyNodeToAdd.keySize);

        KeyPartitionNode* keyPlaceToAdd = (KeyPartitionNode* )(partitionInfo + sizeof(MapNode) * mapSize);
        auto keyPartitionNode = std::make_unique<KeyPartitionNode>();
        auto size = sizeof(*(keyPartitionNode->data)) * keyNodeToAdd.keySize;
        keyPartitionNode->data = (char* )malloc(size);
        if(!keyPartitionNode->data) {
            throw std::runtime_error("Malloc failed");
        }

        memcpy(keyPartitionNode->data, keyNodeToAdd.keyContent.c_str(), size);
        keyPartitionNode->keySize = keyNodeToAdd.keySize;

        memcpy(keyPlaceToAdd, 0x00, sizeof(uint32_t));
        memcpy(keyPlaceToAdd, keyPartitionNode.get(), sizeof(KeyPartitionNode));

        std::cout << '\n' << (KeyPartitionNode* )keyPlaceToAdd->data << '\n';
        auto partitionInfoToUpdate = std::make_unique<PartitionInfo>();
        partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
        partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
        partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + size;

        memcpy(partitionInfo, partitionInfoToUpdate.get(), sizeof(PartitionInfo));

        free(keyPartitionNode->data);
        auto dummy = 0;
//        auto
    }

public:
    KernelEmulation() {
        initFileIfNotDefined();
    }

    uint64_t write(RSA* r) {
        writeKeyToTemporaryFile(r);
        auto keyNode = generateKeyNodeFromKeyInFile();
        auto id = addKeyNodeToPartition(keyNode);
    }

};
#endif //KEYREPO_KERNELEMULATION_H
