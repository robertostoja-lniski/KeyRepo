//
// Created by Robert Ostoja-Lniski on 1/2/21.
//

#ifndef KEYREPO_KEYREPOSYSCALLWRAPPER_H
#define KEYREPO_KEYREPOSYSCALLWRAPPER_H

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define __x64_get_key_num   436
#define __x64_write_key     437
#define __x64_read_key      438
#define __x64_remove_key    439
#define __x64_get_mode      440
#define __x64_set_mode      441
#define __x64_get_key_size  442

int get_key_num() {
    return syscall(__x64_get_key_num);
}

int write_key(const char* key, const size_t keyLen, uint64_t* id) {
    return syscall(__x64_write_key, key, keyLen, id);
}

int read_key(const uint64_t id, char* key, uint64_t keyLen) {
    return syscall(__x64_read_key, id, key, keyLen);
}

int remove_key(const uint64_t id) {
    return syscall(__x64_remove_key, id);
}

int get_key_size(const uint64_t id, uint64_t* size) {
    return syscall(__x64_get_key_size, id, size);
}

int get_mode(const uint64_t id, int* output) {
    return syscall(__x64_get_mode, id, output);
}

int set_mode(const uint64_t id, int new_mode) {
    return syscall(__x64_set_mode, id, new_mode);
}

#endif //KEYREPO_KEYREPOSYSCALLWRAPPER_H
