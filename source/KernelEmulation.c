#define EMULATION 1

#if EMULATION == 1
    #include "../include/KernelEmulation.h"
    uint64_t remove_fragmentation_on = 1;
    uint64_t map_optimisation_on = 1;
#else
    #include "KeyRepoHeaders.h"
#endif

// temporary function
// changes are done to buffer in place for optimization purposes
int encrypt_data_at_rest(char* buf, size_t len, const char* pass, size_t pass_len) {

    if (buf == NULL) {
        return RES_INPUT_ERR;
    }

    if (pass == NULL) {
        return RES_INPUT_ERR;
    }

    // TODO - just a PoC
    int key = 0;
    for (uint64_t i = 0; i < pass_len; i++) {
        key += pass[i];
    }
    key = key % 30;

    for (size_t i = 0; i < len; i++) {
        buf[i] -= (char)key;
    }

    return RES_OK;
}

// temporary function
// changes are done to buffer in place for optimization purposes
int decrypt_data_at_rest(char* buf, size_t len, const char* pass, uint64_t pass_len) {

    if (buf == NULL) {
        return RES_INPUT_ERR;
    }

    if (pass == NULL) {
        return RES_INPUT_ERR;
    }

    // TODO - just a PoC
    int key = 0;
    for (uint64_t i = 0; i < pass_len; i++) {
        key += pass[i];
    }
    key = key % 30;

    for (size_t i = 0; i < len; i++) {
        buf[i] += (char)key;
    }

    return RES_OK;
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
    int                 current_offset;

    first_byte = (uint64_t* )mapped_partition;
    current_offset = 0;

    while(*first_byte != MAGIC || current_offset > MAX_PARTITION_SIZE) {

        first_byte = (uint64_t *) ((uint8_t *) first_byte + 1);
        current_offset++;
    }

    if(current_offset > MAX_PARTITION_SIZE) {
        return RES_PARTITION_FULL;
    }
    return current_offset;
}

// kernel functions signatures defined for user space
#if EMULATION == 1
    void printk(const char* dummy, ...) {
//	 printf(dummy);
    }
    void get_random_bytes(uint64_t* n, size_t size) {
        assert(size == sizeof(*n));
        uint64_t random_seq = 0;
        for (int i = 0; i < 64; i++) {

            random_seq = (random_seq << 1) + (rand() & 1);
        }
        *n = random_seq;
    }
#endif

// trunc param is used only when called from kernel
// in emulation ftrunc() function is used
size_t set_buffered_file(const char* file, char** buf, size_t bufsize, int trunc, int offset) {
#if EMULATION == 1
    FILE* fd = fopen(file, "w");
    if(fd == NULL) {
        free(buf);
        return RES_CANNOT_OPEN;
    }

    fseek(fd, offset, SEEK_SET);

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
        return RES_CANNOT_OPEN;
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

        // intended, fread call needed
        size_t new_len = fread(source, sizeof(char), bufsize, fp);
        if (ferror(fp) != 0 || new_len != bufsize) {
            fclose(fp);
            return NULL;
        }
        
    }

    fclose(fp);
    if (bufsize == 0) {
        return NULL;
    }

    *size = bufsize;
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
uint64_t generate_random_id(partition_info* partition_metadata, int* mod){

    int                 generateTrials;
    int64_t             map_size;
    int64_t             number_of_keys;
    int64_t             keys_checked;
    uint64_t            new_id;
    int                 foundSameId;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            id;
    int                 i;
    uint64_t            modulo;


    printk("Entering generate random id\n");

    generateTrials = 10;
    map_size = partition_metadata->capacity;
    number_of_keys = partition_metadata->number_of_keys;
    keys_checked = 0;

    foundSameId = 0;
    while(generateTrials--) {

        get_random_bytes(&new_id, sizeof(new_id));
        printk("New id is %llu\n", new_id);

        modulo = fast_modulo(new_id, LOOKUP_MAP_SIZE_POW);
        *mod = modulo;

        current_elem_in_map = (map_node* )(partition_metadata + 1);

        lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + modulo;
        if (lookup -> cnt == 0) {
            return new_id;
        }

        for(i = 0; i < map_size; i++) {

            // 0 and 1 are reserved for error codes (and give
            id = current_elem_in_map->id;
            if(id == new_id || id == 1) {
                foundSameId = 1;
                printk("Matched tested id is: %llu\n", id);
                break;
            }

            if (id != 0 && id != 1) {
                keys_checked++;
            }

            // there is no need to iterate though map if we checked all keys
            if (keys_checked == number_of_keys) {
                break;
            }

            current_elem_in_map++;
        }

        if(foundSameId == 0) {
            break;
        }

        printk("NEXT TRIAL!\n");
    }

    return new_id;
}

int write_key_to_custom_file(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t id, uint8_t type) {

    FILE *file;

    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);

    file = fopen(filename, "w");
    if(file == NULL) {
        return RES_CANNOT_OPEN;
    }

    uint64_t adjusted_len = 0;
    size_t ret = 0;

    char* key_to_encrypt = (char* )malloc(key_len);
    if (key_to_encrypt == NULL) {
        return RES_CANNOT_ALLOC;
    }

    if (type == KEY_TYPE_RSA) {

        memcpy(key_to_encrypt, key, key_len);
        encrypt_data_at_rest(key_to_encrypt, key_len, pass, pass_len);
        adjusted_len = key_len - strlen(RSA_BEGIN_LABEL) - strlen(RSA_END_LABEL) - 1;
        ret = fwrite(key_to_encrypt + strlen(RSA_BEGIN_LABEL), sizeof(char) , adjusted_len, file);

    } else if (type == KEY_TYPE_CUSTOM) {

        memcpy(key_to_encrypt, key, key_len);
        encrypt_data_at_rest(key_to_encrypt, key_len, pass, pass_len);
        adjusted_len = key_len;
        ret = fwrite(key_to_encrypt, sizeof(char) , adjusted_len, file);

    } else {
        return RES_NO_KEY_TYPE;
    }

    fclose(file);

    if(ret != adjusted_len) {
        printk("Writing key to file failed\n");
        return RES_CANNOT_WRITE;
    }

    return RES_OK;
}

int delete_custom_file(uint64_t id) {
    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);
    return remove(filename);
}

int read_key_from_custom_file(char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t id, uint8_t type) {

    FILE *file;

    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s%"PRIu64, partition_base, id);

    file = fopen(filename, "r");
    if(file == NULL) {
        return RES_CANNOT_OPEN;
    }

    memset(key + key_len, 0x00, sizeof(char));
    
    uint64_t adjusted_len = 0;
    uint64_t ret = 0;
    
    if (type == KEY_TYPE_RSA) {
        adjusted_len = key_len - strlen(RSA_BEGIN_LABEL) - strlen(RSA_END_LABEL) - 1;

        strcpy(key, RSA_BEGIN_LABEL);
        ret = fread(key + strlen(RSA_BEGIN_LABEL), sizeof(char), adjusted_len, file);
        decrypt_data_at_rest(key + strlen(RSA_BEGIN_LABEL), adjusted_len, pass, pass_len);
        strcpy(key + key_len - strlen(RSA_END_LABEL) - 1, RSA_END_LABEL);
        
    } else if (type == KEY_TYPE_CUSTOM) {

        adjusted_len = key_len;
        ret = fread(key, sizeof(char), adjusted_len, file);
        decrypt_data_at_rest(key, adjusted_len, pass, pass_len);
        
    } else {
        return RES_NO_KEY_TYPE;
    }

    fclose(file);

    if(ret != adjusted_len) {
        printk("Reading key from file failed\n");
        return RES_CANNOT_OPEN;
    }

    return RES_OK;
}

int is_repo_initialized() {

    FILE *file;
    int part_size = 0;
    if ((file = fopen(partition, "r")))
    {
        fseek(file, 0L, SEEK_END);
        part_size = (int)ftell(file);
        if(part_size > DEFAULT_MAP_SIZE * sizeof(map_node)) {
            fclose(file);
            return 1;
        }
    }

    if(file != NULL) {
        fclose(file);
    }

    return 0;
}

int init_file_if_not_defined(void) {

    uint64_t        file_size;
    void*           partition_start;
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
            return RES_OK;
        }
    }
    if(file != NULL) {
    	fclose(file);
    }

    partition_info* partition_metadata = (partition_info* )malloc(sizeof(partition_info));
    if(!partition_metadata) {
        return RES_CANNOT_ALLOC;
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
            return RES_OK;
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
    partition_start = malloc(file_size);
#else
    partition_start = kmalloc(file_size, GFP_KERNEL);
#endif
    if(!partition_start) {
        printk("Allocation failed\n");
        return RES_CANNOT_ALLOC;
    }

    memset(partition_start, 0x00, file_size);
    printk("Memset ok\n");

    partition_metadata = (partition_info* )partition_start;
    partition_metadata->magic = MAGIC;
    partition_metadata->capacity = DEFAULT_MAP_SIZE;
    partition_metadata->number_of_keys = 0;
    partition_metadata->freed_slot = -1;

    map_position = (map_node* )((partition_info* )partition_start + 1);
    for(i = 0; i < partition_metadata->capacity; i++) {

        map_position -> id = 0;
        map_position -> key_info.size = 0;
        map_position -> key_info.type = KEY_TYPE_CUSTOM;
        map_position = map_position + 1;
    }

    memset(map_position, 0x00, LOOKUP_SLOTS_NUM * sizeof(lookup_slot));

    print_partition(partition_start);

    printk("Set buffered file\n");
    ret = set_buffered_file(partition, (char** )&partition_start, file_size, 0, (int)part_size);
    printk("After set ret value is %lu\n", ret);
    if(ret != file_size) {
        printk("Set failed\n");
        return RES_CANNOT_WRITE;
    }

    printk("Exiting init file if not defined with value 0\n");

    return RES_OK;
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
    map_size = partition_metadata->capacity;

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
        tmp = (char* )malloc(current_elem_in_map->key_info.size + 1);
#else
        printk("%llu %llu %llu", current_elem_in_map->id, current_elem_in_map->offset, current_elem_in_map->size);
        tmp = (char* )kmalloc(current_elem_in_map->size + 1, GFP_KERNEL);
#endif

        if(!tmp) {
            return;
        }

        tmp[current_elem_in_map->key_info.size] = 0;

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
int add_key_to_partition(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t *id, user_info rights, uint8_t type) {
#else
int add_key_to_partition(const char* __user key, uint64_t key_len, uint64_t __user *id, user_info rights) {
#endif

    size_t              file_size;
    void*               partition_metadata;
    int                 magic_offset;
    int                 ret;
    partition_info*     partition_start;

    if (key_len > MAX_PARTITION_SIZE) {
        return RES_PARTITION_FULL;
    }

    printk("Entering add key node to partition\n");
    printk("Key len is %llu\n", key_len);
    if(init_file_if_not_defined() != 0) {
        return RES_CANNOT_INIT;
    }

    printk("Loading file to buffer\n");
    partition_metadata = get_buffered_file(partition, &file_size, 0);
    if(partition_metadata == NULL) {
        return RES_CANNOT_OPEN;
    }

    printk("File loaded to buffer with file size %lu\n", file_size);
    magic_offset = get_magic_offset(partition_metadata);
    if(magic_offset < 0) {
        return RES_NON_INTEGRAL;
    }

    printk("Magic is %d bytes from file start\n", magic_offset);

    partition_start = (partition_info* )((uint8_t* )partition_metadata + magic_offset);
    printk("Key num is %llu\n", partition_start->number_of_keys);

    // check if max key num is reached
    if(partition_start->number_of_keys == DEFAULT_MAP_SIZE) {
#if EMULATION == 1
        free(partition_metadata);
#else
        kfree(partition_metadata);
#endif
        return RES_PARTITION_FULL;
    }

    if (update_metadata_when_writing(partition_start, key, key_len, id, rights, type) < 0) {
        return RES_CANNOT_WRITE;
    }

    ret = write_key_to_custom_file(key, key_len, pass, pass_len, *id, type);
    if (ret < 0) {
        return ret;
    }

    printk("Key to partition added\n");
    printk("Saving buffer with size %lu\n", file_size);

    print_partition(partition_metadata);
    // we overwrite partition only if writing key succeeded
    if(set_buffered_file(partition, (char** )&partition_metadata, file_size, 0, 0) != file_size) {
        return RES_CANNOT_WRITE;
    }

    printk("Exiting add key node to partition\n");
    return RES_OK;
}

int get_append_slot_if_possible(map_node* map_start, partition_info* partition_metadata, map_node** slot_to_append) {

    uint16_t            slot_after_last_key_offset;
    map_node*           slot_after_last_key;

    slot_after_last_key_offset = partition_metadata->number_of_keys;
    if (slot_after_last_key_offset > partition_metadata->capacity) {
        return 0;
    }

    slot_after_last_key = map_start + slot_after_last_key_offset;
    if (slot_after_last_key->id == 0) {
        *slot_to_append = slot_after_last_key;
        return 1;
    }

    return 0;
}

#if EMULATION == 1
int update_metadata_when_writing(partition_info * partition_metadata, const char* key, uint64_t key_len, uint64_t *id, user_info effective_user_info, uint8_t type) {
#else
int update_metadata_when_writing(void* mapped_partition, const char* __user key, uint64_t key_len, uint64_t __user *id, user_info user_info) {
#endif

    int                 help_counter;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    map_node*           back_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            next_id;
    uint64_t            i;
    int                 mod;

#if EMULATION == 0
    mm_segment_t        fs;
    mm_segment_t        fs;
#endif

    printk("Entering add key node to partition\n");
    // printk("Key value is: %s\n", key);
    printk("Key len is: %llu\n", key_len);

    map_size = partition_metadata->capacity;
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    printk("Moving to first map node succeeded\n");

    // if previously there was something removed at freed_slot
    if (partition_metadata->freed_slot != -1) {
        current_elem_in_map += partition_metadata->freed_slot;
        partition_metadata->freed_slot = -1;

    } else if (get_append_slot_if_possible(current_elem_in_map, partition_metadata, &back_elem_in_map)) {
        current_elem_in_map = back_elem_in_map;

    } else {
        i = 0;
        while(i < map_size && current_elem_in_map->id != 0) {
            current_elem_in_map = current_elem_in_map + 1;
            i++;
        }
    }

    printk("Id is\n");
    next_id = generate_random_id(partition_metadata, &mod);

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

    key_info key_info;
    key_info.type = type;
    key_info.size = key_len;
    key_info.owner_info = effective_user_info;

    current_elem_in_map->id = next_id;
    current_elem_in_map->key_info = key_info;

#if EMULATION == 1
    current_elem_in_map->key_info.access_control_list = 600;
#else
    current_elem_in_map->key_info.access_control_list = 600;
#endif

    printk("New item values:\n");
    printk("id: %llu\n", current_elem_in_map->id);
    printk("size: %llu\n", current_elem_in_map->key_info.size);
    printk("uid: %llu\n", current_elem_in_map->key_info.owner_info.uid);
    printk("gid: %llu\n", current_elem_in_map->key_info.owner_info.gid);

    partition_metadata->number_of_keys += 1;

    print_partition(partition_metadata);
    printk("Exiting add key Node\n");

    return help_counter;
}
int get_key_by_partition_pointer(void* mapped_partition, uint64_t id, char* keyVal, uint64_t key_len, user_info effective_user_info, uint8_t* type) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    uint64_t            offset;
    int                 found;
    int                 i;
    user_info           owner_info;
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
        return RES_NON_INTEGRAL;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    map_size = partition_metadata->capacity;

    current_elem_in_map = (map_node* )(partition_metadata + 1);
    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return RES_NOT_FOUND;
    }

    found = 0;
    for(i = 0; i < map_size; i++) {
        current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked");

            owner_info = current_elem_in_map->key_info.owner_info;
            *type = current_elem_in_map->key_info.type;

            if(!can_read(current_elem_in_map->key_info.access_control_list, owner_info, effective_user_info)) {
                printk("Cannot read");
                return RES_UNAUTHORIZED;
            }

            printk("ID FOUND: %llu\n", current_id);
            printk("SIZE FOUND: %llu\n", current_elem_in_map->key_info.size);

            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    if(!found) {
        printk("Not found\n");
        return RES_NOT_FOUND;
    }
    printk("Found\n");
    allocation_size = current_elem_in_map->key_info.size;

    if(key_len < allocation_size) {
        printk("Allocation size: %llu is shortened to key len of %llu\n", allocation_size, key_len);
        allocation_size = key_len;
    }

    printk("Exiting: CHANGED get key val by pp");
    return RES_OK;
}
int get_key_size_by_partition_pointer(void* mapped_partition, uint64_t id, uint64_t* key_len, user_info effective_user_info) {

    int                 help_counter;
    partition_info*     partition_metadata;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    uint64_t            i;
    user_info           owner_info;
    uint64_t            size;

    printk("Entering: get key size by pp\n");

    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return RES_NON_INTEGRAL;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return RES_NOT_FOUND;
    }

    found = 0;
    i = 0;
    while(i++ < partition_metadata->capacity) {
        if (current_elem_in_map->id == id) {
            printk("Read rights to be checked\n");

            owner_info = current_elem_in_map->key_info.owner_info;
            if(!can_read(current_elem_in_map->key_info.access_control_list, owner_info, effective_user_info)) {
                printk("Cannot read");
                return RES_UNAUTHORIZED;
            }

            found = 1;
            break;
        }

        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return RES_NOT_FOUND;
    }

    size = current_elem_in_map->key_info.size;
#if EMULATION == 1
    memcpy(key_len, &size, sizeof(*key_len));
#else
    printk("Copying to user key_len\n");
    copy_to_user(key_len, &size, sizeof(*key_len));
    printk("Exiting: get key size by pp\n");
#endif
    return RES_OK;
}

int get_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int* key_mode, user_info effective_user_info) {

    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    uint64_t            i;
    user_info           owner_info;
    int                 help_counter;

#if EMULATION == 0
    mm_segment_t fs;
#endif


    printk("Entering: get key mode by pp\n");

    help_counter = get_magic_offset(mapped_partition);
    if(help_counter < 0) {
        return RES_NON_INTEGRAL;
    }
    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot* )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return RES_NOT_FOUND;
    }

    map_size = partition_metadata->capacity;

    found = 0;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {

            printk("Read rights to be checked\n");
            owner_info = current_elem_in_map->key_info.owner_info;
            if(!can_read(current_elem_in_map->key_info.access_control_list, owner_info, effective_user_info)) {
                printk("Cannot read");
                return RES_UNAUTHORIZED;
            }

            found = 1;
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return RES_NOT_FOUND;
    }

#if EMULATION == 1
    memcpy(key_mode, &current_elem_in_map->key_info.access_control_list, sizeof(*key_mode));
#else

    printk("Next action: get fs\n");
    fs = get_fs();
    printk("Next action: set fs\n");
    set_fs(KERNEL_DS);

    printk("cpy to usr\n");
    copy_to_user(key_mode, &current_elem_in_map->key_info.access_control_list, sizeof(*key_mode));
    printk("Copied mode %d\n", current_elem_in_map->key_info.access_control_list);

    set_fs(fs);
#endif

    printk("Exiting: get key mode by pp\n");
    return RES_OK;
}
int set_key_mode_by_partition_pointer(void* mapped_partition, uint64_t id, int key_mode, user_info effective_user_info) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 found;
    int                 i;
    user_info           owner_info;


    printk("Entering: set key mode by pp\n");
    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return RES_NON_INTEGRAL;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot * )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return RES_NOT_FOUND;
    }

    map_size = partition_metadata->capacity;
    found = 0;
    for(i = 0; i < map_size; i++) {
        uint64_t current_id = current_elem_in_map->id;
        if(current_id == id) {
            found = 1;

            owner_info = current_elem_in_map->key_info.owner_info;
            if(!can_write(current_elem_in_map->key_info.access_control_list, owner_info, effective_user_info)) {
                printk("Cannot read");
                return RES_UNAUTHORIZED;
            }
            break;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }

    if(!found) {
        return RES_NOT_FOUND;
    }

    current_elem_in_map->key_info.access_control_list = key_mode;
    printk("Exiting: set key mode by pp\n");
    return help_counter;
}
int remove_key_by_partition_pointer(void* mapped_partition, uint64_t id, user_info effective_user_info) {

    int                 help_counter;
    partition_info*     partition_metadata;
    uint64_t            map_size;
    map_node*           current_elem_in_map;
    lookup_slot*        lookup;
    int                 i;
    user_info           owner_info;

    print_partition(mapped_partition);
    printk("Entering: key val by pp\n");
    help_counter = get_magic_offset(mapped_partition);
    printk("Help counter is: %d\n", help_counter);
    if(help_counter < 0) {
        return RES_NON_INTEGRAL;
    }

    partition_metadata = (partition_info* )((uint8_t* )mapped_partition + help_counter);

    map_size = partition_metadata->capacity;
    current_elem_in_map = (map_node* )(partition_metadata + 1);

    lookup = ((lookup_slot * )(current_elem_in_map + DEFAULT_MAP_SIZE)) + fast_modulo(id, LOOKUP_MAP_SIZE_POW);
    if (lookup -> cnt == 0) {
        printk("Not found by cached info\n");
        return RES_NOT_FOUND;
    }

    for(i = 0; i < map_size; i++) {

        if(current_elem_in_map->id == id) {

            printk("Checking rights");
            owner_info = current_elem_in_map->key_info.owner_info;
            if(!can_write(current_elem_in_map->key_info.access_control_list, owner_info, effective_user_info)) {
                printk("Cannot read");
                return RES_UNAUTHORIZED;
            }

            current_elem_in_map->id = 0;
            memset(current_elem_in_map, 0x00, sizeof(*current_elem_in_map));
            partition_metadata->number_of_keys -= 1;
            partition_metadata->freed_slot = i;
            lookup -> cnt --;
            printk("Exiting: key val by pp (with failure)\n");
            print_partition(mapped_partition);
            return RES_OK;
        }
        current_elem_in_map = current_elem_in_map + 1;
    }
    printk("Exiting: key val by pp (with failure)\n");
    return RES_NOT_FOUND;
}

#if EMULATION == 1
int get_prv_key_by_id(const uint64_t id, char* prv_key, uint64_t key_len, const char* pass, uint64_t pass_len, user_info proc_rights) {
#else
int get_prv_key_by_id(const uint64_t id, char __user *prv_key, uint64_t key_len, user_info proc_rights) {
#endif

    size_t          file_size;
    void*           mapped_partition;
    int             check_key_rights_ret;
    int             get_key_ret;

    printk("Entering: get prv key by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if (!mapped_partition) {
        return RES_CANNOT_OPEN;
    }

    printk("Next action: get key val by pp\n");
    print_partition(mapped_partition);

    uint8_t type;
    check_key_rights_ret = get_key_by_partition_pointer(mapped_partition, id, prv_key, key_len, proc_rights, &type);
    print_partition(mapped_partition);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if (check_key_rights_ret != 0) {
        return check_key_rights_ret;
    }

    get_key_ret = read_key_from_custom_file(prv_key, key_len, pass, pass_len, id, type);
    if (get_key_ret != 0) {
        return get_key_ret;
    }

    printk("Exiting: get prv key by id\n");
    return RES_OK;
}
int get_prv_key_size_by_id(const uint64_t id, uint64_t* size, user_info proc_rights) {

    size_t          file_size;
    void*           mapped_partition;
    int             get_key_size_ret;

    printk("Entering: get prv key size by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL) {
        return RES_CANNOT_OPEN;
    }

    printk("Next action: get key size by pp\n");
    get_key_size_ret = get_key_size_by_partition_pointer(mapped_partition, id, size, proc_rights);

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
#endif

    if(get_key_size_ret < 0) {
        return get_key_size_ret;
    }

    printk("Exiting: get prv key size by id\n");
    return RES_OK;
}
int remove_private_key_by_id(uint64_t id, user_info proc_rights) {

    partition_info*     partition_metadata;
    size_t              file_size;
    void*               mapped_partition;
    int                 remove_ret;

    printk("Entering: remove prv key by id\n");
    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL) {
        return RES_CANNOT_OPEN;
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
        return RES_CANNOT_WRITE;
    }

    if (delete_custom_file(id) != 0) {
        return RES_CANNOT_DELETE;
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

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
    }

    printk("Entering: get current key num\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL) {
        printk("Exiting: get current key num\n");
        return RES_CANNOT_OPEN;
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
int do_write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len , uint64_t* id, int uid, int gid, int type) {
#else
SYSCALL_DEFINE5(write_key, const char __user *, key, uint64_t, key_len, uint64_t __user *, id, int __user, uid, int __user, gid) {
#endif

    uint64_t            used_key_len;
    uint64_t            used_pass_len;
    user_info           proc_rights;
    int                 ret;

    printk("\n");
    printk("Entering write key\n");

    if (type != KEY_TYPE_RSA && type != KEY_TYPE_CUSTOM) {
        return RES_INPUT_ERR;
    }

    if (key == NULL || pass == NULL) {
        return RES_INPUT_ERR;
    }

    if(key_len == 0 || pass_len == 0) {
        return RES_INPUT_ERR;
    }

    used_key_len = key_len;
    used_pass_len = pass_len;

#if EMULATION == 1
    if(key_len > strlen(key)) {
        used_key_len = strlen(key);
    }

    if(pass_len > strlen(pass)) {
        used_pass_len = strlen(pass);
    }
#endif

    printk("Kernel space memory successfully allocated, %llu of bytes to be copied\n", key_len);

    proc_rights.uid = uid;
    proc_rights.gid = gid;

#if EMULATION == 0
    up(&sem);
#endif

    ret = add_key_to_partition(key, used_key_len, pass, used_pass_len, id, proc_rights, type);
    if(ret < 0) {
#if EMULATION == 0
        down(&sem);
#endif

        return ret;
    }

    printk("Exiting write key\n");

#if EMULATION == 0
    down(&sem);
#endif

    return RES_OK;
}

#if EMULATION == 1
int do_read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len, int uid, int gid) {
#else
SYSCALL_DEFINE5(read_key, const uint64_t, id, char __user *, key, uint64_t, key_len, int __user, uid, int __user, gid) {
#endif

    user_info           proc_rights;
    int                 ret;
    uint64_t            used_pass_len;

    printk("Entering: readKey recompiled\n");
    if(id == 0 || key_len == 0 || pass_len == 0) {
       return RES_INPUT_ERR;
    }

    if(pass == NULL || key == NULL) {
        return RES_INPUT_ERR;
    }

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
    }

    used_pass_len = pass_len;
#if EMULATION == 1
    if(pass_len > strlen(pass)) {
        used_pass_len = strlen(pass);
    }
#endif

    printk("Next action: get prv key by id\n");
    proc_rights.uid = uid;
    proc_rights.gid = gid;

    ret = get_prv_key_by_id(id, key, key_len, pass, used_pass_len, proc_rights);
    if(ret != 0) {
        return ret;
    }

    printk("Exiting: readKey\n");
    return RES_OK;
}

#if EMULATION == 1
int do_remove_key(const uint64_t id, int uid, int gid) {
#else
SYSCALL_DEFINE3(remove_key, const uint64_t __user, id, int __user, uid, int __user, gid) {
#endif

    user_info proc_rights;
    int           ret;

    if(id == 0) {
        return RES_INPUT_ERR;
    }

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
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
int do_get_mode(uint64_t id, int* output, int uid, int gid) {
#else
SYSCALL_DEFINE4(get_mode, const uint64_t __user, id, int __user *, output, int __user, uid, int __user, gid) {
#endif

    size_t              file_size;
    void*               mapped_partition;
    user_info       proc_rights;
    int                 get_key_ret;

    printk("\n");
    printk("Entering: get mode\n");
    if(id == 0) {
        return RES_INPUT_ERR;
    }

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
    }

    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL) {
        return RES_CANNOT_OPEN;
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
        return get_key_ret;
    }

    printk("Exiting: get mode\n");
    return RES_OK;
}

#if EMULATION == 1
int do_set_mode(uint64_t id, int new_mode, int uid, int gid) {
#else
SYSCALL_DEFINE4(set_mode, const uint64_t __user, id, int, new_mode, int __user, uid, int __user, gid) {
#endif

    int             h_digit;
    int             d_digit;
    int             digit;
    size_t          file_size;
    void*           mapped_partition;
    user_info   proc_rights;
    int             get_key_ret;

    if(new_mode > 777 || new_mode < 0) {
        return RES_INPUT_ERR;
    }
    h_digit = (new_mode / 100);
    d_digit = (new_mode / 10) % 10;
    digit = new_mode % 10;

    if(!(h_digit <= 7 && d_digit <= 7 && digit <= 7)) {
        return RES_INPUT_ERR;
    }

    printk("Entering: set mode\n");
    if(id == 0) {
        return RES_INPUT_ERR;
    }

#if EMULATION == 0
    up(&sem);
#endif

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
    }

    printk("Next action: get buffered file\n");
    mapped_partition = get_buffered_file(partition, &file_size, 0);
    if(mapped_partition == NULL) {

#if EMULATION == 0
        down(&sem);
#endif
        return RES_CANNOT_OPEN;
    }

    proc_rights.uid = uid;
    proc_rights.gid = gid;

    printk("Next action: set key mode by pp\n");
    get_key_ret = set_key_mode_by_partition_pointer(mapped_partition, id, new_mode, proc_rights);
    if(get_key_ret < 0) {

#if EMULATION == 1
    free(mapped_partition);
#else
    kfree(mapped_partition);
    down(&sem);
#endif
        return get_key_ret;
    }

    printk("Next action: set buffered file\n");
    if(set_buffered_file(partition, (char** )&mapped_partition, file_size, 0, 0) != file_size) {

#if EMULATION == 0
        down(&sem);
#endif
        return RES_CANNOT_WRITE;
    }

#if EMULATION == 0
    down(&sem);
#endif
    printk("Exiting: set mode\n");
    return RES_OK;
}

int can_read(int mode, user_info mapped, user_info proc) {

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
int can_write(int mode, user_info mapped, user_info proc) {

    int         user_write;
    int         group_write;
    int         others_write;

    if(proc.uid == 0 || proc.gid == 0) {
        return RES_INPUT_ERR;
    }
    user_write = (mode / 100) & WRITE_MASK;
    group_write = ((mode / 10) % 10) & WRITE_MASK;
    others_write = (mode % 10) & WRITE_MASK;

    return (user_write && (mapped.uid == proc.uid)) || (group_write && (mapped.gid == proc.gid)) || (others_write && (mapped.uid != proc.uid));
}

#if EMULATION == 1
int do_get_key_size(uint64_t id, uint64_t* size, int uid, int gid) {
#else
    SYSCALL_DEFINE4(get_key_size, const uint64_t __user, id, uint64_t*, size, int __user, uid, int __user, gid) {
#endif

    user_info       proc_rights;
    int                 ret;

    printk("\n");
    printk("Entering: get key size\n");
    if(id == 0) {
        return RES_INPUT_ERR;
    }

    if (!is_repo_initialized()) {
        return RES_NOT_FOUND;
    }

    printk("Next action: get prv key size by id\n");

    proc_rights.uid = uid;
    proc_rights.gid = gid;

    ret = get_prv_key_size_by_id(id, size, proc_rights);
    if(ret < 0) {
        return ret;
    }

    printk("Exiting: get key size\n");
    return RES_OK;
}
