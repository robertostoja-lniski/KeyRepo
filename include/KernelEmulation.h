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

#define DEFAULT_MAP_SIZE 128
#define DEFAULT_NUMBER_OF_KEYS 0
enum {
    VERBOSE_NO = 0,
    VERBOSE_LOW = 1,
    VERBOSE_HIGH = 2,
};

#define VERBOSE_LEVEL VERBOSE_NO


struct AddKeyInfo {
    uint64_t id;
    uint64_t numberOfKeys;
};

struct MapNode {
    uint64_t id;
    uint64_t offset {0};
};

struct PartitionInfo {
    uint64_t numberOfKeys {0};
    uint64_t fileContentSize;
    uint64_t mapSize {128};
};

struct KeyNode {
    uint32_t keySize {0};
    std::string keyContent;
};

struct KeyPartitionNode {
    uint32_t keySize {0};
    char data[4096] = "UNDEFINED";
};

static PartitionInfo data;
const std::string partition = "/home/robert/.keyPartition";
const std::string tmpKeyStorage = "/tmp/tmpKeyBeforePart.pem";

// TODO there can be two keys with the same ID
static uint64_t generateRandomId();

int initFileIfNotDefined();
int writeKeyToTemporaryFile(RSA* r);
KeyNode  generateKeyNodeFromKeyInFile();
size_t getFileSize(const char* filename);
uint64_t addKeyNodeToPartition(KeyNode keyNodeToAdd);
void printPartition(const void* mappedPartition);
uint64_t addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode keyNodeToAdd);
std::string getKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
uint64_t readIdFromFile(std::string filepath);
std::string getPrvKeyById(uint64_t id);
int removePrvKeyById(uint64_t id);
void print(std::string str);
std::string getPathToTmpPrvKeyStorage(std::string key);

int getCurrentKeyNumFromEmulation();

// PUBLIC
int getCurrentKeyNum();
AddKeyInfo write(RSA* r);
std::string read(std::string filepath);
std::string get(std::string filepath);
int remove(std::string filepath);

#endif //KEYREPO_KERNELEMULATION_H
