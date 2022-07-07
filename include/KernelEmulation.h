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
#define LOOKUP_MAP_SIZE_POW 7
#define LOOKUP_SLOTS_NUM 128

#define KEY_TYPE_CUSTOM 0

#define KEY_TYPE_RSA 1

#define KEY_TYPE_UNKNOWN -1
#define RSA_BEGIN_LABEL "-----BEGIN RSA PRIVATE KEY-----\n"
#define RSA_END_LABEL "-----END RSA PRIVATE KEY-----"

#define REMOVE_FRAGMENTATION 1
#define MAP_UNUSED_ROW_OPTIMISATION 1

extern uint64_t remove_fragmentation_on;
extern uint64_t map_optimisation_on;

enum {
    VERBOSE_NO = 0,
    VERBOSE_LOW = 1,
    VERBOSE_HIGH = 2,
};

enum {
    RES_OK = 0,
    RES_INPUT_ERR = -1,
    RES_CANNOT_OPEN_PARTITION = -1,
    RES_CANNOT_WRITE_TO_PARTITION = -1,
    RES_PARTITION_FULL = -1,
    RES_UNKNOWN_KEY_TYPE = -1,
    RES_CANNOT_ALLOCATE = -1,
    RES_UNAUTHORIZED = -1,
    RES_CANNOT_INITIALIZE_PARTITION = -2,
    RES_NON_INTEGRAL_PARTITION = -1,
    RES_CACHE_ERROR = -1,
    RES_NOT_FOUND = -1,
    RES_CANNOT_DELETE_RESOURCE = -1,

};

// VERBOSE_NO for no prints
// VERBOSE_LOW for the most important info

#define VERBOSE_LEVEL VERBOSE_NO
#define SU_SECURITY 0
#define KEY_PARTITION_NODE_INITIAL_SIZE 4096
#define MAGIC 312312312312
#define AVG_KEY_LEN 4096

struct map_node {
    uint64_t id;
    uint8_t type;
    uint16_t size;
    uint32_t mode;
    uint16_t uid;
    uint64_t gid;
};

struct lookup_slot {
    uint8_t cnt;
};

typedef struct lookup_slot lookup_slot;
typedef struct map_node map_node;

struct partition_info {
    uint64_t magic;
    uint16_t number_of_keys;
    uint16_t map_size;
};
typedef struct partition_info partition_info;

#if __APPLE__
const static char* partition = "/Users/robertostoja-lniski/.keyPartitionV2/meta";
const static char* partition_base = "/Users/robertostoja-lniski/.keyPartitionV2/";
#else
const static char* partition = "/home/robert/.keyPartition";
#endif

static struct semaphore *sem;

struct access_rights {
    int uid;
    int gid;
};
typedef struct access_rights access_rights;


int init_file_if_not_defined();
int add_key_to_partition(const char* key, uint64_t keyLen, uint64_t* id, access_rights, uint8_t type);
void print_partition(const void* mapped_partition);
int update_metadata_when_writing(void* mapped_partition, const char* key, uint64_t keyLen, uint64_t* id, access_rights, uint8_t type);
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t keyLen, access_rights, uint8_t* type);
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id, access_rights);
int remove_private_key_by_id(uint64_t id, access_rights);
uint64_t remove_fragmentation(partition_info*);
// PUBLIC
int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* output, access_rights);
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int mode, access_rights);
int can_read(int mode, access_rights mapped, access_rights effective);
int can_write(int mode, access_rights mapped, access_rights effective);

int do_write_key(const char* key, const char* password, const uint64_t keyLen, uint64_t* id, int uid, int gid, int type);
int do_read_key(const uint64_t id, const char* password, char* key, uint64_t keyLen, int uid, int gid);
int do_remove_key(const uint64_t id, const char* password, int uid, int gid);
int do_get_key_size(const uint64_t id, const char* password, uint64_t* size, int uid, int gid);
// MODE HANDLING
int do_get_mode(const uint64_t id, const char* password, int* output, int uid, int gid);
int do_set_mode(const uint64_t id, const char* password, int new_mode, int uid, int gid);
int do_get_key_num();

#endif //KEYREPO_KERNELEMULATION_H
