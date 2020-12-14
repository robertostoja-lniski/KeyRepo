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

static char *load_file(char* filename, int *input_size)
{
        struct kstat *stat;
        struct file *fp;
        mm_segment_t fs;
        loff_t pos = 0;
        char *buf;

        fp = filp_open(filename, O_RDWR, 0644);
                if (IS_ERR(fp)) {
                        printk("Open file error!\n");
                        return ERR_PTR(-ENOENT);
        }

        fs = get_fs();
        set_fs(KERNEL_DS);
        
        stat =(struct kstat *) kmalloc(sizeof(struct kstat), GFP_KERNEL);
        if (!stat)
                return ERR_PTR(-ENOMEM);

        vfs_stat(filename, stat);
        *input_size = stat->size;

        buf = kmalloc(*input_size, GFP_KERNEL);
                if (!buf) {
                        kfree(stat);
                        printk("malloc input buf error!\n");
                        return ERR_PTR(-ENOMEM);
                }
        kernel_read(fp, buf, *input_size, &pos);

        filp_close(fp, NULL);
        set_fs(fs);
        kfree(stat);
        return buf;
}

struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   

int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}


static kuid_t getuid(void)
{
    return current_uid();
}

static kgid_t getgid(void)
{
    return current_gid();
}

/*
static kgid_t getegid(void)
{
    return current_egid();
}
*/

static kuid_t geteuid(void)
{
    return current_euid();
}


#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL VERBOSE_NO
#define SU_SECURITY 0
#define DEFAULT_MODE 600
#define DEFAULT_MAP_SIZE 128
#define REDUCTION_PARAM 2
#define READ_MASK	4
#define WRITE_MASK	2
#endif

struct MapNode {
    uint64_t id;
    uint64_t offset;
    uint64_t size;
    uint32_t mode;
    kuid_t   uid;
    kgid_t   gid;
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
const static char* partition = ".keyPartition";
#endif

int canRead(int mode, kuid_t uid, kgid_t gid);
int canWrite(int mode, kuid_t uid, kgid_t gid);
size_t getFileSize(const char* filename);
int initFileIfNotDefined(void);
int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t** id);
int addKeyNodeToPartition(struct KeyNode* keyNodeToAdd, uint64_t** id);
int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, KeyPartitionNode** keyVal, uint64_t* keyLen);
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id);
int getKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int** output);
int setKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int mode);
uint64_t removeFragmentation(PartitionInfo* );
int removePrvKeyById(uint64_t id);

uint64_t generateRandomId(void* mappedPartition){

    int generateTrials = 10;
    int i = 0;
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;
    int foundSameId = 0;
    uint64_t newId;

    while(generateTrials--) {

        newId = 0;
        for (i = 0; i<64; i++) {
	    int num;
	    get_random_bytes(&num, sizeof(num));
            newId = newId*2 + num%2;
        }

        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t id;
        for(i = 0; i < mapSize; i++) {
            uint64_t offset = currentElementInMap->offset;
            id = currentElementInMap->id;

            // 0 and 1 for error codes
            if(id == newId || id == 0 || id == 1) {
                foundSameId = 1;
            }
        }

        if(!foundSameId) {
            break;
        }
    }

    return newId;
}

int initFileIfNotDefined() {
    struct file *file;
    if ((file = file_open(partition, O_RDWR, 0)))
    {
        file_close(file);
        return 1;
    }

    PartitionInfo* partitionInfo = (PartitionInfo* )kmalloc(sizeof(PartitionInfo), GFP_KERNEL);
    if(!partitionInfo) {
        return 1;
    }

    partitionInfo->mapSize = DEFAULT_MAP_SIZE;
    partitionInfo->numberOfKeys = 0;
    partitionInfo->fileContentSize = DEFAULT_MAP_SIZE * sizeof(MapNode) + sizeof(PartitionInfo);

    uint64_t fileSize = partitionInfo->fileContentSize;
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

    struct FILE* fp = sys_open(partition, O_RDWR, 0);
    // fputs("EMPTY PARTITION\n", fp);
    sys_close(fp);

    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }
    sys_ftruncate(fd, fileSize);
    sys_close(fd);

    fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }

    if(fileSize != getFileSize(partition)) {
        return 1;
    }

    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
    memset(mappedPartition, 0x00, fileSize);

    void* partitionStart = mappedPartition;
    if(mappedPartition == -1) {
        sys_munmap(mappedPartition, fileSize);
        close(fd);
        return 1;
    }

    memcpy(mappedPartition, partitionInfo, sizeof(PartitionInfo));
    MapNode* mapPosition = (MapNode* )((PartitionInfo* )mappedPartition + 1);
    int i;
    for( i=0; i < partitionInfo->mapSize; i++) {
        MapNode* mapData = (MapNode* )kmalloc(sizeof(MapNode), GFP_KERNEL);
        if(!mapData) {
            sys_munmap(mappedPartition, fileSize);
            sys_close(fd);
            return 1;
        }
        mapData -> id = 0;
        mapData -> offset = 0;
        memcpy(mapPosition, mapData, sizeof(*mapData));
        mapPosition = mapPosition + 1;
        kfree(mapData);
    }

    kfree(partitionInfo);
    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);
    return 0;
}


size_t getFileSize(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

SYSCALL_DEFINE0(get_current_key_num) {
    return data.numberOfKeys;
}

uint64_t removeFragmentation(PartitionInfo* partitionInfo) {

    // create new map
    MapNode* tmpMap = (MapNode* )kmalloc(sizeof(MapNode) * partitionInfo->numberOfKeys, GFP_KERNEL);
    if(!tmpMap) {
        return 0;
    }

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    int tmpMapIndex = 0;
    uint64_t changedOffset = DEFAULT_MAP_SIZE * sizeof(MapNode) + sizeof(PartitionInfo);

    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        uint64_t id = currentElementInMap->id;
        uint64_t size = currentElementInMap->size;
        int mode = currentElementInMap->mode;
        kuid_t uid = currentElementInMap->uid;
        kgid_t gid = currentElementInMap->gid;


        if(offset != 0) {

            uint8_t* keyContent = (uint8_t* )kmalloc(size, GFP_KERNEL);
            if(!keyContent) {
                kfree(tmpMap);
                return 0;
            }

            memcpy(keyContent, ((uint8_t* )partitionInfo + offset), size);
            memcpy(((uint8_t* )partitionInfo + changedOffset), keyContent, size);

            kfree(keyContent);

            tmpMap[tmpMapIndex].id = id;
            tmpMap[tmpMapIndex].offset = changedOffset;
            tmpMap[tmpMapIndex].size = size;
            tmpMap[tmpMapIndex].mode = mode;
            tmpMap[tmpMapIndex].uid = uid;
            tmpMap[tmpMapIndex].gid = gid;

            changedOffset += size;
//            changedOffset += changedOffset % NODE_SIZE;
            tmpMapIndex++;
        }
        currentElementInMap = currentElementInMap + 1;
    }

    currentElementInMap = (MapNode* )(partitionInfo + 1);
    memcpy(currentElementInMap, tmpMap, sizeof(MapNode) * partitionInfo->numberOfKeys);
    memset(currentElementInMap + partitionInfo->numberOfKeys, 0x00, sizeof(MapNode) * (DEFAULT_MAP_SIZE - partitionInfo->numberOfKeys));

    kfree(tmpMap);

    return changedOffset;
}

int addKeyNodeToPartition(KeyNode* keyNodeToAdd, uint64_t** id) {

    initFileIfNotDefined();
    if(data.numberOfKeys == DEFAULT_MAP_SIZE) {
        return -2;
    }

    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }

    size_t fileSize = getFileSize(partition);
    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    void* partitionStart = mappedPartition;

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t usedFileSize = partitionInfo->fileContentSize;

    int isSizeExtNeeded = fileSize < usedFileSize + keyNodeToAdd->keySize;

    int retSizeCheck = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    if(isSizeExtNeeded) {

        int fd = sys_open(partition, O_RDWR, 0);
        if(fd < 0) {
            return -1;
        }

        fileSize = getFileSize(partition) + keyNodeToAdd->keySize;
        sys_ftruncate(fd, fileSize);
        sys_close(fd);

        fd = sys_open(partition, O_RDWR, 0);
        if(fd < 0) {
            return -1;
        }
    }

    if(fileSize != getFileSize(partition)) {
        return -1;
    }

    mappedPartition = sys_mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    partitionStart = mappedPartition;

    if(mappedPartition == -1) {
        sys_close(fd);
        return -1;
    }
    int addRet = addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd, id);


    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    return addRet;
}


int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t** id) {
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t nextId;
    uint64_t prevOffset = currentElementInMap->offset;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;

        if(offset == 0) {
            nextId = generateRandomId(mappedPartition);
            *id = (uint64_t* )kmalloc(sizeof(*id), GFP_KERNEL);
            if(!id) {
                return -2;
            }
            **id = nextId;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
        prevOffset = offset;
    }

    MapNode* elementDataToUpdate = (MapNode* )kmalloc(sizeof(MapNode), GFP_KERNEL);
    if(!elementDataToUpdate) {
        return -1;
    }

    elementDataToUpdate->offset = offsetToAdd;
    elementDataToUpdate->id = nextId;
    elementDataToUpdate->size = keyNodeToAdd->keySize;
    elementDataToUpdate->uid = getuid();
    elementDataToUpdate->gid = getgid();
    elementDataToUpdate->mode = DEFAULT_MODE;

    memcpy(currentElementInMap, elementDataToUpdate, sizeof(MapNode));
    kfree(elementDataToUpdate);


    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offsetToAdd);
    memcpy(keyPlaceToAdd, keyNodeToAdd->keyContent, keyNodeToAdd->keySize);

    PartitionInfo* partitionInfoToUpdate = (PartitionInfo* )kmalloc(sizeof(PartitionInfo), GFP_KERNEL);
    partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
    partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
    partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + keyNodeToAdd->keySize;
    memcpy(partitionInfo, partitionInfoToUpdate, sizeof(PartitionInfo));
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

    kfree(partitionInfoToUpdate);
    kfree(keyNodeToAdd);

    return 0;
}

int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, KeyPartitionNode** keyVal, uint64_t* keyLen) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    int found = 0;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;
        if(currentId == id) {

            if(!canRead(currentElementInMap->mode, currentElementInMap->uid, currentElementInMap->gid)) {
                return -2;
            }

            offset = currentElementInMap->offset;
            found = 1;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
    }
    if(!found) {
        return -1;
    }
    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offset);
    size_t keySize = strlen(keyPlaceToAdd->data);

    uint64_t size = currentElementInMap->size;
    *keyLen = size;

    size_t allocationSize = 0;
    if(size > 4096) {
        allocationSize = size + 1;
    } else {
        allocationSize = sizeof(KeyPartitionNode) + 1;
    }

    *keyVal = (KeyPartitionNode* )kmalloc(allocationSize, GFP_KERNEL);
    if(*keyVal == NULL) {
        return -1;
    }

    memset((*keyVal)->data, 0x00, allocationSize);
    memcpy((*keyVal)->data, keyPlaceToAdd, size);
    return 0;
}

int getKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int** keyMode) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    int found = 0;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;
        if(currentId == id) {
            found = 1;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
    }

    if(!found) {
        return -1;
    }

    *keyMode = (int* )kmalloc(sizeof(currentElementInMap->mode), GFP_KERNEL);
    if(*keyMode == NULL) {
        return -1;
    }
    **keyMode = currentElementInMap->mode;

    return 0;
}
int setKeyModeByPartitionPointer(void* mappedPartition, uint64_t id, int keyMode) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    int found = 0;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;
        if(currentId == id) {
            found = 1;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
    }

    if(!found) {
        return -1;
    }

    currentElementInMap->mode = keyMode;

    return 0;
}
int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;

        if(currentId == id) {

            if(!canWrite(currentElementInMap->mode, currentElementInMap->uid, currentElementInMap->gid)) {
                return -2;
            }

            offset = currentElementInMap->offset;
            currentElementInMap->offset = 0;

            KeyPartitionNode* keyPlaceToRemove = (KeyPartitionNode* )((uint8_t *)partitionInfo + offset);
            uint64_t sizeToRemove = currentElementInMap->size;
            currentElementInMap->size = 0;
            currentElementInMap->id = 0;
            memset(keyPlaceToRemove, 0x00, sizeToRemove);
            PartitionInfo* partitionInfoToChange = (PartitionInfo* )mappedPartition;
            partitionInfoToChange->numberOfKeys -= 1;
            partitionInfoToChange->fileContentSize -= sizeToRemove;
            memcpy(&data, partitionInfoToChange, sizeof(PartitionInfo));
            // TODO change partition info key num
            return 0;
        }
        currentElementInMap = currentElementInMap + 1;
    }
    return -1;
}

int getPrvKeyById(const uint64_t id, char **prvKey, uint64_t* keyLen) {
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == -1) {
        sys_close(fd);
        return -1;
    }

    int getKeyRet = getKeyValByPartitionPointer(mappedPartition, id, (KeyPartitionNode** )prvKey, keyLen);

    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

int removePrvKeyById(uint64_t id) {
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return fd;
    }
    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == -1) {
        sys_close(fd);
        return -1;
    }

    int removeRet = removeKeyValByPartitionPointer(mappedPartition, id);
    if(removeRet != 0) {
        int ret = sys_munmap(mappedPartition, fileSize);
        sys_close(fd);
        return -1;
    }

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t fileContentSize = partitionInfo->fileContentSize;
    uint64_t numberOfKeys = partitionInfo->numberOfKeys;
    uint64_t metadataSize = sizeof(PartitionInfo) + partitionInfo->mapSize * sizeof(MapNode);

    if(numberOfKeys > 0 && fileContentSize * REDUCTION_PARAM <= fileSize) {

        uint64_t changedSize = removeFragmentation(partitionInfo);
        if(changedSize != 0) {
            sys_ftruncate(fd, changedSize);
        }

        partitionInfo->fileContentSize = changedSize;
    }

    if(numberOfKeys == 0) {
        sys_ftruncate(fd, metadataSize);
    }

    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    return 0;
}

SYSCALL_DEFINE3(write_key, const char*, key, const size_t, keyLen, uint64_t**, id) {

    size_t allocationSize = sizeof(KeyNode);
    if(keyLen > 4096) {
        allocationSize += keyLen - 4096;
    }

    KeyNode* keyNode = (KeyNode* )kmalloc(allocationSize, GFP_KERNEL);
    if(keyNode == NULL) {
        return -1;
    }

    memcpy(keyNode->keyContent, key, keyLen);
    keyNode->keySize = keyLen;

    int ret = addKeyNodeToPartition(keyNode, id);
    if(ret == -1 || ret == -2) {
        return ret;
    }

    return 0;
}

SYSCALL_DEFINE3(read_key, const uint64_t*, id, char**, key, uint64_t*, keyLen) {

    if(id == NULL || *id == 0) {
       return -1;
    }

    char* prvKey = NULL;
    int ret = getPrvKeyById(*id, &prvKey, keyLen);
    if(prvKey == NULL || ret == -1) {
        return -1;
    }

    *key = (char *)kmalloc(*keyLen, GFP_KERNEL);
    if(*key == NULL) {
        return -1;
    }

    memcpy(*key, prvKey, *keyLen);
    memset(*key + *keyLen, 0x00, 1);
    kfree(prvKey);
    return 0;
}

SYSCALL_DEFINE2(remove_key, const uint64_t*, id, const char*, filepath) {
    return id == NULL || *id == 0 ? -1 : removePrvKeyById(*id);
}

SYSCALL_DEFINE2(get_mode, const uint64_t*, id, int**, output) {

    if(id == NULL || *id == 0) {
        return -1;
    }

    char* prvKey = NULL;
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == -1) {
        sys_close(fd);
        return -1;
    }

    int getKeyRet = getKeyModeByPartitionPointer(mappedPartition, *id, output);

    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

SYSCALL_DEFINE2(set_mode, const uint64_t*, id, int*, newMode) {

    if(SU_SECURITY == 1 && geteuid().val != 0) {
        return -1;
    }

    if(id == NULL || *id == 0) {
        return -1;
    }

    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = sys_open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = sys_mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == -1) {
        sys_close(fd);
        return -1;
    }

    int getKeyRet = setKeyModeByPartitionPointer(mappedPartition, *id, *newMode);

    int ret = sys_munmap(mappedPartition, fileSize);
    sys_close(fd);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

int canRead(int mode, kuid_t uid, kgid_t gid) {
    int userRead = (mode / 100) & READ_MASK;
    int groupRead = ((mode / 10) % 10) & READ_MASK;
    int othersRead = (mode % 10) & READ_MASK;

    return (userRead && (uid.val == getuid().val)) || (groupRead && (gid.val == getgid().val)) || (othersRead && (uid.val != getuid().val));
}

int canWrite(int mode, kuid_t uid, kgid_t gid) {
    int userWrite = (mode / 100) & WRITE_MASK;
    int groupWrite = ((mode / 10) % 10) & WRITE_MASK;
    int othersWrite = (mode % 10) & WRITE_MASK;

    return (userWrite && (uid.val == getuid().val)) || (groupWrite && (gid.val == getgid().val)) || (othersWrite && ((uid.val != getuid().val)));
}

