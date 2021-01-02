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

#define REMOVE_FRAGMENTATION 1
#define MAP_UNUSED_ROW_OPTIMISATION 1

extern uint64_t remove_fragmentation_on;
extern uint64_t map_optimisation_on;

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

struct map_node {
    uint64_t id;
    uint64_t offset;
    uint64_t size;
    uint32_t mode;
    int      uid;
    int      gid;
};
typedef struct map_node map_node;

struct partition_info {
    uint64_t number_of_keys;
    uint64_t file_content_size;
    uint64_t free_slot;
    uint64_t map_size;
};
typedef struct partition_info partition_info;

#if __APPLE__
const static char* partition = "/Users/robertostoja-lniski/.keyPartition";
#else
const std::string partition = ".keyPartition";
#endif

int init_file_if_not_defined();
int add_key_to_partition(const char* key, uint64_t keyLen, uint64_t* id);
void print_partition(const void* mapped_partition);
int add_key_by_partition_pointer(void* mapped_partition, const char* key, uint64_t keyLen, uint64_t* id);
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t keyLen);
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id);
int remove_private_key_by_id(uint64_t id);
int get_key_num();
uint64_t remove_fragmentation(partition_info*);
// PUBLIC
int write_key(const char* key, const size_t keyLen, uint64_t* id);
int read_key(const uint64_t id, char* key, uint64_t keyLen);
int remove_key(const uint64_t id);
int get_key_size(const uint64_t id, uint64_t* size);
// MODE HANDLING
int get_mode(const uint64_t id, int* output);
int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* output);
int set_mode(const uint64_t id, int new_mode);
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int mode);
int can_read(int mode, int uid, int gid);
int can_write(int mode, int uid, int gid);



#endif //KEYREPO_KERNELEMULATION_H
