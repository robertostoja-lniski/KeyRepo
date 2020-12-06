//
// Created by robert on 25.08.2020.
//

#include "../include/KernelEmulation.h"

size_t getFileSize(const char* filename);

uint64_t generateRandomId(void* mappedPartition){

    int generateTrials = 10;
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    int foundSameId = 0;
    uint64_t newId;
    while(generateTrials--) {

        newId = 0;
        for (int i=0; i<64; i++) {
            newId = newId*2 + rand()%2;
        }

        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t id;
        for(int i = 0; i < mapSize; i++) {
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
    // assertion implies that there is an error with random function
    assert(generateTrials == 0);
}

int initFileIfNotDefined() {
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

    partitionInfo->mapSize = DEFAULT_MAP_SIZE;
    partitionInfo->numberOfKeys = DEFAULT_NUMBER_OF_KEYS;
    partitionInfo->fileContentSize = DEFAULT_MAP_SIZE * sizeof(MapNode) + sizeof(PartitionInfo);

    uint64_t fileSize = partitionInfo->fileContentSize;
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

    FILE* fp = fopen(partition, "w+");
    fputs("EMPTY PARTITION\n", fp);
    fclose(fp);

    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }
    ftruncate(fd, fileSize);
    close(fd);

    fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }

    if(fileSize != getFileSize(partition)) {
        return 1;
    }

    void* mappedPartition = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
    memset(mappedPartition, 0x00, fileSize);

    void* partitionStart = mappedPartition;
    if(mappedPartition == MAP_FAILED) {
        munmap(mappedPartition, fileSize);
        close(fd);
        return 1;
    }

    memcpy(mappedPartition, partitionInfo, sizeof(PartitionInfo));
    MapNode* mapPosition = (MapNode* )((PartitionInfo* )mappedPartition + 1);
    for(int i = 0; i < partitionInfo->mapSize; i++) {
        MapNode* mapData = (MapNode* )malloc(sizeof(MapNode));
        if(!mapData) {
            munmap(mappedPartition, fileSize);
            close(fd);
            return 1;
        }
        mapData -> id = 0;
        mapData -> offset = 0;
        memcpy(mapPosition, mapData, sizeof(*mapData));
        mapPosition = mapPosition + 1;
        free(mapData);
    }

    printPartition(partitionStart);
    free(partitionInfo);
    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);
}


size_t getFileSize(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

int getCurrentKeyNumFromEmulation() {
    return data.numberOfKeys;
}

uint64_t removeFragmentation(PartitionInfo* partitionInfo) {

    // create new map
    MapNode* tmpMap = (MapNode* )malloc(sizeof(MapNode) * partitionInfo->numberOfKeys);
    if(!tmpMap) {
        return 0;
    }

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    int tmpMapIndex = 0;
    uint64_t changedOffset = DEFAULT_MAP_SIZE * sizeof(MapNode) + sizeof(PartitionInfo);

    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        uint64_t id = currentElementInMap->id;
        uint64_t size = currentElementInMap->size;
        int mode = currentElementInMap->mode;
        int uid = currentElementInMap->uid;
        int gid = currentElementInMap->gid;


        if(offset != 0) {

            uint8_t* keyContent = (uint8_t* )malloc(size);
            if(!keyContent) {
                free(tmpMap);
                return 0;
            }

            memcpy(keyContent, ((uint8_t* )partitionInfo + offset), size);
            memcpy(((uint8_t* )partitionInfo + changedOffset), keyContent, size);

            free(keyContent);

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

    free(tmpMap);

    return changedOffset;
}

int addKeyNodeToPartition(KeyNode* keyNodeToAdd, uint64_t** id) {

    initFileIfNotDefined();
    if(data.numberOfKeys == MAX_KEY_NUM) {
        return -2;
    }

    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }

    size_t fileSize = getFileSize(partition);
    void* mappedPartition = mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    void* partitionStart = mappedPartition;

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t usedFileSize = partitionInfo->fileContentSize;

    int isSizeExtNeeded = fileSize < usedFileSize + keyNodeToAdd->keySize;

    int retSizeCheck = munmap(mappedPartition, fileSize);
    close(fd);
    assert(retSizeCheck == 0);

    if(isSizeExtNeeded) {

        int fd = open(partition, O_RDWR, 0);
        if(fd < 0) {
            return -1;
        }

        fileSize = getFileSize(partition) + keyNodeToAdd->keySize;
        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition, O_RDWR, 0);
        if(fd < 0) {
            return -1;
        }
    }

    if(fileSize != getFileSize(partition)) {
        return -1;
    }

    mappedPartition = mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    partitionStart = mappedPartition;

    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return -1;
    }
    printPartition(partitionStart);
    int addRet = addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd, id);

    printPartition(partitionStart);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    return addRet;
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
    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        if(offset != 0) {
            id = currentElementInMap->id;
//            std::cout << "For " << i << " node in map there was found id: " << id << " and offset " << offset
//                << " and size " << currentElementInMap->size <<  std::endl;
        }
        currentElementInMap = currentElementInMap + 1;
    }

}

int addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode* keyNodeToAdd, uint64_t** id) {
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t nextId;
    uint64_t prevOffset = currentElementInMap->offset;
    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;

        if(offset == 0) {
            nextId = generateRandomId(mappedPartition);
            *id = (uint64_t* )malloc(sizeof(*id));
            if(!id) {
                return -2;
            }
            **id = nextId;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
        prevOffset = offset;
    }

    MapNode* elementDataToUpdate = (MapNode* )malloc(sizeof(MapNode));
    if(!elementDataToUpdate) {
        return -1;
    }

    elementDataToUpdate->offset = offsetToAdd;
    elementDataToUpdate->id = nextId;
    elementDataToUpdate->size = keyNodeToAdd->keySize;
    elementDataToUpdate->uid = getuid();
    elementDataToUpdate->gid = getgid();
    elementDataToUpdate->mode = getDefaultMode();

    memcpy(currentElementInMap, elementDataToUpdate, sizeof(MapNode));
    free(elementDataToUpdate);


    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offsetToAdd);
    memcpy(keyPlaceToAdd, keyNodeToAdd->keyContent, keyNodeToAdd->keySize);

    PartitionInfo* partitionInfoToUpdate = (PartitionInfo* )malloc(sizeof(PartitionInfo));
    partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
    partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
    partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + keyNodeToAdd->keySize;
    memcpy(partitionInfo, partitionInfoToUpdate, sizeof(PartitionInfo));
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

    free(partitionInfoToUpdate);
    free(keyNodeToAdd);

    return 0;
}

int getKeyValByPartitionPointer(void* mappedPartition, uint64_t id, KeyPartitionNode** keyVal, uint64_t* keyLen) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    int found = 0;
    for(int i = 0; i < mapSize; i++) {
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

    *keyVal = (KeyPartitionNode* )malloc(allocationSize);
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
    for(int i = 0; i < mapSize; i++) {
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

    *keyMode = (int* )malloc(sizeof(currentElementInMap->mode));
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
    for(int i = 0; i < mapSize; i++) {
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
    for(int i = 0; i < mapSize; i++) {
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
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return -1;
    }

    int getKeyRet = getKeyValByPartitionPointer(mappedPartition, id, (KeyPartitionNode** )prvKey, keyLen);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

int removePrvKeyById(uint64_t id) {
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return fd;
    }
    void* mappedPartition = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return -1;
    }

    int removeRet = removeKeyValByPartitionPointer(mappedPartition, id);
    if(removeRet != 0) {
        int ret = munmap(mappedPartition, fileSize);
        close(fd);
        assert(ret == 0);
        return -1;
    }

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t fileContentSize = partitionInfo->fileContentSize;
    uint64_t numberOfKeys = partitionInfo->numberOfKeys;
    uint64_t metadataSize = sizeof(PartitionInfo) + partitionInfo->mapSize * sizeof(MapNode);

    if(numberOfKeys > 0 && fileContentSize * REDUCTION_PARAM <= fileSize) {

        uint64_t changedSize = removeFragmentation(partitionInfo);
        if(changedSize != 0) {
            ftruncate(fd, changedSize);
        }

        partitionInfo->fileContentSize = changedSize;
    }

    if(numberOfKeys == 0) {
        ftruncate(fd, metadataSize);
    }

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    return removeRet;
}

int writeKey(const char* key, const size_t keyLen, uint64_t** id) {

    size_t allocationSize = sizeof(KeyNode);
    if(keyLen > 4096) {
        allocationSize += keyLen - 4096;
    }

    KeyNode* keyNode = (KeyNode* )malloc(allocationSize);
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

int readKey(const uint64_t* id, char** key, uint64_t* keyLen) {

    if(id == NULL || *id == 0) {
       return -1;
    }

    char* prvKey = NULL;
    int ret = getPrvKeyById(*id, &prvKey, keyLen);
    if(prvKey == NULL || ret == -1) {
        return -1;
    }

    *key = (char *)malloc(*keyLen);
    if(*key == NULL) {
        return -1;
    }

    memcpy(*key, prvKey, *keyLen);
    memset(*key + *keyLen, 0x00, 1);
    free(prvKey);
    return 0;
}

int get(const uint64_t* id, char** output) {
    if(id == NULL || *id == 0) {
        return -1;
    }

    char* prvKey = NULL;
    uint64_t keyLen;
    int getKeyRet = getPrvKeyById(*id, &prvKey, &keyLen);
    if(getKeyRet != 0) {
        return -1;
    }

    *output = prvKey;
    return 0;
}

int removeKey(const uint64_t* id, const char* filepath) {
    return id == NULL || *id == 0 ? -1 : removePrvKeyById(*id);
}

int getMode(const uint64_t* id, int** output) {

    if(id == NULL || *id == 0) {
        return -1;
    }

    char* prvKey = NULL;
    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return -1;
    }

    int getKeyRet = getKeyModeByPartitionPointer(mappedPartition, *id, output);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

int setMode(const uint64_t* id, int* newMode) {

    if(SU_SECURITY == 1 && geteuid() != 0) {
        return -1;
    }

    if(id == NULL || *id == 0) {
        return -1;
    }

    size_t fileSize = getFileSize(partition);
    //Open file
    int fd = open(partition, O_RDWR, 0);
    if(fd < 0) {
        return -1;
    }
    void* mappedPartition = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return -1;
    }

    int getKeyRet = setKeyModeByPartitionPointer(mappedPartition, *id, *newMode);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    if(getKeyRet == -1) {
        return -1;
    }
    // tmp
    return 0;
}

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

