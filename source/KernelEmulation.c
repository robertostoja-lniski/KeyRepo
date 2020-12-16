
#define EMULATION 0

#if EMULATION == 1
    #include "../include/KernelEmulation.h"
#else
    #include "KeyRepoHeaders.h"
#endif

// partition file IO
void* get_buffered_file(char* filepath, size_t* filesize, size_t extra_size);
size_t set_buffered_file(char* partition, char** buf, size_t bufsize);

size_t set_buffered_file(char* partition, char** buf, size_t bufsize) {
#if EMULATION == 1
    FILE* fd = fopen(partition, "w");
    if(fd < 0) {
        free(buf);
        return -1;
    }

    size_t ret = fwrite(*buf , sizeof(char) , bufsize, fd);

    fclose(fd);
    free(*buf);

    return ret;
#else
    printk("Entering: Set buffered file\n");
    struct file *fp;
    mm_segment_t fs;
    loff_t pos = 0;

    printk("File open\n");
    fp = filp_open(partition, O_RDWR, 0644);
    if (IS_ERR(fp)) {
            printk("Cannot open file\n");
            return -1;
    }

    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("Next action: kernel read\n");
    size_t ret = kernel_read(fp, buf, bufsize, &pos);

    printk("Next action: file close\n");
    filp_close(fp, NULL);
    printk("Next action: set fs\n");
    set_fs(fs);
    printk("Next action: kfree on *buf\n");
    kfree(*buf);
    printk("Exiting: Set buffered file\n");
    return ret;
#endif
}

void *get_buffered_file(char* filepath, size_t* size, size_t extra_size) {
#if EMULATION == 1
    FILE* fp = fopen(filepath, "r");
    if(fp < 0) {
        return NULL;
    }

    uint8_t* buf = NULL;

    if(fp == NULL) {
        return NULL;
    }

    char* source;
    long bufsize;

    if (fseek(fp, 0L, SEEK_END) == 0) {
        bufsize = ftell(fp);
        if (bufsize == -1) {
            fclose(fp);
            return NULL;
        }

        bufsize += extra_size;

        source = (char* )(malloc(sizeof(char) * bufsize));
        if(source == NULL) {
            fclose(fp);
            return NULL;
        }

        if (fseek(fp, 0L, SEEK_SET) != 0) {
            fclose(fp);
            return NULL;
        }

        size_t newLen = fread(source, sizeof(char), bufsize, fp);
        if (ferror(fp) != 0) {
            fclose(fp);
            return NULL;
        }
    }

    *size = bufsize;
    fclose(fp);
    return (void* )source;
#else
    printk("Entering get buffer file\n");

    struct kstat *stat;
    struct file *fp;
    mm_segment_t fs;
    loff_t pos = 0;
    char *buf;

    printk("Opening partion\n");
    fp = filp_open(partition, O_RDWR, 0644);
    if (IS_ERR(fp)) {
            printk("Open file error!\n");
            return NULL;
    }

    printk("Next action: getting fs\n");
    fs = get_fs();
    printk("Next action: setting fs\n");
    set_fs(KERNEL_DS);

    printk("Next action: stat kmalloc\n");
    stat =(struct kstat *) kmalloc(sizeof(struct kstat), GFP_KERNEL);
    if (!stat) {
        printk("Kmalloc failed\n");
        return ERR_PTR(-ENOMEM);
    }

    printk("Next action: vfs_stat\n");
    vfs_stat(partition, stat);
    *size = stat->size;

    printk("Next action: buffer\n");
    buf = kmalloc(*size, GFP_KERNEL);
    if (!buf) {
        kfree(stat);
        printk("malloc input buf error!\n");
        NULL;
    }

    printk("Next action: kernel read\n");
    kernel_read(fp, buf, *size, &pos);

    printk("Next action: filp_close\n");
    filp_close(fp, NULL);
    printk("Next action: set_fs\n");
    set_fs(fs);
    printk("Next action: kfree\n");
    kfree(stat);
    printk("Entering get buffer file\n");
    return buf;
#endif
}

// private
uint64_t generateRandomId(void* mappedPartition){

    printk("Entering generate random id\n");

    int generateTrials = 10;
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    int foundSameId = 0;
    uint64_t newId;
    int n;
    while(generateTrials--) {

        newId = 0;
	int i;
        for (i=0; i<64; i++) {
            printk("Next action: get_random_bytes\n");
            get_random_bytes(&n, sizeof(n));
            newId = newId*2 + n%2;
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

        printk("NEXT TRIAL!\n");
    }

    return newId;
}
int initFileIfNotDefined() {
#if EMULATION == 1
    FILE *file;
    if ((file = fopen(partition, "r")))
    {
        fclose(file);
        return 1;
    }

    PartitionInfo* partitionInfo = (PartitionInfo* )malloc(sizeof(PartitionInfo));
    if(!partitionInfo) {
        return 1;
    }

#else

    printk("Entering init file if not defined\n");
    struct file *fp;
    fp = filp_open(partition, O_RDWR, 0644);
    if (IS_ERR(fp)) {
            printk("Open file error!\n");
            return 1;
    }
    printk("Open file success\n");
    filp_close(fp, NULL);
    printk("File closed\n");

    printk("Partition info to be allocated\n");
    PartitionInfo* partitionInfo = (PartitionInfo* )kmalloc(sizeof(PartitionInfo), GFP_KERNEL);
    if(!partitionInfo) {
        printk("Allocation failed, exiting\n");
        return 1;
    }

#endif

    partitionInfo->mapSize = DEFAULT_MAP_SIZE;
    partitionInfo->numberOfKeys = DEFAULT_NUMBER_OF_KEYS;
    partitionInfo->fileContentSize = DEFAULT_MAP_SIZE * sizeof(MapNode) + sizeof(PartitionInfo);

    uint64_t fileSize = partitionInfo->fileContentSize;

#if EMULATION == 1
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));
#else
    printk("Memory to be copied to data field\n");
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));
    printk("Copy to data successful\n");
#endif

#if EMULATION == 1
    void* partitionStart = malloc(fileSize);
#else
    printk("New partition file to be allocated\n");
    void* partitionStart = kmalloc(fileSize, GFP_KERNEL);
    printk("After allocation\n");
#endif

    if(!partitionStart) {
        printk("Allocation failed\n");
        return 1;
    }
    memset(partitionStart, 0x00, fileSize);

    printk("Memset ok\n");

    memcpy(partitionStart, partitionInfo, sizeof(PartitionInfo));

    printk("Memcpy ok\n");

    MapNode* mapPosition = (MapNode* )((PartitionInfo* )partitionStart + 1);
    int i;
    for(i = 0; i < partitionInfo->mapSize; i++) {

#if EMULATION == 1
        MapNode* mapData = (MapNode* )malloc(sizeof(MapNode));
#else
        printk("Malloc to be used\n");
        MapNode* mapData = (MapNode* )kmalloc(sizeof(MapNode), GFP_KERNEL);
        printk("Malloc ok\n");
#endif

        if(!mapData) {
            return 1;
        }
        mapData -> id = 0;
        mapData -> offset = 0;
        memcpy(mapPosition, mapData, sizeof(*mapData));
        printk("Memcpy ok\n");
        mapPosition = mapPosition + 1;
#if EMULATION == 1
        free(mapData);
#else
        kfree(mapData);
        printk("Free ok\n");
#endif
    }

    printPartition(partitionStart);
#if EMULATION == 1
    free(partitionInfo);
#else
    kfree(partitionInfo);
    printk("Free ok\n");
#endif

    size_t ret;
    printk("Set buffered file\n");
    ret = set_buffered_file(partition, (char** )&partitionStart, fileSize);
    printk("After set\n");
    if(ret != fileSize) {
        printk("Set failed\n");
        return 1;
    }

#if EMULATION == 1
    printk("Exiting init file if not defined\n");
#endif

    return 0;
}
uint64_t removeFragmentation(PartitionInfo* partitionInfo) {

    // create new map

#if EMULATION == 1 
    MapNode* tmpMap = (MapNode* )malloc(sizeof(MapNode) * partitionInfo->numberOfKeys);
#else
    MapNode* tmpMap = (MapNode* )kmalloc(sizeof(MapNode) * partitionInfo->numberOfKeys, GFP_KERNEL);
#endif

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

#if EMULATION == 1
        int uid = currentElementInMap->uid;
        int gid = currentElementInMap->gid;
#else
        kuid_t uid = currentElementInMap->uid;
        kgid_t gid = currentElementInMap->gid;
#endif

        if(offset != 0) {

#if EMULATION == 1
            uint8_t* keyContent = (uint8_t* )malloc(size);
            if(!keyContent) {
                free(tmpMap);
#else
            uint8_t* keyContent = (uint8_t* )kmalloc(size, GFP_KERNEL);
            if(!keyContent) {
                kfree(tmpMap);
#endif
                return 0;
            }

            memcpy(keyContent, ((uint8_t* )partitionInfo + offset), size);
            memcpy(((uint8_t* )partitionInfo + changedOffset), keyContent, size);

#if EMULATION == 1
            free(keyContent);
#else
            kfree(keyContent);
#endif

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
    memset(currentElementInMap + partitionInfo->numberOfKeys, 0x00, sizeof(MapNode) * (MAX_KEY_NUM - partitionInfo->numberOfKeys));

#if EMULATION == 1
    free(tmpMap);
#else
    kfree(tmpMap);
#endif

    return changedOffset;
}
void printPartition(const void* mappedPartition) {
    return;
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t keys = partitionInfo->numberOfKeys;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;
    uint64_t mapSize = partitionInfo->mapSize;

    printf("This partition has: %llu keys.\n", keys);
    printf("First free slot is: %llu.\n", offsetToAdd + sizeof(PartitionInfo));

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t id;
    int i;
    for(i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        if(offset != 0) {
            id = currentElementInMap->id;
//            std::cout << "For " << i << " node in map there was found id: " << id << " and offset " << offset
//                << " and size " << currentElementInMap->size <<  std::endl;
        }
        currentElementInMap = currentElementInMap + 1;
    }

}
int addKeyNodeToPartition(KeyNode* keyNodeToAdd, uint64_t** id) {

    printk("Entering add key node to partition\n");
    initFileIfNotDefined();
    if(data.numberOfKeys == MAX_KEY_NUM) {
        return -2;
    }

    size_t fileSizeAdd;
    printk("Loading file to buffer\n");
    void* mappedPartition = get_buffered_file(partition, &fileSizeAdd, keyNodeToAdd->keySize);
    printk("File loaded to buffer\n");
    if(!mappedPartition) {
        return -1;
    }

    printk("Adding key to partition\n");
    int addRet = addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd, id);
    printk("Key to partition added\n");

    printk("Saving buffer\n");
    if(set_buffered_file(partition, (char** )&mappedPartition, fileSizeAdd) != fileSizeAdd) {
        printk("Error in saving to buffer\n");
        return -1;
    }

    printk("Exiting add key node to partition\n");
    return addRet;
}
int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t** id) {

    printk("Entering add key node to partition\n");
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t nextId;
    uint64_t prevOffset = currentElementInMap->offset;
    int i;

    printk("Moving to first map node succeeded\n");
    for(i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;

        if(offset == 0) {
            printk("Offset is zero\n");
            nextId = generateRandomId(mappedPartition);
            printk("Next id is %llu\n", nextId);

#if EMULATION == 1
            *id = (uint64_t* )malloc(sizeof(*id));
#else
            *id = (uint64_t* )kmalloc(sizeof(*id), GFP_KERNEL);
#endif

            if(!id) {
                return -2;
            }
            **id = nextId;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
        prevOffset = offset;
    }

#if EMULATION == 1
    MapNode* elementDataToUpdate = (MapNode* )malloc(sizeof(MapNode));
#else
    MapNode* elementDataToUpdate = (MapNode* )kmalloc(sizeof(MapNode), GFP_KERNEL);
#endif

    if(!elementDataToUpdate) {
        return -1;
    }

    elementDataToUpdate->offset = offsetToAdd;
    elementDataToUpdate->id = nextId;
    elementDataToUpdate->size = keyNodeToAdd->keySize;
    printk("Next action: get uid\n");
    elementDataToUpdate->uid = getuid();
    printk("Next action: get gid\n");
    elementDataToUpdate->gid = getgid();
    elementDataToUpdate->mode = 777;

    memcpy(currentElementInMap, elementDataToUpdate, sizeof(MapNode));

#if EMULATION == 1
    free(elementDataToUpdate);
#else 
    kfree(elementDataToUpdate);
#endif


    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offsetToAdd);
    memcpy(keyPlaceToAdd, keyNodeToAdd->keyContent, keyNodeToAdd->keySize);

#if EMULATION == 1
    PartitionInfo* partitionInfoToUpdate = (PartitionInfo* )malloc(sizeof(PartitionInfo));
#else
    PartitionInfo* partitionInfoToUpdate = (PartitionInfo* )kmalloc(sizeof(PartitionInfo), GFP_KERNEL);
#endif

    partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
    partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
    partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + keyNodeToAdd->keySize;
    memcpy(partitionInfo, partitionInfoToUpdate, sizeof(PartitionInfo));
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

#if EMULATION == 1
    free(partitionInfoToUpdate);
    free(keyNodeToAdd);
#else
    kfree(partitionInfoToUpdate);
    kfree(keyNodeToAdd);
#endif

    printk("Exiting add key Node\n");
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
//        std::cout << "offset is " << offset << std::endl;
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

#if EMULATION == 1
    *keyVal = (KeyPartitionNode* )malloc(allocationSize);
#else
    *keyVal = (KeyPartitionNode* )kmalloc(allocationSize, GFP_KERNEL);
#endif

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

#if EMULATION == 1
    *keyMode = (int* )malloc(sizeof(currentElementInMap->mode));
#else
    *keyMode = (int* )kmalloc(sizeof(currentElementInMap->mode), GFP_KERNEL);
#endif

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
//        std::cout << "offset is " << offset << std::endl;
    return -1;
}
int getPrvKeyById(const uint64_t id, char **prvKey, uint64_t* keyLen) {

    size_t fileSize;
    void* mappedPartition = get_buffered_file(partition, &fileSize, 0);
    if(!mappedPartition) {
        return -1;
    }

    int getKeyRet = getKeyValByPartitionPointer(mappedPartition, id, (KeyPartitionNode** )prvKey, keyLen);

#if EMULATION == 1
    free(mappedPartition);
#else
    kfree(mappedPartition);
#endif

    if(getKeyRet == -1) {
        return -1;
    }

    return 0;
}
int removePrvKeyById(uint64_t id) {

    size_t fileSize;
    void* mappedPartition = get_buffered_file(partition, &fileSize, 0);
    if(mappedPartition == NULL) {
        return -1;
    }

    int removeRet = removeKeyValByPartitionPointer(mappedPartition, id);
    if(removeRet != 0) {
        
#if EMULATION == 1
    free(mappedPartition);
#else
    kfree(mappedPartition);
#endif

        return -1;
    }

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t fileContentSize = partitionInfo->fileContentSize;
    uint64_t numberOfKeys = partitionInfo->numberOfKeys;
    uint64_t metadataSize = sizeof(PartitionInfo) + partitionInfo->mapSize * sizeof(MapNode);

    if(numberOfKeys > 0 && fileContentSize * REDUCTION_PARAM <= fileSize) {

        uint64_t changedSize = removeFragmentation(partitionInfo);

#if EMULATION == 1
        if(changedSize != 0) {
            int fdDef = open(partition, O_RDWR, 0);
            if(fdDef < 0) {
                return -1;
            }
            ftruncate(fdDef, changedSize);
            close(fdDef);
        }
#endif

        partitionInfo->fileContentSize = changedSize;
        fileSize = changedSize;
    }

    if(numberOfKeys == 0) {
#if EMULATION == 1

        int fdDef = open(partition, O_RDWR, 0);
        if(fdDef < 0) {
            return -1;
        }
        ftruncate(fdDef, metadataSize);
        close(fdDef);
#endif
        fileSize = metadataSize;
    }

    if(set_buffered_file(partition, (char** )&mappedPartition, fileSize) != fileSize) {
        return -1;
    }

    return removeRet;
}

// public
#if EMULATION == 1
int getCurrentKeyNumFromEmulation() {
#else
SYSCALL_DEFINE0(get_current_key_num) {
#endif

    return data.numberOfKeys;
}

#if EMULATION == 1
int writeKey(const char* key, const size_t keyLen, uint64_t** id) {
#else
SYSCALL_DEFINE3(write_key, const char*, key, const size_t, keyLen, uint64_t**, id) {
    printk("Entering write key\n");
#endif

    size_t allocationSize = sizeof(KeyNode);
    printk("AllocationSize (size of key node is) %zu\n", allocationSize);
    if(keyLen > 4096) {
        allocationSize += keyLen - 4096;
        printk("AllocationSize extended to %zu\n", allocationSize);
    }

    KeyNode* keyNode;

#if EMULATION == 1
    keyNode = (KeyNode* )malloc(allocationSize);
#else
    printk("Kernel space memory to be allocated\n");
    keyNode = (KeyNode* )kmalloc(allocationSize, GFP_KERNEL);
    printk("Kernel space memory maybe allocated\n");
#endif

    if(keyNode == NULL) {
        printk("Kernel space memory allocation failed\n");
        return -1;
    }

    printk("Kernel space memory successfully allocated, %zu of bytes to be copied to keyNode\n", keyLen);
    memcpy(keyNode->keyContent, key, keyLen);
    printk("Memcpy successful");
    keyNode->keySize = keyLen;

    int ret = addKeyNodeToPartition(keyNode, id);
    if(ret == -1 || ret == -2) {
        return ret;
    }

#if EMULATION == 0
    printk("Exiting write key\n");
#endif

    return 0;
}

#if EMULATION == 1
int readKey(const uint64_t* id, char** key, uint64_t* keyLen) {
#else
SYSCALL_DEFINE3(read_key, const uint64_t*, id, char**, key, uint64_t*, keyLen) {
#endif

    if(id == NULL || *id == 0) {
       return -1;
    }

    char* prvKey = NULL;
    int ret = getPrvKeyById(*id, &prvKey, keyLen);
    if(prvKey == NULL || ret == -1) {
        return -1;
    }

#if EMULATION == 1
    *key = (char *)malloc(*keyLen);
#else
    *key = (char *)kmalloc(*keyLen, GFP_KERNEL);
#endif

    if(*key == NULL) {
        return -1;
    }

    memcpy(*key, prvKey, *keyLen);
    memset(*key + *keyLen, 0x00, 1);

#if EMULATION == 1
    free(prvKey);
#else 
    kfree(prvKey);
#endif

    return 0;
}

#if EMULATION == 1
int removeKey(const uint64_t* id, const char* filepath) {
#else
SYSCALL_DEFINE2(remove_key, const uint64_t*, id, const char*, filepath) {
#endif

    return id == NULL || *id == 0 ? -1 : removePrvKeyById(*id);
}


// for kernel kuid_t instead of uid
#if EMULATION == 1
int getMode(const uint64_t* id, int** output) {
#else
SYSCALL_DEFINE2(get_mode, const uint64_t*, id, int**, output) {
#endif

    if(id == NULL || *id == 0) {
        return -1;
    }

    char* prvKey = NULL;

    size_t fileSize;
    void* mappedPartition = get_buffered_file(partition, &fileSize, 0);
    if(mappedPartition == NULL) {
        return -1;
    }

    int getKeyRet = getKeyModeByPartitionPointer(mappedPartition, *id, output);

#if EMULATION == 1
    free(mappedPartition);
#else
    kfree(mappedPartition);
#endif

    if(getKeyRet == -1) {
        return -1;
    }
    return 0;
}

#if EMULATION == 1
int setMode(const uint64_t* id, int* newMode) {
#else
SYSCALL_DEFINE2(set_mode, const uint64_t*, id, int*, newMode) {
#endif

    if(SU_SECURITY == 1 && geteuid().val != 0) {
        return -1;
    }

    if(id == NULL || *id == 0) {
        return -1;
    }

    size_t fileSize;
    void* mappedPartition = get_buffered_file(partition, &fileSize, 0);
    if(mappedPartition == NULL) {
        return -1;
    }

    int getKeyRet = setKeyModeByPartitionPointer(mappedPartition, *id, *newMode);
    if(getKeyRet == -1) {

#if EMULATION == 1
    free(mappedPartition);
#else
    kfree(mappedPartition);
#endif
        return -1;
    }

    if(set_buffered_file(partition, (char** )&mappedPartition, fileSize) != fileSize) {
        return -1;
    }

    return 0;
}

#if EMULATION == 1
int canRead(int mode, int uid, int gid) {
    int userRead = (mode / 100) & READ_MASK;
    int groupRead = ((mode / 10) % 10) & READ_MASK;
    int othersRead = (mode % 10) & READ_MASK;

    return userRead && uid == getuid() || groupRead && gid == getgid() || othersRead && uid != getuid();
}
int canWrite(int mode, int uid, int gid) {
    int userWrite = (mode / 100) & WRITE_MASK;
    int groupWrite = ((mode / 10) % 10) & WRITE_MASK;
    int othersWrite = (mode % 10) & WRITE_MASK;

    return userWrite && uid == getuid() || groupWrite && gid == getgid() || othersWrite && uid != getuid();
}
#else

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

#endif

