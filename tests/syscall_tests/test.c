
#if __APPLE__
#else
#include <linux/kernel.h>
#endif

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define EMULATION 1

#if EMULATION == 1

#include "../../include/KernelEmulation.h"

#endif

int main(int argc, char *argv[])
{
    char action[1024];
    memset(action, 0x00, 1024);
    char *prefix = "rm ";
    strcat(action, prefix);
    strcat(action, partition);
    system(action);
    printf("Running %s\n", action);
    const char* key = "abcde";
    uint64_t id;
    printf("Key is %s\n", key);
    int writeRet = write_key(key, (const size_t)5, &id);
    printf("Write ret: %d\n", writeRet);
    int getKeyNumRet = get_key_num();
    printf("Get key num: %d\n", getKeyNumRet);
    
    int mode;
    int getKeyModeRet = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRet, mode);

    int setKeyModeRet = set_mode(id, 777);
    printf("Set mode ret is %d\n", getKeyModeRet);

    int getKeyModeRetNew = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRetNew, mode);

    uint64_t size;
    int getSizeRet = get_key_size(id, &size);
    printf("Get key size is: %d and size is: %llu\n", getSizeRet, size);

    char* buf = (char* )malloc(6);
    int readRet = read_key(id, buf, 6);
    free(buf);
    printf("Get key ret is: %d and key is: %s\n", readRet, buf);

    int removeRet = remove_key(id);
    printf("Remove key ret is: %d\n", removeRet);

    int getKeyNumRetNoKey = get_key_num();
    printf("Get key num: %d\n", getKeyNumRetNoKey);

    return 0;
}
