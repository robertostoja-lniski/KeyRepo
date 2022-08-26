
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

#ifndef VERBOSE_LEVEL
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
#endif

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

const static char* partition = "/krepo";

int init_file_if_not_defined(void);
int add_key_to_partition(const char* key, uint64_t keyLen, uint64_t* id, access_rights);
void print_partition(const void* mapped_partition);
int add_key_by_partition_pointer(void* mapped_partition, const char* key, uint64_t keyLen, uint64_t* id, access_rights);
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t keyLen, access_rights);
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id, access_rights);
int remove_private_key_by_id(uint64_t id, access_rights);
uint64_t remove_fragmentation(partition_info*);
// PUBLIC
int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* output, access_rights);
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int mode, access_rights);
int can_read(int mode, access_rights mapped, access_rights effective);
int can_write(int mode, access_rights mapped, access_rights effective);

#endif //KEYREPO_KEYREPOHEADERS_H
