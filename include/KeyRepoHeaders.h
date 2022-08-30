
#ifndef KEYREPO_KEYREPOHEADERS_H
#define KEYREPO_KEYREPOHEADERS_H

#include "linux/kernel.h"
#include "linux/syscalls.h"
#include "linux/cred.h"
#include "linux/mman.h"
#include "linux/types.h"
#include "linux/fcntl.h"
#include "linux/stat.h"
#include "linux/string.h"
#include "linux/random.h"
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/stat.h>

#define LOOKUP_MAP_SIZE_POW 7
#define MAX_FILENAME_LEN 128
#define VERBOSE_LEVEL VERBOSE_NO
#define SU_SECURITY 0
#define DEFAULT_MODE 600
#define DEFAULT_MAP_SIZE 128
#define DEFAULT_NUMBER_OF_KEYS DEFAULT_MAP_SIZE
#define MAX_KEY_NUM DEFAULT_MAP_SIZE
#define AVG_KEY_LEN 4096
#define KEY_PARTITION_NODE_INITIAL_SIZE AVG_KEY_LEN + 128
#define MAX_PARTITION_SIZE MAX_KEY_NUM * AVG_KEY_LEN
#define REDUCTION_PARAM 2
#define READ_MASK	4
#define WRITE_MASK	2
#define MAGIC 112359876543
#define AVG_KEY_LEN 4096
#define REMOVE_FRAGMENTATION  1
#define LOOKUP_MAP_SIZE_POW 7
#define LOOKUP_SLOTS_NUM MAX_KEY_NUM

#define RSA_BEGIN_LABEL "-----BEGIN RSA PRIVATE KEY-----\n"
#define RSA_END_LABEL "-----END RSA PRIVATE KEY-----"

#define KEY_TYPE_CUSTOM 0
#define KEY_TYPE_RSA 1

enum {
    VERBOSE_NO = 0,
    VERBOSE_LOW = 1,
    VERBOSE_HIGH = 2,
};

enum {
    RES_OK = 0,
    RES_INPUT_ERR = -1,

    RES_CANNOT_OPEN = -2,
    RES_CANNOT_READ = -2,
    RES_CANNOT_WRITE = -3,
    RES_CANNOT_DELETE = -4,

    RES_PARTITION_FULL = -5,
    RES_CANNOT_INIT = -6,
    RES_NON_INTEGRAL = -7,

    RES_NO_KEY_TYPE = -8,
    RES_UNAUTHORIZED = -9,
    RES_NOT_FOUND = -10,

    RES_CANNOT_ALLOC = -11,
};

//static kuid_t getuid(void);
//static kgid_t getgid(void);
//static kuid_t geteuid(void);

#define KEY_FILE_NAME_LEN 32

struct user_info {
    int uid;
    int gid;
};
typedef struct user_info user_info;

struct key_info {
    char key_file[KEY_FILE_NAME_LEN];
    uint8_t type;
    uint16_t size;
    uint32_t access_control_list;
    user_info owner_info;
};


typedef struct key_info key_info;

struct map_node {
    uint64_t id;
    key_info key_info;
};



struct lookup_slot {
    uint8_t cnt;
};

typedef struct lookup_slot lookup_slot;
typedef struct map_node map_node;

struct partition_info {
    uint64_t magic;
    uint16_t number_of_keys;
    uint16_t capacity;
    int32_t freed_slot;
};
typedef struct partition_info partition_info;

const static char* partition = "/krepo/meta";
const static char* partition_base = "/krepo/";

static struct semaphore sem;

struct metadata {
    user_info user_info;
    int type;
};

typedef struct metadata metadata;

int add_key_to_partition(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, user_info, uint8_t type);
void print_partition(const void* mapped_partition);
int update_metadata_when_writing(partition_info* partition_start, const char* key, uint64_t keyLen, uint64_t* id, user_info, uint8_t type);
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t keyLen, user_info, uint8_t* type);
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id, user_info);
int remove_private_key_by_id(uint64_t id, user_info);
uint64_t remove_fragmentation(partition_info*);
// PUBLIC
int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* output, user_info);
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int mode, user_info);
int can_read(int mode, user_info owner_info, user_info effective_user_info);
int can_write(int mode, user_info owner_info, user_info effective_user_info);

#endif //KEYREPO_KEYREPOHEADERS_H
