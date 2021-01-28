
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

//static kuid_t getuid(void);
//static kgid_t getgid(void);
//static kuid_t geteuid(void);

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
    uint64_t magic;
    uint64_t number_of_keys;
    uint64_t file_content_size;
    uint64_t free_slot;
    uint64_t map_size;
};
typedef struct partition_info partition_info;

struct access_rights {
    int uid;
    int gid;
};
typedef struct access_rights access_rights;

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
