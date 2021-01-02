
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
#define KEY_PARTITION_NODE_INITIAL_SIZE 4096 + 128
#define MAX_PARTITION_SIZE MAX_KEY_NUM * 4096
#define REDUCTION_PARAM 2
#define READ_MASK	4
#define WRITE_MASK	2
#endif

static kuid_t getuid(void);
static kgid_t getgid(void);
static kuid_t geteuid(void);

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
const static char* partition = ".keyPartition";

int canRead(int mode, kuid_t uid, kgid_t gid);
int canWrite(int mode, kuid_t uid, kgid_t gid);
int initFileIfNotDefined(void);
int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t* id);
int addKeyNodeToPartition(struct KeyNode* keyNodeToAdd, uint64_t* id);
int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, char* keyVal, uint64_t keyLen);
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
int getKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int** output);
int setKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int mode);
uint64_t removeFragmentation(PartitionInfo* );
int removePrvKeyById(uint64_t id);

#endif //KEYREPO_KEYREPOHEADERS_H
