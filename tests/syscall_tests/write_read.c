
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

    char* buf = (char* )malloc(6);
    int readRet = read_key(buf, id, "pass", (uint64_t)4, 5);
    printf("Get key ret is: %d and key is: %s\n", readRet, buf);
    free(buf);

    return 0;
}
