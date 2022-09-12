
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

#if EMULATION == 1

#include "../../include/KernelEmulation.h"

#else

#include "../../include/KeyRepoSyscallWrapper.h"

#endif

int main(int argc, char *argv[])
{
    int size = 256;
    printf("Reading 256 key bytes from key\n");
    char* buf = (char* )malloc(size);
    if (buf == NULL) {
        return 1;
    }
    memset(buf, 0x00, size);

    uint64_t id;
    printf("Enter key id\n");
    scanf("%lu", &id);

    int read_key_ret = read_key(buf, id, "1234567812345678", (uint64_t)16, (uint64_t)size);
    printf("\nGet key ret is: %d and key is: %s\n", read_key_ret, buf);

    return 0;
}
