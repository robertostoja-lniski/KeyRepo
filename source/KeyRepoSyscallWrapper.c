//
// Created by Robert Ostoja-Lniski on 1/7/21.
//
#include "../include/KeyRepoSyscallWrapper.h"

// private structures and functions
struct original_uids {
    int uid;
    int gid;
};
typedef struct original_uids original_uids;

original_uids get_original_uids() {

    const char* uid;
    const char* gid;
    int num_uid = -1;
    int num_gid = -1;

    uid = getenv("SUDO_UID");

#if __APPLE__
    if(uid == NULL) {
        num_uid = (int)geteuid();
    }

    gid = getenv("SUDO_GID");
    if(gid == NULL) {
        num_gid = (int)getegid();
    }
#else
    if(uid != NULL) {
        num_uid = (int)geteuid();
    }

    gid = getenv("SUDO_GID");
    if(gid != NULL) {
        num_gid = (int)getegid();
    }
#endif


    original_uids ret;
    ret.uid = num_uid;
    ret.gid = num_gid;
    return ret;

}

#if EMULATION == 0

int get_key_num(uint64_t* key_num) {
    return syscall(__x64_get_key_num, key_num);
}

int write_key_uid(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type, int uid, int gid) {

    metadata metadata;
    metadata.user_info.uid = uid;
    metadata.user_info.gid = gid;
    metadata.type = type;

    return syscall(__x64_write_key, key, usedLen, id, (void* )&metadata);
}

int write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type) {

    printf("Entering write key\n");
    uint64_t usedLen = key_len;
    if(key_len > strnlen(key, 4096 * 4)) {
        usedLen = strnlen(key, 4096 * 4);
    }

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    metadata metadata;
    metadata.user_info.uid = ids.uid;
    metadata.user_info.gid = ids.gid;
    metadata.type = type;
	
    return syscall(__x64_write_key, key, usedLen, id, (void* )&metadata);
}

int read_key_uid(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len, int uid, int gid) {

    user_info user_info;
    user_info.uid = ids.uid;
    user_info.gid = ids.gid;

    return syscall(__x64_read_key, id, key, key_len, (void* )&user_info);
}

int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    user_info user_info;
    user_info.uid = ids.uid;
    user_info.gid = ids.gid;

    return syscall(__x64_read_key, id, key, key_len, (void* )&user_info);
}

int remove_key_uid(uint64_t id, int uid, int gid) {
    return syscall(__x64_remove_key, id, ids.uid, ids.gid);
}

int remove_key(const uint64_t id) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_remove_key, id, ids.uid, ids.gid);
}

int get_key_size_uid(const uint64_t id, uint64_t* size, int uid, int gid) {
    return syscall(__x64_get_key_size, id, size, ids.uid, ids.gid);
}

int get_key_size(const uint64_t id, uint64_t* size) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_get_key_size, id, size, ids.uid, ids.gid);
}

int get_mode_uid(const uint64_t id, int* output, int uid, int gid) {
    return syscall(__x64_get_mode, id, output, ids.uid, ids.gid);
}

int get_mode(const uint64_t id, int* output) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_get_mode, id, output, ids.uid, ids.gid);
}

int set_mode_uid(const uint64_t id, int new_mode, int uid, int gid) {
    return syscall(__x64_set_mode, id, new_mode, ids.uid, ids.gid);
}

int set_mode(const uint64_t id, int new_mode) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_set_mode, id, new_mode, ids.uid, ids.gid);
}
#else

#include "../include/KernelEmulation.h"

int write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    metadata metadata;
    metadata.user_info.uid = ids.uid;
    metadata.user_info.gid = ids.gid;
    metadata.type = type;

    return do_write_key(key, key_len, pass, pass_len, id, (void* )&metadata);
}

int write_key_uid(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type, int uid, int gid) {

    metadata metadata;
    metadata.user_info.uid = uid;
    metadata.user_info.gid = gid;
    metadata.type = type;

    return do_write_key(key, key_len, pass, pass_len, id, (void* )&metadata);
}

int read_key_uid(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len, int uid, int gid) {

    user_info user_info;
    user_info.uid = uid;
    user_info.gid = gid;

    return do_read_key(key, id, pass, pass_len, key_len, (void* )&user_info);
}

int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len) {
    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    user_info user_info;
    user_info.uid = ids.uid;
    user_info.gid = ids.gid;

    return do_read_key(key, id, pass, pass_len, key_len, (void* )&user_info);
}

int remove_key_uid(uint64_t id, int uid, int gid) {
    return do_remove_key(id, uid, gid);
}

int remove_key(uint64_t id) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_remove_key(id, ids.uid, ids.gid);
}

int get_key_size_uid(const uint64_t id, uint64_t* size, int uid, int gid) {
    return do_get_key_size(id, size, uid, gid);
}

int get_key_size(const uint64_t id, uint64_t* size) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_get_key_size(id, size, ids.uid, ids.gid);
}

int get_mode_uid(const uint64_t id, int* output, int uid, int gid) {
    return do_get_mode(id, output, uid, gid);
}

int get_mode(const uint64_t id, int* output) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_get_mode(id, output, ids.uid, ids.gid);
}

int set_mode_uid(const uint64_t id, int new_mode, int uid, int gid) {
    return do_set_mode(id, new_mode, uid, gid);
}

int set_mode(const uint64_t id, int new_mode) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_set_mode(id, new_mode, ids.uid, ids.gid);
}

int get_key_num(uint64_t* key_num) {
    return do_get_key_num(key_num);
}

#endif
