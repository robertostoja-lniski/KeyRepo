
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

#define EMULATION 0

#if EMULATION == 1

#include "../../include/KernelEmulation.h"

#else

#include "../../include/KeyRepoSyscallWrapper.h"

#endif

int main(int argc, char *argv[])
{
    char action[1024];
    memset(action, 0x00, 1024);
    char *prefix = "rm ";
    strcat(action, prefix);
    strcat(action, ".keyPartition");
//    system(action);
    printf("Running %s\n", action);
    const char* key = "abcde";
    uint64_t id;
    printf("Key is %s\n", key);
    // key type set for custom
    int writeRet = write_key(key, (uint64_t)5, "pass", (uint64_t)4, &id, 0);
    printf("Write ret: %d\n", writeRet);

    uint64_t keyNum;
    int getKeyNumRet = get_key_num(&keyNum);
    printf("Get key num: %llu\n", keyNum);
    
    int mode;
    int getKeyModeRet = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRet, mode);

    int setKeyModeRet = set_mode(id, 777);
    printf("Set mode ret is %d\n", getKeyModeRet);

//    return 0;
    int getKeyModeRetNew = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRetNew, mode);

    uint64_t size;
    int getSizeRet = get_key_size(id, &size);
    printf("Get key size is: %d and size is: %llu\n", getSizeRet, size);

    char* buf = (char* )malloc(6);
    int readRet = read_key(buf, id, "pass", (uint64_t)4, 6);
    printf("Get key ret is: %d and key is: %s\n", readRet, buf);
    free(buf);

    return 0;
}
