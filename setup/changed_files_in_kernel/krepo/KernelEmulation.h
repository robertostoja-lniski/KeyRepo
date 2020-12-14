//
// Created by robert on 27.05.2020.
//

#ifndef KEYREPO_KERNELEMULATION_H
#define KEYREPO_KERNELEMULATION_H

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
#define DEFAULT_MODE 600

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
    uint64_t mapSize;
};
typedef struct PartitionInfo PartitionInfo;

struct KeyNode {
    uint32_t keySize;
    char keyContent[4096];
};
typedef struct KeyNode KeyNode;

struct KeyPartitionNode {
    char data[4096];
};
typedef struct KeyPartitionNode KeyPartitionNode;

static PartitionInfo data;
#if __APPLE__
const static char* partition = "/Users/robertostoja-lniski/.keyPartition";
#else
const std::string partition = ".keyPartition";
#endif





#endif //KEYREPO_KERNELEMULATION_H
