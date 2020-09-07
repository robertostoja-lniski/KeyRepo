//
// Created by robert on 25.08.2020.
//

#include <sstream>
#include "../include/KernelEmulation.h"

uint64_t generateRandomId(){
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<uint64_t> uDist(0, UINT64_MAX);
    return uDist(gen);
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
    partitionInfo->fileContentSize = DEFAULT_MAP_SIZE * sizeof(MapNode);

    memcpy(&data, partitionInfo, sizeof(PartitionInfo));
    size_t fileSize = sizeof(PartitionInfo) + sizeof(KeyNode) * partitionInfo->mapSize;
    //Open file
    std::ofstream os(partition.c_str());
    os << "";
    os.close();

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
        close(fd);
        return 1;
    }

    memcpy(mappedPartition, partitionInfo, sizeof(PartitionInfo));
    MapNode* mapPosition = (MapNode* )((PartitionInfo* )mappedPartition + 1);
    for(int i = 0; i < partitionInfo->mapSize; i++) {
        MapNode* mapData = (MapNode* )malloc(sizeof(MapNode));
        mapData -> id = generateRandomId();
        mapData -> offset = 0;
        memcpy(mapPosition, mapData, sizeof(*mapData));
        mapPosition = mapPosition + 1;
        free(mapData);
    }

    printPartition(partitionStart);
    free(partitionInfo);
    int ret = munmap(mappedPartition, fileSize);
    assert(ret == 0);
    close(fd);
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
    std::ifstream keyTmpFile(tmpKeyStorage);
    if (keyTmpFile.is_open()) {
        while (getline(keyTmpFile,line)) {
            keyNode.keyContent += line;
        }
        keyTmpFile.close();
    }
    // when serialised there will be no access to bytes as std::string
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

uint64_t addKeyNodeToPartition(KeyNode keyNodeToAdd) {
    initFileIfNotDefined();
    size_t fileSize = getFileSize(partition.c_str()) + keyNodeToAdd.keySize + 16 * sizeof(keyNodeToAdd.keySize) + 16;
    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << "when adding new node file size is: " << fileSize << std::endl;
    }

    //Open file
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
    if(VERBOSE_LEVEL >= VERBOSE_LOW) {
        std::cout << std::endl << "file size is " << fileSize << std::endl;
    }

    void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    void* partitionStart = mappedPartition;

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
    assert(ret == 0);
    close(fd);

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
            std::cout << "For " << i << " node in map there was found id: " << id << " and offset " << offset <<  std::endl;
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
    for(int i = 0; i < mapSize; i++) {
        uint64_t offset = currentElementInMap->offset;
        if(offset == 0) {
            id = currentElementInMap->id;
            break;
        }
        currentElementInMap = currentElementInMap + 1;
    }

    MapNode* elementDataToUpdate = (MapNode* )malloc(sizeof(MapNode));
    if(!elementDataToUpdate) {
        return 1;
    }

    elementDataToUpdate->offset = offsetToAdd;
    elementDataToUpdate->id = id;

    assert(currentElementInMap->id == elementDataToUpdate->id);

    memcpy(currentElementInMap, elementDataToUpdate, sizeof(MapNode));
    free(elementDataToUpdate);


    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)(partitionInfo + 1) + offsetToAdd);
    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << "map start " << partitionInfo + 1 << std::endl;
        std::cout << "Key place to add (right after map end) is: " << keyPlaceToAdd << std::endl;
        std::cout << "Key to add size: " << keyNodeToAdd.keySize << std::endl;
        std::cout << "Key to add content len: " << keyNodeToAdd.keyContent.size() << std::endl;
        std::cout << "Key place to add data before cpy: " << keyPlaceToAdd->data <<std::endl;
    }

    memcpy(keyPlaceToAdd->data, keyNodeToAdd.keyContent.c_str(), keyNodeToAdd.keySize);

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
    KeyPartitionNode *keyPlaceToAdd = (KeyPartitionNode* )((uint8_t *)(partitionInfo + 1) + offset);
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
            KeyPartitionNode* keyPlaceToRemove = (KeyPartitionNode* )((uint8_t *)(partitionInfo + 1) + offset);
            memset(keyPlaceToRemove, 0x00, sizeof(keyPlaceToRemove->keySize) + sizeof(keyPlaceToRemove->data));
            PartitionInfo* partitionInfoToChange = (PartitionInfo* )mappedPartition;
            partitionInfoToChange->numberOfKeys -= 1;
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
    std::ifstream myfile (filepath);
    if (myfile.is_open()) {
        getline (myfile,line);
        myfile.close();
        uint64_t id;
        std::istringstream iss(line);
        iss >> id;
        return id;
    }
    return 0;
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
    assert(ret == 0);
    close(fd);

    return key;
}

int removePrvKeyById(uint64_t id) {
    size_t fileSize = getFileSize(partition.c_str());
    //Open file
    int fd = open(partition.c_str(), O_RDWR, 0);
    if(fd < 0) {
        return 1;
    }
    void* mappedPartition = mmap(nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, fd, 0);
    if(mappedPartition == MAP_FAILED) {
        close(fd);
        return 1;
    }

    int removeRet = removeKeyValByPartitionPointer(mappedPartition, id);

    int ret = munmap(mappedPartition, fileSize);
    assert(ret == 0);
    close(fd);

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
    std::string header = "-----BEGIN RSA PRIVATE KEY-----";
    std::string bottom = "-----END RSA PRIVATE KEY-----";
    int charsInRow = 64;
    std::string filepath = "/tmp/prvKey.pem";
    std::ofstream os;
    os.open(filepath);
    os << header << "\n";
    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << header << "\n";
    }

    for(int i = header.size(); i < key.size() - header.size() - bottom.size();) {
        std::string nextLine;
        for(int j = 0; j < charsInRow; j++) {
            nextLine += key[i];
            i++;
        }
        os << nextLine << "\n";

        if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
            std::cout << nextLine << "\n";
        }
    }
    os << bottom << "\n";
    if(VERBOSE_LEVEL >= VERBOSE_HIGH) {
        std::cout << bottom << "\n";
    }
    os.close();
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



