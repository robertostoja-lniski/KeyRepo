#define EMULATION 1

#if EMULATION == 1
    #include "../include/KernelEmulation.h"
    uint64_t remove_fragmentation_on = 1;
    uint64_t map_optimisation_on = 1;
#else
    #include "KeyRepoHeaders.h"
#endif

// partition file IO
void* get_buffered_file(const char* filepath, size_t* filesize, size_t extra_size);
size_t set_buffered_file(const char* partition, char** buf, size_t bufsize);

// dummy compilation fix
#if EMULATION == 1
    void printk(const char* dummy, ...) {}

    void get_random_bytes(uint64_t* n, size_t size) {
        assert(size == 8);
        uint64_t r = 0;
        for (int i=0; i<64; i++) {
            r = r*2 + rand()%2;
        }
        *n = r;
    }

    void copy_to_user(void* dest, const void* src, size_t size) {
        memcpy(dest, src, size);
    }

    void copy_from_user(void* dest, const void* src, size_t size) {
        memcpy(dest, src, size);
    }

#endif

size_t set_buffered_file(const char* partition, char** buf, size_t bufsize) {
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

    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("File open\n");
    fp = filp_open(partition, O_RDWR | O_TRUNC, 0644);
    if (IS_ERR(fp)) {
        printk("Cannot open file\n");
        return -1;
    }

    printk("Next action: kernel write\n");
    printk("Buf size is: %zu bufsize\n", bufsize);
    size_t ret = kernel_write(fp, *buf, bufsize, &pos);
    printk("Bytes written %zu : bytes wanted to be read : %zu\n", ret, bufsize);

    printk("Next action: file close\n");
    filp_close(fp, NULL);
    printk("Next action: kfree on *buf\n");
    kfree(*buf);
    
    printk("Next action: set fs\n");
    set_fs(fs);
    printk("Exiting: Set buffered file\n");
    return ret;
#endif
}

void *get_buffered_file(const char* filepath, size_t* size, size_t extra_size) {
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

    printk("Next action: getting fs\n");
    fs = get_fs();
    printk("Next action: setting fs\n");
    set_fs(KERNEL_DS);

    printk("Opening partion\n");
    fp = filp_open(partition, O_RDWR, 0644);
    if (IS_ERR(fp)) {
        printk("Open file error!\n");
        return NULL;
    }

    printk("Next action: stat kmalloc\n");
    stat =(struct kstat *) kmalloc(sizeof(struct kstat), GFP_KERNEL);
    if (!stat) {
        printk("Kmalloc failed\n");
        return ERR_PTR(-ENOMEM);
    }

    printk("Next action: vfs_stat\n");
    vfs_stat(partition, stat);
    *size = stat->size;

    printk("File size is %zu\n", *size);

    printk("Next action: buffer\n");
    printk("*size is %zu\n", *size);
    printk("extra is %zu\n", extra_size);
    buf = kmalloc(*size + extra_size, GFP_KERNEL);
    if (!buf) {
        kfree(stat);
        printk("malloc input buf error!\n");
        NULL;
    }

    printk("Next action: kernel read\n");
    size_t ret = (size_t)kernel_read(fp, buf, *size + extra_size, &pos);
    printk("Bytes read %zu : bytes wanted to be read : %zu\n", ret, *size + extra_size);

    printk("Next action: filp_close\n");
    filp_close(fp, NULL);
    printk("Next action: kfree\n");
    kfree(stat);
    printk("Next action: set_fs\n");
    set_fs(fs);
    printk("Exiting get buffer file\n");
    return buf;
#endif
}

// private
uint64_t generate_random_id(void* mapped_partition){

    printk("Entering generate random id\n");

    printk("Entering generate random id\n");

    int generateTrials = 10;
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    int foundSameId = 0;
    uint64_t newId;
    while(generateTrials--) {

        get_random_bytes(&newId, sizeof(newId));
        printk("New id is %zu\n", newId);

        map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
        uint64_t id;
        int i;
        for(i = 0; i < map_size; i++) {
            id = current_elem_in_map->id;
            // 0 and 1 for error codes
            if(id == newId || id == 1) {
                foundSameId = 1;
                printk("Matched tested id is: %zu\n", id);
                break;
            }

            current_elem_in_map++;
        }

        if(foundSameId == 0) {
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
        return 0;
    }

    partition_info* partion_metadata = (partition_info* )malloc(sizeof(partition_info));
    if(!partion_metadata) {
        return 1;
    }

#else

    printk("Entering init file if not defined %s\n", partition);
    struct file *fp;

    mm_segment_t fs;
    printk("Next action: getting fs\n");
    fs = get_fs();
    printk("Next action: setting fs\n");
    set_fs(KERNEL_DS);

    fp = filp_open(partition, O_RDWR, 0644);
    if (!IS_ERR(fp)) {
        filp_close(fp, NULL);
        set_fs(fs);
        return;
    }
    printk("Open file success\n");
    filp_close(fp, NULL);
    printk("File closed\n");

    printk("Open file error! - maybe does not exist\n");
    fp = filp_open(partition, O_CREAT, 0644);

    if(IS_ERR(fp)) {
        printk("Open file for create unhandled error. Exiting\n");
        set_fs(fs);
        return 1;
    }

    printk("Open file success\n");
    filp_close(fp, NULL);
    printk("File closed\n");
    printk("Next action: set_fs\n");
    set_fs(fs);

    printk("Partition info to be allocated\n");
    partition_info* partion_metadata = (partition_info* )kmalloc(sizeof(partition_info), GFP_KERNEL);
    if(!partion_metadata) {
        printk("Allocation failed, exiting\n");
        return 1;
    }

#endif

    partion_metadata->map_size = DEFAULT_MAP_SIZE;
    partion_metadata->number_of_keys = DEFAULT_NUMBER_OF_KEYS;
    partion_metadata->file_content_size = DEFAULT_MAP_SIZE * sizeof(map_node) + sizeof(partition_info);
    partion_metadata->free_slot = partion_metadata->file_content_size;

    uint64_t file_size = partion_metadata->file_content_size;

#if EMULATION == 1
    void* partitionStart = malloc(file_size);
#else
    printk("New partition file to be allocated\n");
    void* partitionStart = kmalloc(file_size, GFP_KERNEL);
    printk("After allocation\n");
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
    memset(partitionStart, 0x00, file_size);

    printk("Memset ok\n");

    memcpy(partitionStart, partion_metadata, sizeof(partition_info));

    printk("Memcpy ok\n");

    map_node* mapPosition = (map_node* )((partition_info* )partitionStart + 1);
    int i;
    for(i = 0; i < partion_metadata->map_size; i++) {

#if EMULATION == 1
        map_node* mapData = (map_node* )malloc(sizeof(map_node));
#else
        // printk("Malloc to be used\n");
        map_node* mapData = (map_node* )kmalloc(sizeof(map_node), GFP_KERNEL);
        // printk("Malloc ok\n");
#endif

    printk("Memset ok\n");

    memcpy(partitionStart, partitionInfo, sizeof(PartitionInfo));

    printk("Memcpy ok\n");

    MapNode* mapPosition = (MapNode* )((PartitionInfo* )partitionStart + 1);
    int i;
    for(i = 0; i < partitionInfo->mapSize; i++) {

#if EMULATION == 1
        MapNode* mapData = (MapNode* )malloc(sizeof(MapNode));
#else
        // printk("Malloc to be used\n");
        MapNode* mapData = (MapNode* )kmalloc(sizeof(MapNode), GFP_KERNEL);
        // printk("Malloc ok\n");
#endif

        if(!mapData) {
            return 1;
        }
        mapData -> id = 0;
        mapData -> offset = 0;
        mapData -> size = 0;
        memcpy(mapPosition, mapData, sizeof(*mapData));
        // printk("Memcpy ok\n");
        mapPosition = mapPosition + 1;
#if EMULATION == 1
        free(mapData);
#else
        kfree(mapData);
#endif
    }

    print_partition(partitionStart);
#if EMULATION == 1
    free(partion_metadata);
#else
    printk("Map initialised!\n");
    kfree(partion_metadata);
    printk("Free ok\n");
#endif

    size_t ret;
    printk("Set buffered file\n");
    ret = set_buffered_file(partition, (char** )&partitionStart, file_size);
    printk("After set ret value is %zu\n", ret);
    if(ret != file_size) {
        printk("Set failed\n");
        return 1;
    }

    printk("Exiting init file if not defined with value 0\n");

    return 0;
}
uint64_t remove_fragmentation(partition_info* partion_metadata) {
    printk("Entering: remove fragmentation\n");

    // create new map
#if EMULATION == 1
    map_node* tmpMap = (map_node* )malloc(sizeof(map_node) * partion_metadata->number_of_keys);
#else
    map_node* tmpMap = (map_node* )kmalloc(sizeof(map_node) * partion_metadata->number_of_keys, GFP_KERNEL);
#endif

    if(!tmpMap) {
        return 0;
    }

    uint64_t map_size = partion_metadata->map_size;
    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    int tmpMapIndex = 0;
    uint64_t changedOffset = DEFAULT_MAP_SIZE * sizeof(map_node) + sizeof(partition_info);

    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t offset = current_elem_in_map->offset;
        uint64_t id = current_elem_in_map->id;
        uint64_t size = current_elem_in_map->size;
        int mode = current_elem_in_map->mode;

#if EMULATION == 1
        int uid = current_elem_in_map->uid;
        int gid = current_elem_in_map->gid;
#else
        kuid_t uid = current_elem_in_map->uid;
        kgid_t gid = current_elem_in_map->gid;
#endif
        if(id == 0 && offset != 0) {
            offset = 0;
        }

        if(id != 0) {

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

            printk("Next action: memcpy (from kernel to kernel)\n");
            memcpy(keyContent, ((uint8_t* )partion_metadata + offset), size);
            printk("Next action: memcpy (from kernel to kernel)\n");
            memcpy(((uint8_t* )partion_metadata + changedOffset), keyContent, size);
            printk("Memcpy x2 done!\n");

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
            tmpMapIndex++;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    current_elem_in_map = (map_node* )(partion_metadata + 1);
    printk("Next action: memcpy (from kernel to kernel)\n");
    memcpy(current_elem_in_map, tmpMap, sizeof(map_node) * partion_metadata->number_of_keys);
    printk("Next action: Memset - zeros in rest of bytes\n");
    memset(current_elem_in_map + partion_metadata->number_of_keys, 0x00, sizeof(map_node) * (MAX_KEY_NUM - partion_metadata->number_of_keys));
    printk("Memset and memcpy done!\n");

#if EMULATION == 1
    free(tmpMap);
#else
    kfree(tmpMap);
#endif

    printk("Exiting: remove fragmentation\n");
    return changedOffset;
}
void print_partition(const void* mapped_partition) {
    return;
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t keys = partion_metadata->number_of_keys;
    uint64_t offsetToAdd = partion_metadata->file_content_size;
    uint64_t map_size = partion_metadata->map_size;

    printf("This partition has: %llu keys.\n", keys);
    printf("File content size is: %llu.\n", offsetToAdd);
    printf("Next free slot is: %llu.\n", partion_metadata->free_slot);

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t id;
    int i;
    for(i = 0; i < 5; i++) {
        printf("%llu %llu %llu", current_elem_in_map->id, current_elem_in_map->offset, current_elem_in_map->size);

        char *tmp = (char* )malloc(current_elem_in_map->size + 1);
        if(!tmp) {
            return;
        }

        memcpy(tmp, (uint8_t* )mapped_partition + current_elem_in_map->offset, current_elem_in_map->size);
        tmp[current_elem_in_map->size] = 0;

        printf(" %s\n", tmp);
        free(tmp);
        current_elem_in_map += 1;
    }

}

#if EMULATION == 1
int add_key_to_partition(const char* key, uint64_t key_len, uint64_t *id) {
#else
int add_key_to_partition(char* __user key, uint64_t __user *id) {
#endif

    printk("Entering add key node to partition\n");
    if(initFileIfNotDefined() != 0) {
        return -2;
    }
    printk("Partition initialised\n");

    size_t file_size;
    printk("Loading file to buffer\n");
    void* mapped_partition = get_buffered_file(partition, &file_size, key_len);
    printk("File loaded to buffer\n");
    if(!mapped_partition || file_size == 0) {
        return -1;
    }

    uint64_t key_num = ((partition_info* )mapped_partition)->number_of_keys;
    if(key_num == MAX_KEY_NUM) {
        free(mapped_partition);
        return -2;
    }

    printk("Adding key to partition\n");

    uint64_t first_slot_before_add = ((partition_info* )mapped_partition)->free_slot;
    int addRet = add_key_by_partition_pointer(mapped_partition, key, key_len, id);
    if(addRet != 0) {
        return -1;
    }
    printk("Key to partition added\n");

    printk("Saving buffer with size %zu\n", file_size);
    printk("And extra size: %zu\n", key_len);

    uint64_t first_slot_after_add = ((partition_info* )mapped_partition)->free_slot;
    if(first_slot_before_add != first_slot_after_add) {
        file_size = first_slot_after_add;
    }
    print_partition(mapped_partition);
    if(set_buffered_file(partition, (char** )&mapped_partition, file_size) != file_size) {
        printk("Error in saving to buffer\n");
        return -1;
    }

    printk("Exiting add key node to partition\n");
    return addRet;
}
#if EMULATION == 1
int add_key_by_partition_pointer(void* mapped_partition, const char* key, uint64_t key_len, uint64_t *id) {
#else
int add_key_by_partition_pointer(void* mapped_partition, char* __user key, uint64_t __user *id) {
#endif

    printk("Entering add key node to partition\n");
    printk("Key value is: %s\n", key);
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t offsetToAdd = partion_metadata->free_slot;

    if(key_len + partion_metadata->file_content_size > MAX_PARTITION_SIZE) {
        return -1;
    }

    uint64_t map_size = partion_metadata->map_size;
    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t next_id;
    int is_storage_optimised = 0;
    int i;

    printk("Moving to first map node succeeded\n");
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;

        if (current_id == 0) {
            printk("Id is\n");
            next_id = generate_random_id(mapped_partition);

#if EMULATION == 1
            if(map_optimisation_on) {
#endif

                // optimised storage of keys shorter than 4096
                if (key_len <= current_elem_in_map->size) {

                    // if not first key in this slot
                    if (current_elem_in_map->offset != 0) {
                        offsetToAdd = current_elem_in_map->offset;
                        is_storage_optimised = 1;
                    }
                }

#if EMULATION == 1
            }
#endif

#if EMULATION == 1
            // memcpy is to make emulation as close to final coden, which uses copy_to_user
            memcpy(id, &next_id, sizeof(next_id));
#else
            printk("Assigning NEW CHANGE id\n");
            copy_to_user(id, &next_id, sizeof(next_id));
            printk("Copied and exiting map iteration.\n");
#endif
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

#if EMULATION == 1
    map_node* new_content = (map_node* )malloc(sizeof(map_node));
#else
    printk("Allocation of map node to be updated\n");
    map_node* new_content = (map_node* )kmalloc(sizeof(map_node), GFP_KERNEL);
#endif

    if(!new_content) {
        return -1;
    }

    new_content->offset = offsetToAdd;
    new_content->id = next_id;
    new_content->size = key_len;
    printk("Next action: get uid\n");
    new_content->uid = getuid();
    printk("Next action: get gid\n");
    new_content->gid = getgid();

#if EMULATION == 1
    new_content->mode = 600;
#else
    new_content->mode = 600;
#endif

    printk("Element to update has values of:\n");
    printk("offset %zu:\n", new_content->offset);
    printk("id: %zu\n", new_content->id);
    printk("size: %zu\n", new_content->size);

    memcpy(current_elem_in_map, new_content, sizeof(map_node));

    printk("Copied element to update has values of:\n");
    printk("offset %zu:\n", current_elem_in_map->offset);
    printk("id: %zu\n", current_elem_in_map->id);
    printk("size: %zu\n", current_elem_in_map->size);


#if EMULATION == 1
    free(new_content);
#else 
    kfree(new_content);
#endif


    uint8_t *key_address = (uint8_t *)partion_metadata + offsetToAdd;

#if EMULATION == 1
    memcpy(key_address, key, key_len);
#else
    mm_segment_t fs;
    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    copy_from_user(key_address, key, key_len);
    printk("Memcpy successful\n");

    set_fs(fs);
#endif

#if EMULATION == 1
    partition_info* partition_metadata_to_be_updated = (partition_info* )malloc(sizeof(partition_info));
#else
    partition_info* partition_metadata_to_be_updated = (partition_info* )kmalloc(sizeof(partition_info), GFP_KERNEL);
#endif

    partition_metadata_to_be_updated->map_size = partion_metadata->map_size;
    partition_metadata_to_be_updated->number_of_keys = partion_metadata->number_of_keys + 1;
    partition_metadata_to_be_updated->file_content_size = partion_metadata->file_content_size + key_len;

    if(!is_storage_optimised) {
        partition_metadata_to_be_updated->free_slot = partion_metadata->free_slot + key_len;
    } else {
        partition_metadata_to_be_updated->free_slot = partion_metadata->free_slot;
    }

    memcpy(partion_metadata, partition_metadata_to_be_updated, sizeof(partition_info));

#if EMULATION == 1
    free(partition_metadata_to_be_updated);
#else
    kfree(partition_metadata_to_be_updated);
    kfree(keyNodeToAdd);
#endif

    print_partition(mapped_partition);
    printk("Exiting add key Node\n");

    return 0;
}
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t key_len) {

    printk("Entering: get key val by pp\n");
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t offset;
    int found = 0;
    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked");
            if(!can_read(current_elem_in_map->mode, current_elem_in_map->uid, current_elem_in_map->gid)) {
                return -2;
            }

            printk("ID FOUND: %zu\n", current_id);
            printk("SIZE FOUND: %zu\n", current_elem_in_map->size);
            printk("OFFSET FOUND: %zu\n", current_elem_in_map->offset);

            offset = current_elem_in_map->offset;
            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    if(!found) {
        return -1;
    }
    printk("Found\n");
    uint64_t allocation_size = current_elem_in_map->size;

    if(key_len < allocation_size) {
        printk("Allocation size: %zu is shortened to key len of %zu\n", allocation_size, key_len);
        allocation_size = key_len;
    }

#if EMULATION == 1
    memcpy(keyVal, (uint8_t *)partion_metadata + offset, allocation_size);
    memset(keyVal + allocation_size, 0x00, sizeof(*keyVal));
#else
    mm_segment_t fs;
    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);
    
    printk("cpy to usr\n");
    copy_to_user(keyVal, (uint8_t *)partion_metadata + offset, allocation_size);
    char dummy = NULL;
    copy_to_user(keyVal + allocation_size, &dummy, sizeof(dummy));
    printk("Copied key is %s\n", key_address->data);

    set_fs(fs);
#endif

    printk("Exiting: CHANGED get key val by pp");
    return 0;
}
int get_key_size_by_partition_pointer(void* mapped_partition, uint64_t id, uint64_t* key_len) {

    printk("Entering: get key size by pp\n");
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t offset;
    int found = 0;
    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked\n");
            if(!can_read(current_elem_in_map->mode, current_elem_in_map->uid, current_elem_in_map->gid)) {
                return -2;
            }

            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    if(!found) {
        return -1;
    }

    uint64_t size = current_elem_in_map->size;
    *key_len = size;
#if EMULATION == 1
    memcpy(key_len, &size, sizeof(*key_len));
#else
    printk("Copying to user key_len\n");
    copy_to_user(key_len, &size, sizeof(*key_len));
    printk("Exiting: get key size by pp\n");
#endif
    return 0;
}
int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* key_mode) {

    printk("Entering: get key mode by pp\n");

    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t offset;
    int found = 0;
    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {
            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return -1;
    }

#if EMULATION == 1
    memcpy(key_mode, &current_elem_in_map->mode, sizeof(*key_mode));
#else

    mm_segment_t fs;
    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("cpy to usr\n");
    char dummy = NULL;
    copy_to_user(key_mode, &current_elem_in_map->mode, sizeof(*key_mode));
    printk("Copied mode %d\n", current_elem_in_map->mode);

    set_fs(fs);
#endif

    printk("Exiting: get key mode by pp\n");
    return 0;
}
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int key_mode) {

    printk("Entering: set key mode by pp\n");
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    int found = 0;
    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {
            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return -1;
    }

    current_elem_in_map->mode = key_mode;
    printk("Exiting: set key mode by pp\n");
    return 0;
}
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id) {

//    printf("Removal start: id %zu\n", id);
    print_partition(mapped_partition);
    printk("Entering: key val by pp\n");
    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t map_size = partion_metadata->map_size;

    map_node* current_elem_in_map = (map_node* )(partion_metadata + 1);
    uint64_t offset;
    int i;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;

        if(current_id == id) {

            printk("Checking write rights\n");
            if(!can_write(current_elem_in_map->mode, current_elem_in_map->uid, current_elem_in_map->gid)) {
                return -2;
            }

            offset = current_elem_in_map->offset;
            uint64_t sizeToRemove = current_elem_in_map->size;
//            current_elem_in_map->size = 0;
            current_elem_in_map->id = 0;
            memset((uint8_t *)partion_metadata + offset, 0x00, sizeToRemove);
            partion_metadata->number_of_keys -= 1;
            partion_metadata->file_content_size -= sizeToRemove;
            printk("memcpy done");
            printk("Exiting: key val by pp (with failure)\n");
//            printf("Removal end\n");
            print_partition(mapped_partition);
            return 0;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    printk("Exiting: key val by pp (with failure)\n");
    return -1;
}

#if EMULATION == 1
int get_prv_key_by_id(const uint64_t id, char* prvKey, uint64_t key_len) {
#else
int get_prv_key_by_id(const uint64_t id, char __user *prvKey, uint64_t key_len) {
#endif

    printk("Entering: get prv key by id\n");
    size_t file_size;
    printk("Next action: get buffered file\n");
    void *mapped_partition = get_buffered_file(partition, &file_size, 0);
    if (!mapped_partition || file_size == 0) {
        return -1;
    }

    printk("Next action: get key val by pp\n");

//    printf("Before get: %zu\n", id);
    print_partition(mapped_partition);
    int get_key_ret = get_key_by_partition_pointer(mapped_partition, id, prvKey, key_len);
//    printf("After get\n");
    print_partition(mapped_partition);
    // printk("Key from partition is %s\n", *prvKey);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if (get_key_ret == -1) {
        return -1;
    }

    printk("Exiting: get prv key by id\n");
    return 0;
}
int get_prv_key_size_by_id(const uint64_t id, uint64_t* size) {

    printk("Entering: get prv key size by id\n");
    size_t file_size;
    printk("Next action: get buffered file\n");
    void* mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(!mapped_partition || file_size == 0) {
        return -1;
    }

    printk("Next action: get key size by pp\n");
    int get_key_size_ret = get_key_size_by_partition_pointer(mapped_partition, id, size);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if(get_key_size_ret == -1) {
        return -1;
    }

    printk("Exiting: get prv key size by id\n");
    return 0;
}
int remove_private_key_by_id(uint64_t id) {

    printk("Entering: remove prv key by id\n");
    size_t file_size;
    printk("Next action: get buffered file\n");
    void* mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    printk("Next action: remove key val by pp\n");
    int removeRet = remove_key_by_partition_pointer(mapped_partition, id);
    if(removeRet != 0) {
        
#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

        return -1;
    }

    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t file_content_size = partion_metadata->file_content_size;
    uint64_t number_of_keys = partion_metadata->number_of_keys;
    uint64_t metadataSize = sizeof(partition_info) + partion_metadata->map_size * sizeof(map_node);

#if REMOVE_FRAGMENTATION == 1

#if EMULATION == 1
    if(remove_fragmentation_on) {
#endif

        if (number_of_keys > 0 && file_content_size * REDUCTION_PARAM <= file_size) {

            printk("IMPORTANT: remove fragmentation to be triggered\n");
            uint64_t changedSize = remove_fragmentation(partion_metadata);

#if EMULATION == 1
            if (changedSize != 0) {
                int fdDef = open(partition, O_RDWR, 0);
                if (fdDef < 0) {
                    return -1;
                }
                ftruncate(fdDef, changedSize);
                close(fdDef);
            }
#endif
            printk("trunc temporary ommited\n");
            partion_metadata->file_content_size = changedSize;
            partion_metadata->free_slot = changedSize;
            file_size = changedSize;
        } else if (number_of_keys == 0) {
            printk("Number of keys = 0\n");
#if EMULATION == 1

            memset((uint8_t *) partion_metadata + sizeof(partition_info), 0x00, metadataSize - sizeof(partition_info));
            partion_metadata->file_content_size = metadataSize;
            partion_metadata->free_slot = metadataSize;

            int fdDef = open(partition, O_RDWR, 0);
            if (fdDef < 0) {
                return -1;
            }
            ftruncate(fdDef, metadataSize);
            close(fdDef);
#endif
            printk("trunc temporary ommited\n");
            file_size = metadataSize;
        }
#endif

#if EMULATION == 1
    }
#endif

#endif

    printk("Next action: set bufferd file\n");
    if(set_buffered_file(partition, (char** )&mapped_partition, file_size) != file_size) {
        return -1;
    }

    printk("Entering: remove prv key by id\n");
    return removeRet;
}

// public
#if EMULATION == 1
int get_key_num() {
#else
SYSCALL_DEFINE0(get_current_key_num) {
#endif

    printk("Next action: get buffered file\n");
    size_t file_size;
    void* mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    partition_info* partion_metadata = (partition_info* )mapped_partition;
    uint64_t key_num = (partion_metadata->number_of_keys) % INT_MAX;
    free(mapped_partition);

    return key_num;
}

#if EMULATION == 1
int write_key(const char* key, const size_t key_len, uint64_t* id) {
#else
SYSCALL_DEFINE3(write_key, const char __user *, key, const size_t, key_len, uint64_t __user *, id) {
    printk("Entering write key\n");
#endif

    if(key_len == 0) {
        return -1;
    }

    uint64_t usedLen = key_len;
    if(key_len > strlen(key)) {
        usedLen = strlen(key);
    }

    printk("Kernel space memory successfully allocated, %zu of bytes to be copied to keyNode\n", key_len);

    int ret = add_key_to_partition(key, usedLen, id);
    if(ret == -1 || ret == -2) {
        return ret;
    }

#if EMULATION == 0
    printk("Exiting write key\n");
#endif

    return 0;
}

#if EMULATION == 1
int read_key(const uint64_t id, char* key, uint64_t key_len) {
#else
SYSCALL_DEFINE3(read_key, const uint64_t, id, char __user *, key, uint64_t, key_len) {
#endif

    printk("\n\nEntering: readKey recompiled\n");
    if(id == 0 || key_len == 0) {
       return -1;
    }

    printk("Next action: get prv key by id\n");
    int ret = get_prv_key_by_id(id, key, key_len);
    if(ret == -1) {
        return -1;
    }
    memset(key + key_len, 0x00, 1);

    printk("Exiting: readKey\n");
    return 0;
}

#if EMULATION == 1
int remove_key(const uint64_t id) {
#else
SYSCALL_DEFINE1(remove_key, const uint64_t __user *, id) {
#endif

    printk("Entering and soon exiting remove key\n");
    return id == 0 ? -1 : remove_private_key_by_id(id);
}


// for kernel kuid_t instead of uid
#if EMULATION == 1
int get_mode(const uint64_t id, int* output) {
#else
SYSCALL_DEFINE2(get_mode, const uint64_t __user*, id, int __user **, output) {
#endif

    printk("Entering: get mode\n");
    if(id == 0) {
        return -1;
    }

    size_t file_size;

    printk("Next action: get buffered file\n");
    void* mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    printk("Next action: get key mode by pp\n");
    int get_key_ret = get_key_mode_by_partition_pointer(mapped_partition, id, output);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if(get_key_ret == -1) {
        return -1;
    }

    printk("Exiting: get mode\n");
    return 0;
}

#if EMULATION == 1
int set_mode(const uint64_t id, int new_mode) {
    if(SU_SECURITY == 1 && geteuid() != 0) {
        return -1;
    }
#else
SYSCALL_DEFINE2(set_mode, const uint64_t __user*, id, int*, new_mode) {
     if(SU_SECURITY == 1 && geteuid().val != 0) {
        return -1;
    }
#endif

    if(new_mode > 777) {
        return -1;
    }
    int h_digit = (new_mode / 100);
    int d_digit = (new_mode / 10) % 10;
    int digit = new_mode % 10;

    if(!(h_digit <= 7 && d_digit <= 7 && digit <= 7)) {
        return -1;
    }

    printk("Entering: set mode\n");
    if(id == 0) {
        return -1;
    }

    size_t file_size;
    printk("Next action: get buffered file\n");
    void* mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    printk("Next action: set key mode by pp\n");
    int get_key_ret = set_key_mode_by_partition_pointer(mapped_partition, id, new_mode);
    if(get_key_ret == -1) {

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif
        return -1;
    }

    printk("Next action: set buffered file\n");
    if(set_buffered_file(partition, (char** )&mapped_partition, file_size) != file_size) {
        return -1;
    }

    printk("Exiting: set mode\n");
    return 0;
}

#if EMULATION == 1
int can_read(int mode, int uid, int gid) {

    int user_read = (mode / 100) & READ_MASK;
    int group_read = ((mode / 10) % 10) & READ_MASK;
    int others_read = (mode % 10) & READ_MASK;

    return user_read && uid == getuid() || group_read && gid == getgid() || others_read && uid != getuid();
}
int can_write(int mode, int uid, int gid) {
    int user_write = (mode / 100) & WRITE_MASK;
    int group_write = ((mode / 10) % 10) & WRITE_MASK;
    int others_write = (mode % 10) & WRITE_MASK;

    return user_write && uid == getuid() || group_write && gid == getgid() || others_write && uid != getuid();
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

int can_read(int mode, kuid_t uid, kgid_t gid) {
    int user_read = (mode / 100) & READ_MASK;
    int group_read = ((mode / 10) % 10) & READ_MASK;
    int others_read = (mode % 10) & READ_MASK;
    printk("Entering: can read\n");

    return (user_read && (uid.val == getuid().val)) || (group_read && (gid.val == getgid().val)) || (others_read && (uid.val != getuid().val));
}

int can_write(int mode, kuid_t uid, kgid_t gid) {
    int user_write = (mode / 100) & WRITE_MASK;
    int group_write = ((mode / 10) % 10) & WRITE_MASK;
    int others_write = (mode % 10) & WRITE_MASK;

    printk("Entering: can write\n");
    return (user_write && (uid.val == getuid().val)) || (group_write && (gid.val == getgid().val)) || (others_write && ((uid.val != getuid().val)));
}

#endif

#if EMULATION == 1
int get_key_size(const uint64_t id, uint64_t* size) {
#else
    SYSCALL_DEFINE2(get_key_size, const uint64_t __user*, id, uint64_t*, size) {
#endif

    printk("Entering: get key size\n");
    if(id == 0) {
        return -1;
    }

    printk("Next action: get prv key size by id\n");
    int ret = get_prv_key_size_by_id(id, size);
    if(ret == -1) {
        return -1;
    }

    printk("Exiting: get key size\n");
    return 0;
}
