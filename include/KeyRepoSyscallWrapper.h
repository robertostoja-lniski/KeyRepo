//
// Created by Robert Ostoja-Lniski on 1/2/21.
//

#ifndef KEYREPO_KEYREPOSYSCALLWRAPPER_H
#define KEYREPO_KEYREPOSYSCALLWRAPPER_H

#define EMULATION 1
#if EMULATION == 0

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

#define FILE_PATH_LEN 128

#define __x64_get_key_num   436
#define __x64_write_key     437
#define __x64_read_key      438
#define __x64_remove_key    439
#define __x64_get_mode      440
#define __x64_set_mode      441
#define __x64_get_key_size  442

struct user_info {
    int uid;
    int gid;
};
typedef struct user_info user_info;

struct metadata {
    user_info user_info;
    int type;
};

typedef struct metadata metadata;

int write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type);
int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len);
int remove_key(uint64_t id);
int get_key_size(uint64_t id, uint64_t* size);
int get_mode(uint64_t id, int* modes);
int set_mode(uint64_t id, int new_mode);

int write_key_uid(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type, int uid, int gid);
int read_key_uid(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len, int uid, int gid);
int remove_key_uid(uint64_t id, int uid, int gid);
int get_key_size_uid(uint64_t id, uint64_t* size, int uid, int gid);
int get_mode_uid(uint64_t id, int* modes, int uid, int gid);
int set_mode_uid(uint64_t id, int new_mode, int uid, int gid);

// password not needed, because it requires only root priviledges
int get_key_num(uint64_t* key_num);

#else

#include "KernelEmulation.h"

int write_key_uid(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type, int uid, int gid);
int read_key_uid(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len, int uid, int gid);
int remove_key_uid(uint64_t id, int uid, int gid);
int get_key_size_uid(uint64_t id, uint64_t* size, int uid, int gid);
int get_mode_uid(uint64_t id, int* modes, int uid, int gid);
int set_mode_uid(uint64_t id, int new_mode, int uid, int gid);
// password not needed, because it requires only root priviledges
int get_key_num(uint64_t* key_num);

int write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type);
int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len);
int remove_key(uint64_t id);
int get_key_size(uint64_t id, uint64_t* size);
int get_mode(uint64_t id, int* modes);
int set_mode(uint64_t id, int new_mode);

#endif

#endif //KEYREPO_KEYREPOSYSCALLWRAPPER_H
