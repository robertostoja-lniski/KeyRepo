//
// Created by Robert Ostoja-Lniski on 1/2/21.
//

#ifndef KEYREPO_KEYREPOSYSCALLWRAPPER_H
#define KEYREPO_KEYREPOSYSCALLWRAPPER_H

#define EMULATION 0
#if EMULATION == 0

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

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
// password not needed, because it requires only root priviledges
int get_key_num(uint64_t* key_num);

#else

#include "KernelEmulation.h"

int write_key(const char* key, uint64_t key_len, const char* pass, uint64_t pass_len, uint64_t* id, int type);
int read_key(char* key, uint64_t id, const char* pass, uint64_t pass_len, uint64_t key_len);
int remove_key(uint64_t id);
int get_key_size(uint64_t id, uint64_t* size);
int get_mode(uint64_t id, int* modes);
int set_mode(uint64_t id, int new_mode);
// password not needed, because it requires only root priviledges
int get_key_num(uint64_t* key_num);

#endif

#endif //KEYREPO_KEYREPOSYSCALLWRAPPER_H
