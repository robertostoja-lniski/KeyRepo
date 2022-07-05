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

#define __x64_get_key_num   436
#define __x64_write_key     437
#define __x64_read_key      438
#define __x64_remove_key    439
#define __x64_get_mode      440
#define __x64_set_mode      441
#define __x64_get_key_size  442

int get_key_num();
int write_key(const char* key, const uint64_t keyLen, uint64_t* id);
int read_key(const uint64_t id, char* key, uint64_t keyLen);
int remove_key(const uint64_t id);
int get_key_size(const uint64_t id, uint64_t* size);
int get_mode(const uint64_t id, int* output);
int set_mode(const uint64_t id, int new_mode);

#else

#include "KernelEmulation.h"

int write_key(const char* key, const uint64_t keyLen, uint64_t* id, uint8_t type);
int read_key(const uint64_t id, char* key, uint64_t keyLen, uint8_t type);
int remove_key(const uint64_t id);
int get_key_size(const uint64_t id, uint64_t* size);
int get_mode(const uint64_t id, int* output);
int set_mode(const uint64_t id, int new_mode);
int get_key_num();

#endif

#endif //KEYREPO_KEYREPOSYSCALLWRAPPER_H
