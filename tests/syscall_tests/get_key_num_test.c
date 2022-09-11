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
    uint64_t keyNum;
    int getKeyNumRet = get_key_num(&keyNum);
    printf("Get key num: %llu\n", keyNum);

    return 0;
}
