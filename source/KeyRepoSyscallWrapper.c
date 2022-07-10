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

int get_key_num() {
    return syscall(__x64_get_key_num);
}

int write_key(const char* key, const uint64_t keyLen, uint64_t* id) {

    printf("Entering write key\n");
    uint64_t usedLen = keyLen;
    if(keyLen > strlen(key)) {
        usedLen = strlen(key);
    }

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }
	
    return syscall(__x64_write_key, key, usedLen, id, ids.uid, ids.gid);
}

int read_key(const uint64_t id, char* key, uint64_t keyLen) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_read_key, id, key, keyLen, ids.uid, ids.gid);
}

int remove_key(const uint64_t id) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_remove_key, id, ids.uid, ids.gid);
}

int get_key_size(const uint64_t id, uint64_t* size) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_get_key_size, id, size, ids.uid, ids.gid);
}

int get_mode(const uint64_t id, int* output) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return syscall(__x64_get_mode, id, output, ids.uid, ids.gid);
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

    return do_write_key(key, key_len, pass, pass_len, id, ids.uid, ids.gid, type);
}
int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len) {
    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_read_key(key, id, pass, pass_len, key_len, ids.uid, ids.gid);
}
int remove_key(const uint64_t id) {
    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_remove_key(id, ids.uid, ids.gid);
}
int get_key_size(const uint64_t id, uint64_t* size) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_get_key_size(id, size, ids.uid, ids.gid);
}
int get_mode(const uint64_t id, int* output) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_get_mode(id, output, ids.uid, ids.gid);
}
int set_mode(const uint64_t id, int new_mode) {

    original_uids ids = get_original_uids();
    if(ids.uid == -1 || ids.gid == -1) {
        return -1;
    }

    return do_set_mode(id, new_mode, ids.uid, ids.gid);
}
int get_key_num() {
    return do_get_key_num();
}

#endif
