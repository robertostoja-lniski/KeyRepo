#define EMULATION 1

#if EMULATION == 1
    #include "../include/KernelEmulation.h"
    uint64_t remove_fragmentation_on = 1;
    uint64_t map_optimisation_on = 1;
#else
    #include "KeyRepoHeaders.h"
#endif

// changes are done to buffer in place for optimization purposes
int encrypt_data_at_rest(char* buf, size_t len, const char* password) {

    if (buf == NULL) {
        return -1;
    }

    if (password == NULL) {
        return -2;
    }

    for (size_t i = 0; i < len; i++) {
        buf[i] -= 30;
    }

    return 0;
}

// changes are done to buffer in place for optimization purposes
int decrypt_data_at_rest(char* buf, size_t len, const char* password) {

    if (buf == NULL) {
        return -1;
    }

    if (password == NULL) {
        return -2;
    }

    for (size_t i = 0; i < len; i++) {
        buf[i] += 30;
    }

    return 0;
}

void* get_buffered_file(const char* filepath, size_t* filesize, size_t extra_size);
size_t set_buffered_file(const char* file, char** buf, size_t bufsize, int trunc, int offset);

int fast_modulo(uint64_t num, uint32_t pow_of_2) {

    uint64_t tmpNum = num >> pow_of_2;
    tmpNum = tmpNum << pow_of_2;
    return (int)(num - tmpNum);
}

int get_magic_offset(void* mapped_partition) {
    uint64_t*           first_byte;
    int                 help_counter;
    partition_info      *part_info;

    first_byte = (uint64_t* )mapped_partition;
    help_counter = 0;

    while(*first_byte != MAGIC || help_counter > MAX_PARTITION_SIZE) {

        part_info = (partition_info *) mapped_partition;
        first_byte = (uint64_t *) ((uint8_t *) first_byte + 1);
        help_counter++;
    }

    if(help_counter > MAX_PARTITION_SIZE) {
        return -1;
    }
    return help_counter;
}

// kernel functions signatures defined for user space
#if EMULATION == 1
    void printk(const char* dummy, ...) {
//	 printf(dummy);
    }
    void get_random_bytes(uint64_t* n, size_t size) {
        assert(size == sizeof(*n));
        uint64_t r = 0;
        for (int i=0; i<64; i++) {
            r = r*2 + rand()%2;
        }
        *n = r;
    }
#endif

// trunc param is used only when called from kernel
// in emulation ftrunc() function is used
size_t set_buffered_file(const char* file, char** buf, size_t bufsize, int trunc, int offset) {
#if EMULATION == 1
    FILE* fd = fopen(file, "w");
    if(fd == NULL) {
        free(buf);
        return -1;
    }

    fseek(fd, offset, SEEK_SET);

    encrypt_data_at_rest(*buf, bufsize, "dummy");
    size_t ret = fwrite(*buf , sizeof(char) , bufsize, fd);

    fclose(fd);
    free(*buf);

    return ret;
#else
    struct file     *fp;
    mm_segment_t    fs;
    size_t          ret;
    loff_t          pos;

    printk("Entering: Set buffered file\n");

    pos = offset;

    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("File open\n");

    // truncate only if necessary O_TRUNC flag hugely decreses efficiency
    if(trunc) {
            fp = filp_open(partition, O_RDWR | O_TRUNC, 0644);
    } else {
            fp = filp_open(partition, O_RDWR , 0644);
    }

    if (IS_ERR(fp)) {
        printk("Cannot open file\n");
        return -1;
    }

    printk("Next action: kernel write\n");
    printk("Buf size is: %lu bufsize\n", bufsize);
    ret = kernel_write(fp, *buf, bufsize, &pos);
    printk("Bytes written %lu : bytes wanted to be written : %lu\n", ret, bufsize);

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
    if(fp == NULL) {
        return NULL;
    }

    uint8_t* buf = NULL;

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

    decrypt_data_at_rest(source, *size, "dummy");
    return (void* )source;
#else

    struct kstat        *stat;
    struct file         *fp;
    mm_segment_t        fs;
    loff_t              pos;
    size_t              ret;
    char                *buf;

    printk("Entering get buffer file\n");
    pos = 0;

    printk("Next action: getting and setting fs for kernel pool\n");
    fs = get_fs();
    set_fs(KERNEL_DS);

    printk("Opening partion\n");
    fp = filp_open(partition, O_RDWR, 0644);
    if (IS_ERR(fp)) {
        set_fs(fs);
        printk("Open file error!\n");
        return NULL;
    }

    printk("Next action: stat kmalloc\n");
    stat =(struct kstat *) kmalloc(sizeof(struct kstat), GFP_KERNEL);
    if (!stat) {
        set_fs(fs);
        printk("Kmalloc failed\n");
        return NULL;
    }

    printk("Next action: vfs_stat\n");
    vfs_stat(partition, stat);
    *size = stat->size + extra_size;
    printk("File size is %lu\n", *size);
//
//    if(*size == 0) {
//
//        printk("Check if it is separate partition\n");
//        int fd = open("/dev/sdb2", O_RDONLY);
//        off_t real_size = lseek(fd, 0, SEEK_END);
//        close(fd);
//
//        printk("Real size is %llu\n", real_size);
//
//    }

    kfree(stat);
    printk("Next action: buffer\n");
    printk("*size is %lu\n", *size);
    printk("extra is %lu\n", extra_size);
    buf = kmalloc(*size + extra_size, GFP_KERNEL);
    if (!buf) {
        set_fs(fs);
        kfree(stat);
        printk("malloc input buf error!\n");
        return NULL;
    }

    printk("Next action: kernel read\n");
    ret = (size_t)kernel_read(fp, buf, *size, &pos);
    printk("Bytes read %lu : bytes wanted to be read : %lu\n", ret, *size);

    printk("Next action: filp_close\n");
    filp_close(fp, NULL);
    printk("Next action: set_fs\n");
    set_fs(fs);
    printk("Exiting get buffer file\n");
    return buf;
#endif
}

// private
uint64_t generate_random_id(void* mapped_partition, int offset, int* mod){

    int                 generateTrials;
    partition_info*     partition_metadata;
    int64_t             map_size;
    uint64_t            newId;
    int                 foundSameId;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            id;
    int                 i;
    uint64_t            modulo;


    printk("Entering generate random id\n");

    generateTrials = 10;
    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + offset);
    map_size = partition_metadata->map_size;

    foundSameId = 0;
    while(generateTrials--) {

        get_random_bytes(&newId, sizeof(newId));
        printk("New id is %llu\n", newId);

        modulo = fast_modulo(newId, LOOKUP_MAP_SIZE_POW);
        *mod = modulo;

        current_elem_in_map = (map_node* )(partition_metadata + 1);

        lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + modulo;
        if (lookup -> cnt == 0) {
            return newId;
        }

        for(i = 0; i < map_size; i++) {

            // 0 and 1 are reserved for error codes (and give
            id = current_elem_in_map->id;
            if(id == newId || id == 1) {
                foundSameId = 1;
                printk("Matched tested id is: %llu\n", id);
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

int write_key_to_custom_file(const char* key, uint64_t key_len, uint64_t id, uint8_t type) {

    FILE *file;

    char filename[128];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);

    file = fopen(filename, "w");
    if(file == NULL) {
        return 1;
    }

    uint64_t ajdusted_len = 0;
    size_t ret = 0;

    if (type == KEY_TYPE_RSA) {
        ajdusted_len = key_len - strlen(RSA_BEGIN_LABEL) - strlen(RSA_END_LABEL) - 1;
        ret = fwrite(key + strlen(RSA_BEGIN_LABEL), sizeof(char) , ajdusted_len, file);

    } else if (type == KEY_TYPE_CUSTOM) {
        ajdusted_len = key_len;
        ret = fwrite(key, sizeof(char) , ajdusted_len, file);

    } else {
        return 1;
    }

    fclose(file);

    if(ret != ajdusted_len) {
        printk("Writing key to file failed\n");
        return 1;
    }

    return 0;
}

int delete_custom_file(uint64_t id) {
    char filename[128];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);
    return remove(filename);
}

int read_key_from_custom_file(char* key, uint64_t key_len, uint64_t id, uint8_t type) {

    FILE *file;

    char filename[128];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);

    file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }

    memset(key + key_len, 0x00, sizeof(char));
    
    uint64_t ajdusted_len = 0;
    uint64_t ret = 0;
    
    if (type == KEY_TYPE_RSA) {
        ajdusted_len = key_len - strlen(RSA_BEGIN_LABEL) - strlen(RSA_END_LABEL) - 1;

        strcpy(key, RSA_BEGIN_LABEL);
        ret = fread(key + strlen(RSA_BEGIN_LABEL), sizeof(char), ajdusted_len, file);
        strcpy(key + key_len - strlen(RSA_END_LABEL) - 1, RSA_END_LABEL);
        
    } else if (type == KEY_TYPE_CUSTOM) {

        ajdusted_len = key_len;
        ret = fread(key, sizeof(char), ajdusted_len, file);
        
    } else {
        return 1;
    }
    
   

    fclose(file);

    if(ret != ajdusted_len) {
        printk("Reading key from file failed\n");
        return 1;
    }

    return 0;
}

int init_file_if_not_defined(void) {

    uint64_t        file_size;
    void*           partitionStart;
    map_node*       map_position;
    int             i;
    size_t          ret;

#if EMULATION == 1
    FILE *file;
    int part_size = 0;
    if ((file = fopen(partition, "r")))
    {
        fseek(file, 0L, SEEK_END);
        part_size = (int)ftell(file);
        if(part_size > DEFAULT_MAP_SIZE * sizeof(map_node)) {
            fclose(file);
            return 0;
        }
    }
    if(file != NULL) {
    	fclose(file);
    }

    partition_info* partition_metadata = (partition_info* )malloc(sizeof(partition_info));
    if(!partition_metadata) {
        return 1;
    }


#else

    struct file         *fp;
    mm_segment_t        fs;
    size_t              part_size;
    struct kstat*       stat;
    partition_info*     partition_metadata;

    printk("Entering init file if not defined %s\n", partition);
    printk("Next action: getting fs\n");
    fs = get_fs();
    printk("Next action: setting fs\n");
    set_fs(KERNEL_DS);
    part_size = 0;

    fp = filp_open(partition, O_RDONLY, 0600);
    if (!IS_ERR(fp)) {

        printk("File exists");

        stat =(struct kstat *) kmalloc(sizeof(struct kstat), GFP_KERNEL);
        if (!stat) {
            set_fs(fs);
            printk("Kmalloc failed\n");
            return -1;
        }

        vfs_stat(partition, stat);
        part_size = stat->size;
        printk("Part size is %lu\n", part_size);
        kfree(stat);
        filp_close(fp, NULL);

        // if current used size is lower than minimal partition size
        // it means that at the beggining of partition there are non-partition bytes
        // partition will be added right after them
        if(part_size >= DEFAULT_MAP_SIZE * sizeof(map_node) + sizeof(partition_info)) {
            set_fs(fs);
            return 0;
        }

    }

    if(part_size == 0) {
        printk("Initialise file with partition");
        printk("Open file error! - maybe does not exist\n");

        fp = filp_open(partition, O_CREAT, 0644);
        if(IS_ERR(fp)) {
            printk("Open file for create unhandled error. Exiting\n");
            set_fs(fs);
            return 1;
        }

        printk("Save partition initialised\n");
    }

    printk("Next action: set_fs\n");
    set_fs(fs);

    printk("Partition info to be allocated\n");
    partition_metadata = (partition_info* )kmalloc(sizeof(partition_info), GFP_KERNEL);
    if(!partition_metadata) {
        printk("Allocation failed, exiting\n");
        return 1;
    }

    sema_init(&sem, 1);

#endif

    file_size = DEFAULT_MAP_SIZE * sizeof(map_node) + sizeof(partition_info) + LOOKUP_SLOTS_NUM * sizeof(lookup_slot);

#if EMULATION == 1
    partitionStart = malloc(file_size);
#else
    partitionStart = kmalloc(file_size, GFP_KERNEL);
#endif
    if(!partitionStart) {
        printk("Allocation failed\n");
        return 1;
    }

    memset(partitionStart, 0x00, file_size);
    printk("Memset ok\n");

    partition_metadata = (partition_info* )partitionStart;
    partition_metadata->magic = MAGIC;
    partition_metadata->map_size = DEFAULT_MAP_SIZE;
    partition_metadata->number_of_keys = 0;

    printk("Memcpy ok\n");

    map_position = (map_node* )((partition_info* )partitionStart + 1);
    for(i = 0; i < partition_metadata->map_size; i++) {

        map_position -> id = 0;
        map_position -> size = 0;
        map_position -> type = KEY_TYPE_CUSTOM;
        // printk("Memcpy ok\n");
        map_position = map_position + 1;
    }

    memset(map_position, 0x00, LOOKUP_SLOTS_NUM * sizeof(lookup_slot));

    print_partition(partitionStart);

    printk("Set buffered file\n");
    ret = set_buffered_file(partition, (char** )&partitionStart, file_size, 0, (int)part_size);
    printk("After set ret value is %lu\n", ret);
    if(ret != file_size) {
        printk("Set failed\n");
        return 1;
    }

    printk("Exiting init file if not defined with value 0\n");

    return 0;
}

void print_partition(const void* mapped_partition) {

    int                 magic_offset;
    partition_info*     partition_metadata;
    uint64_t            keys;
    uint64_t            offsetToAdd;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    int                 i;
    char*               tmp;

    return;

    magic_offset = get_magic_offset((void*)mapped_partition);
    partition_metadata = (partition_info* )((uint8_t* )mapped_partition);
    keys = partition_metadata->number_of_keys;
    map_size = partition_metadata->map_size;

#if EMULATION == 1
    printf("This partition has: %llu keys.\n", keys);
    printf("File content size is: %llu.\n", offsetToAdd);
#else
    printk("This partition has: %llu keys.\n", keys);
    printk("File content size is: %llu.\n", offsetToAdd);
#endif

    current_elem_in_map = (map_node* )(partition_metadata + 1);
    for(i = 0; i < 5; i++) {

#if EMULATION == 1
        tmp = (char* )malloc(current_elem_in_map->size + 1);
#else
        printk("%llu %llu %llu", current_elem_in_map->id, current_elem_in_map->offset, current_elem_in_map->size);
        tmp = (char* )kmalloc(current_elem_in_map->size + 1, GFP_KERNEL);
#endif

        if(!tmp) {
            return;
        }

        tmp[current_elem_in_map->size] = 0;

#if EMULATION == 1
        printf(" %s\n", tmp);
        free(tmp);
#else
        kfree(tmp);
#endif

        current_elem_in_map += 1;
    }

}

#if EMULATION == 1
int add_key_to_partition(const char* key, uint64_t key_len, uint64_t *id, access_rights rights, uint8_t type) {
#else
int add_key_to_partition(const char* __user key, uint64_t key_len, uint64_t __user *id, access_rights rights) {
#endif

    size_t      file_size;
    void*       partition_metadata;
    int         magic_offset;
    uint64_t    key_num;

    if (key_len > MAX_PARTITION_SIZE) {
        return -1;
    }

    printk("Entering add key node to partition\n");
    printk("Key len is %llu\n", key_len);
    if(init_file_if_not_defined() != 0) {
        return -2;
    }

    printk("Loading file to buffer\n");
    partition_metadata = get_buffered_file(partition, &file_size, 0);
    if(partition_metadata == NULL || file_size == 0) {
        return -1;
    }

    printk("File loaded to buffer with file size %lu\n", file_size);
    magic_offset = get_magic_offset(partition_metadata);
    if(magic_offset < 0) {
        return -1;
    }

    printk("Magic is %d bytes from file start\n", magic_offset);

    key_num = ((partition_info* )((uint8_t* )partition_metadata + magic_offset))->number_of_keys;
    printk("Key num is %llu\n", key_num);

    // check if max key num is reached
    if(key_num == DEFAULT_MAP_SIZE) {
#if EMULATION == 1
        free(partition_metadata);
#else
        kfree(partition_metadata);
#endif
        return -2;
    }

    if (update_metadata_when_writing(partition_metadata, key, key_len, id, rights, type) < 0) {
        return -1;
    }

    if (write_key_to_custom_file(key, key_len, *id, type) != 0) {
        return - 1;
    }

    printk("Key to partition added\n");
    printk("Saving buffer with size %lu\n", file_size);

    print_partition(partition_metadata);
    if(set_buffered_file(partition, (char** )&partition_metadata, file_size, 0, 0) != file_size) {
        return -1;
    }

    printk("Exiting add key node to partition\n");
    return 0;
}
#if EMULATION == 1
int update_metadata_when_writing(void* mapped_partition, const char* key, uint64_t key_len, uint64_t *id, access_rights proc_rights, uint8_t type) {
#else
int update_metadata_when_writing(void* mapped_partition, const char* __user key, uint64_t key_len, uint64_t __user *id, access_rights proc_rights) {
#endif

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            next_id;
    uint64_t            i;
    int                 mod;

#if EMULATION == 0
    mm_segment_t        fs;
#endif

    printk("Entering add key node to partition\n");
    // printk("Key value is: %s\n", key);
    printk("Key len is: %llu\n", key_len);

    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return -1;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);

    map_size = partition_metadata->map_size;
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    printk("Moving to first map node succeeded\n");
    i = 0;
    while(i++ < map_size && current_elem_in_map->id != 0) {
        current_elem_in_map = current_elem_in_map + 1;
    }

    printk("Id is\n");
    next_id = generate_random_id(mapped_partition, help_counter, &mod);

    lookup = ((lookup_slot* )((map_node* )(partition_metadata + 1) + DEFAULT_MAP_SIZE)) + mod;
    lookup -> cnt ++;

#if EMULATION == 1
    // memcpy is to make emulation as close to final code, which uses copy_to_user
    memcpy(id, &next_id, sizeof(next_id));
#else
    printk("Assigning NEW CHANGE id\n");
    copy_to_user(id, &next_id, sizeof(next_id));
    printk("Copied and exiting map iteration.\n");
#endif

    current_elem_in_map->id = next_id;
    current_elem_in_map->type = type;
    current_elem_in_map->size = key_len;
    current_elem_in_map->uid = proc_rights.uid;
    current_elem_in_map->gid = proc_rights.gid;

#if EMULATION == 1
    current_elem_in_map->mode = 600;
#else
    current_elem_in_map->mode = 600;
#endif

    printk("New item values:\n");
    printk("id: %llu\n", current_elem_in_map->id);
    printk("size: %llu\n", current_elem_in_map->size);
    printk("uid: %llu\n", current_elem_in_map->uid);
    printk("gid: %llu\n", current_elem_in_map->gid);

    partition_metadata->number_of_keys += 1;

    print_partition(mapped_partition);
    printk("Exiting add key Node\n");

    return help_counter;
}
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t key_len, access_rights proc_rights, uint8_t type) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            offset;
    int                 found;
    int                 i;
    access_rights       mapped_rights;
    uint64_t            allocation_size;
    uint64_t            current_id;


#if EMULATION == 0
    mm_segment_t        fs;
    char                dummy;
#endif

    printk("Entering: get key val by pp\n");

    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return -1;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    map_size = partition_metadata->map_size;

    current_elem_in_map = (map_node* )(partition_metadata + 1);
    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return -1;
    }

    found = 0;
    for(i = 0; i < map_size; i++) {
        current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked");

            mapped_rights.uid = current_elem_in_map->uid;
            mapped_rights.gid = current_elem_in_map->gid;

            if (current_elem_in_map->type != type) {
                printk("Wrong type");
                return -2;
            }

            if(!can_read(current_elem_in_map->mode, mapped_rights, proc_rights)) {
                printk("Cannot read");
                return -2;
            }

            printk("ID FOUND: %llu\n", current_id);
            printk("SIZE FOUND: %llu\n", current_elem_in_map->size);

            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    if(!found) {
        printk("Not found\n");
        return -1;
    }
    printk("Found\n");
    allocation_size = current_elem_in_map->size;

    if(key_len < allocation_size) {
        printk("Allocation size: %llu is shortened to key len of %llu\n", allocation_size, key_len);
        allocation_size = key_len;
    }

    printk("Exiting: CHANGED get key val by pp");
    return 0;
}
int get_key_size_by_partition_pointer(void* mapped_partition, uint64_t id, uint64_t* key_len, access_rights proc_rights) {

    int                 help_counter;
    partition_info*     partition_metadata;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    uint64_t            i;
    access_rights       mapped_rights;
    uint64_t            size;

    printk("Entering: get key size by pp\n");

    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return -1;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return -1;
    }

    found = 0;
    i = 0;
    while(i++ < partition_metadata->map_size) {
        if (current_elem_in_map->id == id) {
            printk("Read rights to be checked\n");

            mapped_rights.uid = current_elem_in_map->uid;
            mapped_rights.gid = current_elem_in_map->gid;
            if(!can_read(current_elem_in_map->mode, mapped_rights, proc_rights)) {
                printk("Cannot read");
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

    size = current_elem_in_map->size;
#if EMULATION == 1
    memcpy(key_len, &size, sizeof(*key_len));
#else
    printk("Copying to user key_len\n");
    copy_to_user(key_len, &size, sizeof(*key_len));
    printk("Exiting: get key size by pp\n");
#endif
    return 0;
}

int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* key_mode, access_rights proc_rights) {

    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    uint64_t            i;
    access_rights       mapped_rights;
    int                 help_counter;

#if EMULATION == 0
    mm_segment_t fs;
#endif


    printk("Entering: get key mode by pp\n");

    help_counter = get_magic_offset(mapped_partition);
    if(help_counter < 0) {
        return -1;
    }
    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return -1;
    }

    map_size = partition_metadata->map_size;

    found = 0;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked\n");
            mapped_rights.uid = current_elem_in_map->uid;
            mapped_rights.gid = current_elem_in_map->gid;
            if(!can_read(current_elem_in_map->mode, mapped_rights, proc_rights)) {
                printk("Cannot read");
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

#if EMULATION == 1
    memcpy(key_mode, &current_elem_in_map->mode, sizeof(*key_mode));
#else

    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("cpy to usr\n");
    copy_to_user(key_mode, &current_elem_in_map->mode, sizeof(*key_mode));
    printk("Copied mode %d\n", current_elem_in_map->mode);

    set_fs(fs);
#endif

    printk("Exiting: get key mode by pp\n");
    return 0;
}
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int key_mode, access_rights proc_rights) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    int                 i;
    access_rights       mapped_rights;


    printk("Entering: set key mode by pp\n");
    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return -1;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot * )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return -1;
    }

    map_size = partition_metadata->map_size;
    found = 0;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {
            found = 1;

            mapped_rights.uid = current_elem_in_map->uid;
            mapped_rights.gid = current_elem_in_map->gid;
            if(!can_write(current_elem_in_map->mode, mapped_rights, proc_rights)) {
                printk("Cannot read");
                return -2;
            }
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return -1;
    }

    current_elem_in_map->mode = key_mode;
    printk("Exiting: set key mode by pp\n");
    return help_counter;
}
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id, access_rights proc_rights) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 i;
    access_rights       mapped_rights;

    print_partition(mapped_partition);
    printk("Entering: key val by pp\n");
    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return -1;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);

    map_size = partition_metadata->map_size;
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot * )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);

    int diff = ((uint8_t* )lookup - (uint8_t* )((map_node* )partition_metadata + DEFAULT_MAP_SIZE)) / 4;

    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return -1;
    }

    for(i = 0; i < map_size; i++) {

        if(current_elem_in_map->id == id) {

            printk("Checking rights");
            mapped_rights.uid = current_elem_in_map->uid;
            mapped_rights.gid = current_elem_in_map->gid;
            if(!can_write(current_elem_in_map->mode, mapped_rights, proc_rights)) {
                printk("Cannot read");
                return -2;
            }

            current_elem_in_map->id = 0;
            partition_metadata->number_of_keys -= 1;
            lookup -> cnt --;
            printk("Exiting: key val by pp (with failure)\n");
            print_partition(mapped_partition);
            return 0;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    printk("Exiting: key val by pp (with failure)\n");
    return -1;
}

#if EMULATION == 1
int get_prv_key_by_id(const uint64_t id, char* prvKey, uint64_t key_len, access_rights proc_rights, uint8_t type) {
#else
int get_prv_key_by_id(const uint64_t id, char __user *prvKey, uint64_t key_len, access_rights proc_rights) {
#endif

    size_t          file_size;
    void*           mapped_partition;
    int             check_key_rights_ret;
    int             get_key_ret;

    printk("Entering: get prv key by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if (!mapped_partition || file_size == 0) {
        return -1;
    }

    printk("Next action: get key val by pp\n");

    print_partition(mapped_partition);
    check_key_rights_ret = get_key_by_partition_pointer(mapped_partition, id, prvKey, key_len, proc_rights, type);
    print_partition(mapped_partition);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if (check_key_rights_ret != 0) {
        return check_key_rights_ret;
    }

    get_key_ret = read_key_from_custom_file(prvKey, key_len, id, type);
    if (get_key_ret != 0) {
        return get_key_ret;
    }

    printk("Exiting: get prv key by id\n");
    return 0;
}
int get_prv_key_size_by_id(const uint64_t id, uint64_t* size, access_rights proc_rights) {

    size_t          file_size;
    void*           mapped_partition;
    int             get_key_size_ret;

    printk("Entering: get prv key size by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == 0|| file_size == 0) {
        return -1;
    }

    printk("Next action: get key size by pp\n");
    get_key_size_ret = get_key_size_by_partition_pointer(mapped_partition, id, size, proc_rights);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if(get_key_size_ret < 0 ) {
        return get_key_size_ret;
    }

    printk("Exiting: get prv key size by id\n");
    return 0;
}
int remove_private_key_by_id(uint64_t id, access_rights proc_rights) {

    partition_info*     partition_metadata;
    size_t              file_size;
    void*               mapped_partition;
    int                 remove_ret;

    printk("Entering: remove prv key by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    printk("Next action: remove key val by pp\n");
    remove_ret = remove_key_by_partition_pointer(mapped_partition, id, proc_rights);
    if(remove_ret != 0) {
        
#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

        return remove_ret;
    }

    partition_metadata = (partition_info* )mapped_partition;
    printk("Next action: set bufferd file\n");
    if (set_buffered_file(partition, (char** )&mapped_partition, file_size, 0, 0) != file_size) {
        return -1;
    }

    if (delete_custom_file(id) != 0) {
        return -1;
    }

    printk("Entering: remove prv key by id\n");
    return remove_ret;
}

// public
#if EMULATION == 1
int do_get_key_num() {
#else
SYSCALL_DEFINE0(get_key_num) {
#endif

    size_t              file_size;
    void*               mapped_partition;
    partition_info*     partition_metadata;
    uint64_t            key_num;


    printk("\n");
    printk("Entering: get current key num\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        printk("Exiting: get current key num\n");
        return -1;
    }

    partition_metadata = (partition_info* )mapped_partition;
    key_num = (partition_metadata->number_of_keys) % INT_MAX;

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    printk("Exiting: get current key num\n");
    return key_num;
}

#if EMULATION == 1
int do_write_key(const char* key, const char* password, uint64_t key_len, uint64_t* id, int uid, int gid, int type) {
#else
SYSCALL_DEFINE5(write_key, const char __user *, key, uint64_t, key_len, uint64_t __user *, id, int __user, uid, int __user, gid) {
#endif

    uint64_t            used_len;
    access_rights       proc_rights;
    int                 ret;

    printk("\n");
    printk("Entering write key\n");

    if(key_len == 0) {
        return -1;
    }

    used_len = key_len;

#if EMULATION == 1
    if(key_len > strlen(key)) {
        used_len = strlen(key);
    }
#endif

    printk("Kernel space memory successfully allocated, %llu of bytes to be copied\n", key_len);

    proc_rights.uid = uid;
    proc_rights.gid = gid;

#if EMULATION == 0
    up(&sem);
#endif

    ret = add_key_to_partition(key, used_len, id, proc_rights, type);
    if(ret == -1 || ret == -2) {
#if EMULATION == 0
        down(&sem);
#endif

        return ret;
    }

    printk("Exiting write key\n");

#if EMULATION == 0
    down(&sem);
#endif

    return 0;
}

#if EMULATION == 1
int do_read_key(const uint64_t id, const char* password, char* key, uint64_t key_len, int uid, int gid, int type) {
#else
SYSCALL_DEFINE5(read_key, const uint64_t, id, char __user *, key, uint64_t, key_len, int __user, uid, int __user, gid) {
#endif

    access_rights       proc_rights;
    int                 ret;

    printk("\n");
    printk("Entering: readKey recompiled\n");
    if(id == 0 || key_len == 0) {
       return -1;
    }

    printk("Next action: get prv key by id\n");
    proc_rights.uid = uid;
    proc_rights.gid = gid;

    ret = get_prv_key_by_id(id, key, key_len, proc_rights, type);
    if(ret != 0) {
        return ret;
    }

    printk("Exiting: readKey\n");
    return 0;
}

#if EMULATION == 1
int do_remove_key(const uint64_t id, const char* password, int uid, int gid) {
#else
SYSCALL_DEFINE3(remove_key, const uint64_t __user, id, int __user, uid, int __user, gid) {
#endif

    access_rights proc_rights;
    int           ret;
    if(id == 0) {
        return -1;
    }

    printk("Entering and soon exiting remove key\n");
    proc_rights.uid = uid;
    proc_rights.gid = gid;

#if EMULATION == 0
    up(&sem);
#endif

    ret = remove_private_key_by_id(id, proc_rights);

#if EMULATION == 0
    down(&sem);
#endif

    return ret;
}


// for kernel kuid_t instead of uid
#if EMULATION == 1
int do_get_mode(const uint64_t id, const char* password, int* output, int uid, int gid) {
#else
SYSCALL_DEFINE4(get_mode, const uint64_t __user, id, int __user *, output, int __user, uid, int __user, gid) {
#endif

    size_t              file_size;
    void*               mapped_partition;
    access_rights       proc_rights;
    int                 get_key_ret;

    printk("\n");
    printk("Entering: get mode\n");
    if(id == 0) {
        return -1;
    }

    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {
        return -1;
    }

    proc_rights.uid = uid;
    proc_rights.gid = gid;
    printk("Next action: get key mode by pp\n");
    get_key_ret = get_key_mode_by_partition_pointer(mapped_partition, id, output, proc_rights);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if(get_key_ret < 0) {
        return -1;
    }

    printk("Exiting: get mode\n");
    return 0;
}

#if EMULATION == 1
int do_set_mode(const uint64_t id, const char* password, int new_mode, int uid, int gid) {
#else
SYSCALL_DEFINE4(set_mode, const uint64_t __user, id, int, new_mode, int __user, uid, int __user, gid) {
#endif

    int             h_digit;
    int             d_digit;
    int             digit;
    size_t          file_size;
    void*           mapped_partition;
    access_rights   proc_rights;
    int             get_key_ret;

    if(new_mode > 777 || new_mode < 0) {
        return -1;
    }
    h_digit = (new_mode / 100);
    d_digit = (new_mode / 10) % 10;
    digit = new_mode % 10;

    if(!(h_digit <= 7 && d_digit <= 7 && digit <= 7)) {
        return -1;
    }

    printk("Entering: set mode\n");
    if(id == 0) {
        return -1;
    }

#if EMULATION == 0
    up(&sem);
#endif

    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL || file_size == 0) {

#if EMULATION == 0
        down(&sem);
#endif
        return -1;
    }

    proc_rights.uid = uid;
    proc_rights.gid = gid;

    printk("Next action: set key mode by pp\n");
    get_key_ret = set_key_mode_by_partition_pointer(mapped_partition, id, new_mode, proc_rights);
    if(get_key_ret == -1) {

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
    down(&sem);
#endif
        return -1;
    }

    printk("Next action: set buffered file\n");
    if(set_buffered_file(partition, (char** )&mapped_partition, file_size, 0, 0) != file_size) {

#if EMULATION == 0
        down(&sem);
#endif
        return -1;
    }

#if EMULATION == 0
    down(&sem);
#endif
    printk("Exiting: set mode\n");
    return 0;
}

int can_read(int mode, access_rights mapped, access_rights proc) {

    int         user_read;
    int         group_read;
    int         others_read;

    if(proc.uid == 0 || proc.gid == 0) {
        return 1;
    }

    user_read = (mode / 100) & READ_MASK;
    group_read = ((mode / 10) % 10) & READ_MASK;
    others_read = (mode % 10) & READ_MASK;

    return (user_read && (mapped.uid == proc.uid)) || (group_read && (mapped.gid == proc.gid)) || (others_read && (mapped.uid != proc.uid));
}
int can_write(int mode, access_rights mapped, access_rights proc) {

    int         user_write;
    int         group_write;
    int         others_write;

    if(proc.uid == 0 || proc.gid == 0) {
        return 1;
    }
    user_write = (mode / 100) & WRITE_MASK;
    group_write = ((mode / 10) % 10) & WRITE_MASK;
    others_write = (mode % 10) & WRITE_MASK;

    return (user_write && (mapped.uid == proc.uid)) || (group_write && (mapped.gid == proc.gid)) || (others_write && (mapped.uid != proc.uid));
}

#if EMULATION == 1
int do_get_key_size(const uint64_t id, const char* password, uint64_t* size, int uid, int gid) {
#else
    SYSCALL_DEFINE4(get_key_size, const uint64_t __user, id, uint64_t*, size, int __user, uid, int __user, gid) {
#endif

    access_rights       proc_rights;
    int                 ret;

    printk("\n");
    printk("Entering: get key size\n");
    if(id == 0) {
        return -1;
    }

    printk("Next action: get prv key size by id\n");

    proc_rights.uid = uid;
    proc_rights.gid = gid;

    ret = get_prv_key_size_by_id(id, size, proc_rights);
    if(ret == -1) {
        return -1;
    }

    printk("Exiting: get key size\n");
    return 0;
}
