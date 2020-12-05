//
// Created by robert on 27.05.2020.
//

#ifndef KEYREPO_KERNELEMULATION_H
#define KEYREPO_KERNELEMULATION_H

#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <random>

#if __APPLE__
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"
#else
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#endif

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_KEY_NUM 128
#define NODE_SIZE 2048
#define DEFAULT_MAP_SIZE MAX_KEY_NUM
#define DEFAULT_NUMBER_OF_KEYS 0
#define REDUCTION_PARAM 2

enum {
    VERBOSE_NO = 0,
    VERBOSE_LOW = 1,
    VERBOSE_HIGH = 2,
};

// VERBOSE_NO for no prints
// VERBOSE_LOW for the most important info

#define VERBOSE_LEVEL VERBOSE_NO
#define SU_SECURITY 0

struct MapNode {
    uint64_t id;
    uint64_t offset {0};
    uint64_t size {0};
    uint32_t mode {0};
    int      uid {0};
    int      gid {0};
};

struct PartitionInfo {
    uint64_t numberOfKeys {0};
    uint64_t fileContentSize;
    uint64_t mapSize {MAX_KEY_NUM};
};

struct KeyNode {
    uint32_t keySize {0};
    char keyContent[4096];
};

struct KeyPartitionNode {
    char data[4096] = "UNDEFINED";
};

static PartitionInfo data;
#if __APPLE__
const std::string partition = "/Users/robertostoja-lniski/.keyPartition";
#else
const std::string partition = "/home/robert/.keyPartition";
#endif
const static char* tmpKeyStorage = "/tmp/tmpKeyBeforePart.pem";
const static char* pathToPrivateKey = "/tmp/prvKey.pem";

// TODO there can be two keys with the same ID
static uint64_t generateRandomId();
static int getDefaultMode() {
    return 600;
}

int initFileIfNotDefined();
int writeKeyToTemporaryFile(RSA* r);
int generateKeyNodeFromKeyInFile(KeyNode**);
size_t getFileSize(const char* filename);
int addKeyNodeToPartition(KeyNode* keyNodeToAdd, uint64_t** id);
void printPartition(const void* mappedPartition);
int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t** id);
int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, KeyPartitionNode** keyVal);
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
int getPrvKeyById(uint64_t id, const char **prvKey);
int removePrvKeyById(uint64_t id);
void print(std::string str);
int storeKey(char* key);
int isPartitionFull();
int getPathToTmpPrvKeyStorage(char* key);
int getCurrentKeyNumFromEmulation();
uint64_t removeFragmentation(PartitionInfo* );
// PUBLIC
int getCurrentKeyNum();
int write(RSA* r, uint64_t** id);
int readKey(const uint64_t* id, char** outpath);
int get(const uint64_t* id, char** output);
int remove(const uint64_t* id, const char* filepath);
// MODE HANDLING
int getMode(const uint64_t* id, int** output);
int getKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int** output);
int setMode(const uint64_t* id, int* newMode);
int setKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int mode);

int canRead(int mode, int uid, int gid);
int canWrite(int mode, int uid, int gid);

#define READ_MASK       4
#define WRITE_MASK      2

#endif //KEYREPO_KERNELEMULATION_H
