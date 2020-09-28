//
// Created by robert on 25.08.2020.
//

#include "../include/KernelEmulation.h"

uint64_t generateRandomId(void* mappedPartition){
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<uint64_t> uDist(0, UINT64_MAX);
    int generateTrials = 10;

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    int foundSameId = false;
    uint64_t newId;
    while(generateTrials--) {

        newId = uDist(gen);
        MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
        uint64_t id;
        for(int i = 0; i < mapSize; i++) {
            uint64_t offset = currentElementInMap->offset;
            id = currentElementInMap->id;

            if(id == newId) {
                foundSameId = true;
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
    if(std::experimental::filesystem::exists(partition)) {
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

    FILE* fp = fopen(partition.c_str(), "w+");
    fputs("EMPTY PARTITION\n", fp);
    fclose(fp);

    int fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }
    ftruncate(fd, fileSize);
    close(fd);

    fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }

    if(fileSize != getFileSize(partition.c_str())) {
        return 1;
    }

    void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
    memset(mappedPartition, 0x00, fileSize);
    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "last accessible byte is: " << (uint8_t* )mappedPartition + fileSize << std::endl;
    }

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

int writeKeyToTemporaryFile(RSA* r) {
    FILE *fp = fopen(tmpKeyStorage.c_str(), "wb");
    if(fp == nullptr) {
        return 1;
    }
    int success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
    if(!success) {
        return 1;
    }
    fclose(fp);
}

KeyNode generateKeyNodeFromKeyInFile() {
    KeyNode keyNode;
    std::string line;

    FILE *fp;
    size_t buffSize = 4096;
    char str[buffSize];
    memset(str, 0x00, buffSize);

    fp = fopen(tmpKeyStorage.c_str(), "r");
    if (fp == NULL){
        return keyNode;
    }

    while (fgets(str, buffSize, fp) != NULL) {
        line += str;
        memset(str, 0x00, buffSize);
    }
    fclose(fp);

    // when serialised there will be no access to bytes as std::string

    keyNode.keyContent = line;
    keyNode.keySize = keyNode.keyContent.size();
    return keyNode;
}

size_t getFileSize(const char* filename) {
    struct stat st{};
    stat(filename, &st);
    return st.st_size;
}

int getCurrentKeyNumFromEmulation() {
    return data.numberOfKeys;
}

uint64_t removeFragmentation(PartitionInfo* partitionInfo) {

    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "BEFORE DEFRAGMENTATION" << std::endl;
        printPartition(partitionInfo);
    }

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

    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "AFTER DEFRAGMENTATION" << std::endl;
        std::cout << "Changed offset is: " << changedOffset << std::endl;
        printPartition(partitionInfo);
    }
    return changedOffset;
}

uint64_t addKeyNodeToPartition(KeyNode keyNodeToAdd) {

    initFileIfNotDefined();
    if(data.numberOfKeys == MAX_KEY_NUM) {
        return 0;
    }

    int fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }

    size_t fileSize = getFileSize(partition.c_str());
    void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    void* partitionStart = mappedPartition;

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t usedFileSize = partitionInfo->fileContentSize;

    bool isSizeExtNeeded = fileSize < usedFileSize + keyNodeToAdd.keySize;

    int retSizeCheck = munmap(mappedPartition, fileSize);
    close(fd);
    assert(retSizeCheck == 0);

    if(isSizeExtNeeded) {

        int fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }

        fileSize = getFileSize(partition.c_str()) + keyNodeToAdd.keySize;
        if(VERBOSE_LEVEL >= VERBOSE_LOW) {
            std::cout << "when adding new node ext file size is: " << fileSize << std::endl;
            std::cout << "real file size is " << getFileSize(partition.c_str()) << std::endl;
            std::cout << "key size is " << keyNodeToAdd.keySize << std::endl;
        }

        ftruncate(fd, fileSize);
        close(fd);

        fd = open(partition.c_str(), O_RDWR, 0);
        if(fd < 0) {
            return 1;
        }
    }

    if(fileSize != getFileSize(partition.c_str())) {
        return 1;
    }
    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << std::endl << "file size is " << fileSize << std::endl;
    }

    mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    partitionStart = mappedPartition;

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "last accessible byte is: " << (uint8_t* )mappedPartition + fileSize << std::endl;
    }

    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return 1;
    }
    printPartition(partitionStart);
    uint64_t id = addKeyNodeByPartitionPointer(mappedPartition, keyNodeToAdd);

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << std::endl;
    }

    printPartition(partitionStart);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    return id;
}

void printPartition(const void* mappedPartition) {
    if(VERBOSE_LEVEL == VERBOSE_NO) {
        return;
    }
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t keys = partitionInfo->numberOfKeys;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;
    uint64_t mapSize = partitionInfo->mapSize;

    std::cout << "This partition has: " << keys << " keys." << std::endl;
    std::cout << "First free slot is: " << offsetToAdd + sizeof(PartitionInfo) << std::endl;
    std::cout << "Map has: " << mapSize << " number of nodes" << std::endl;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t id;
    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        if(offset != 0) {
            id = currentElementInMap->id;
            std::cout << "For " << i << " node in map there was found id: " << id << " and offset " << offset
                << " and size " << currentElementInMap->size <<  std::endl;
        }
        currentElementInMap = currentElementInMap + 1;
    }

}

uint64_t addKeyNodeByPartitionPointer(void* mappedPartition, KeyNode keyNodeToAdd) {
    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t offsetToAdd = partitionInfo->fileContentSize;

    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "offset to add is: " << offsetToAdd << std::endl;
        std::cout << "The difference should be: " << (uint8_t *)(partitionInfo + 1) - (uint8_t *)(partitionInfo) << std::endl;
        std::cout << "I think it should be: " << sizeof(PartitionInfo) + sizeof(MapNode) * 128 << std::endl;
    }

    uint64_t mapSize = partitionInfo->mapSize;
    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t id;
    uint64_t prevOffset = currentElementInMap->offset;
    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
            std::cout << i << " offset: " << offset << std::endl;
            std::cout << "offset diff is: " << offset - prevOffset << std::endl;
        }

        if(offset == 0) {
            id = generateRandomId(mappedPartition);
            break;
        }
        currentElementInMap = currentElementInMap + 1;
        prevOffset = offset;
    }

    MapNode* elementDataToUpdate = (MapNode* )malloc(sizeof(MapNode));
    if(!elementDataToUpdate) {
        return 1;
    }

    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "New element is: " << offsetToAdd << " " << id << " " << keyNodeToAdd.keySize << std::endl;
    }

    elementDataToUpdate->offset = offsetToAdd;
    elementDataToUpdate->id = id;
    elementDataToUpdate->size = keyNodeToAdd.keySize;

    memcpy(currentElementInMap, elementDataToUpdate, sizeof(MapNode));
    free(elementDataToUpdate);


    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offsetToAdd);

    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "map start " << partitionInfo + 1 << std::endl;
        std::cout << "Key place to add (right after map end) is: " << keyPlaceToAdd << std::endl;
        std::cout << "Key to add size: " << keyNodeToAdd.keySize << std::endl;
        std::cout << "Key to add content len: " << keyNodeToAdd.keyContent.size() << std::endl;
        std::cout << "Key place to add data before cpy: " << keyPlaceToAdd->data <<std::endl;
    }

    memcpy(keyPlaceToAdd, keyNodeToAdd.keyContent.c_str(), keyNodeToAdd.keySize);


    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "Key place to add data after cpy: " << keyPlaceToAdd->data <<std::endl;
    }

    PartitionInfo* partitionInfoToUpdate = (PartitionInfo* )malloc(sizeof(PartitionInfo));
    partitionInfoToUpdate->mapSize = partitionInfo->mapSize;
    partitionInfoToUpdate->numberOfKeys = partitionInfo->numberOfKeys + 1;
    partitionInfoToUpdate->fileContentSize = partitionInfo->fileContentSize + keyNodeToAdd.keySize;
    memcpy(partitionInfo, partitionInfoToUpdate, sizeof(PartitionInfo));
    memcpy(&data, partitionInfo, sizeof(PartitionInfo));

    free(partitionInfoToUpdate);
    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "id of added key is: " << id << std::endl;
    }

    return id;
}

std::string getKeyValByPartitionPointer(void* mappedPartition, uint64_t id) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    bool found = false;
    for(int i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;
        if(currentId == id) {
            offset = currentElementInMap->offset;
            found = true;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
    }
    if(!found) {
        return "";
    }
//        std::cout << "offset is " << offset << std::endl;
    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)partitionInfo + offset);
    return keyPlaceToAdd->data;
}

int removeKeyValByPartitionPointer(void* mappedPartition, uint64_t id) {

    PartitionInfo* partitionInfo = (PartitionInfo* )mappedPartition;
    uint64_t mapSize = partitionInfo->mapSize;

    MapNode* currentElementInMap = (MapNode* )(partitionInfo + 1);
    uint64_t offset;
    for(int i = 0; i < mapSize; i++) {
        uint64_t currentId = currentElementInMap->id;

        if(currentId == id) {
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

uint64_t readIdFromFile(std::string filepath) {
    std::string line;

    FILE *fp;
    size_t buffSize = 4096;
    char str[buffSize];
    memset(str, 0x00, buffSize);

    fp = fopen(filepath.c_str(), "r");
    if (fp == NULL){
        return 0;
    }

    while (fgets(str, buffSize, fp) != NULL) {
        line += str;
        memset(str, 0x00, buffSize);
    }
    fclose(fp);
    char *eptr;

    return strtoull(&line[0], &eptr, 10);
}

std::string getPrvKeyById(uint64_t id) {
    size_t fileSize = getFileSize(partition.c_str());
    //Open file
    int fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return "";
    }
    void* mappedPartition = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return "";
    }

    std::string key = getKeyValByPartitionPointer(mappedPartition, id);

    int ret = munmap(mappedPartition, fileSize);
    close(fd);
    assert(ret == 0);

    return key;
}

int removePrvKeyById(uint64_t id) {
    size_t fileSize = getFileSize(partition.c_str());
    //Open file
    int fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return fd;
    }
    void* mappedPartition = mmap(nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
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
        if(changedSize!= 0) {
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

void print(std::string str) {
    int charsInALine = 30;
    for(int i = 0; i < str.size(); i++) {
        std::cout << str[i];
        if(i != 0 && i % charsInALine == 0) {
            std::cout << std::endl;
        }
    }
}

std::string getPathToTmpPrvKeyStorage(std::string key) {
    std::string filepath = "/tmp/prvKey.pem";

    size_t maxKeySize = 4 * 4096 + 1024;
    char fileContent[maxKeySize];
    memset(fileContent, 0x00, maxKeySize);

    FILE* fp = fopen(filepath.c_str(), "w+");
    fprintf(fp, "%s", key.c_str());
    fclose(fp);

    return filepath;
}

AddKeyInfo write(RSA* r) {
    writeKeyToTemporaryFile(r);
    KeyNode keyNode = generateKeyNodeFromKeyInFile();

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "Kernel will add a key to partition with value" << std::endl;
    }
//        print(keyNode.keyContent);
    return {addKeyNodeToPartition(keyNode), data.numberOfKeys};
}
std::string read(std::string filepath) {
    uint64_t id = readIdFromFile(filepath);
    if(id == 0) {
        return "";
    }

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "Kernel Emualtion will give key with id: " <<  id << std::endl;
    }

    std::string prvKey = getPrvKeyById(id);
    if(prvKey.empty()) {
        return "";
    }

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "Kernel Emulation found a key with value" << std::endl;
    }
//        print(prvKey);
    return getPathToTmpPrvKeyStorage(prvKey);
}

std::string get(std::string filepath) {
    uint64_t id = readIdFromFile(filepath);
    if(id == 0) {
        return "";
    }

    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "Kernel Emualtion will give key with id: " <<  id << std::endl;
    }

    return getPrvKeyById(id);
}

int remove(std::string filepath) {
    uint64_t id = readIdFromFile(filepath);
    return id == 0 ? -1 : removePrvKeyById(id);
}



