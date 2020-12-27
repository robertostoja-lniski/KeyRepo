//
// Created by robert on 27.05.2020.
//

#ifndef KEYREPO_KERNELEMULATION_H
#define KEYREPO_KERNELEMULATION_H

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
#include <stdlib.h>

#define MAX_KEY_NUM 128
#define NODE_SIZE 2048
#define DEFAULT_MAP_SIZE MAX_KEY_NUM
#define DEFAULT_NUMBER_OF_KEYS 0
#define MAX_PARTITION_SIZE MAX_KEY_NUM * 4096
#define REDUCTION_PARAM 2
#define READ_MASK       4
#define WRITE_MASK      2

enum {
    VERBOSE_NO = 0,
    VERBOSE_LOW = 1,
    VERBOSE_HIGH = 2,
};

// VERBOSE_NO for no prints
// VERBOSE_LOW for the most important info

#define VERBOSE_LEVEL VERBOSE_NO
#define SU_SECURITY 0
#define KEY_PARTITION_NODE_INITIAL_SIZE 4096

struct MapNode {
    uint64_t id;
    uint64_t offset;
    uint64_t size;
    uint32_t mode;
    int      uid;
    int      gid;
};
typedef struct MapNode MapNode;

struct PartitionInfo {
    uint64_t numberOfKeys;
    uint64_t fileContentSize;
    uint64_t freeSlot;
    uint64_t mapSize;
};
typedef struct PartitionInfo PartitionInfo;

struct KeyPartitionNode {
    char data[KEY_PARTITION_NODE_INITIAL_SIZE];
};
typedef struct KeyPartitionNode KeyPartitionNode;

static PartitionInfo data;
#if __APPLE__
const static char* partition = "/Users/robertostoja-lniski/.keyPartition";
#else
const std::string partition = ".keyPartition";
#endif

int initFileIfNotDefined();
int addKeyNodeToPartition(char* key, uint64_t keyLen, uint64_t* id);
void printPartition(const void* mappedPartition);
int addKeyNodeByPartitionPointer(void* mappedPartition, char* key, uint64_t keyLen, uint64_t* id);
int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, char* keyVal, uint64_t keyLen);
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
int removePrvKeyById(uint64_t id);
int getCurrentKeyNumFromEmulation();
uint64_t removeFragmentation(PartitionInfo* );
// PUBLIC
int writeKey(const char* key, const size_t keyLen, uint64_t* id);
int readKey(const uint64_t id, char* key, uint64_t keyLen);
int removeKey(const uint64_t id);
int getKeySize(const uint64_t id, uint64_t* size);
// MODE HANDLING
int getMode(const uint64_t id, int* output);
int getKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int* output);
int setMode(const uint64_t id, int newMode);
int setKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int mode);

int canRead(int mode, int uid, int gid);
int canWrite(int mode, int uid, int gid);



#endif //KEYREPO_KERNELEMULATION_H
