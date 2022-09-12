
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

    printf("Running %s\n", action);
    const char* key = "1234567812345678";
    uint64_t key_len = 16;
    uint64_t pass_len = 16;
    uint64_t id;
    printf("Key is %s\n", key);
    // key type set for custom
    int writeRet = write_key(key, key_len, "passpasspasspass", pass_len, &id, 0);
    printf("Write ret: %d, id is: %lu\n", writeRet, id);

    uint64_t keyNum;
    int getKeyNumRet = get_key_num(&keyNum);
    printf("Get key num: %lu\n", keyNum);
    
    int mode;
    int getKeyModeRet = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRet, mode);

    int setKeyModeRet = set_mode(id, 777);
    printf("Set mode ret is %d\n", getKeyModeRet);

    int getKeyModeRetNew = get_mode(id, &mode);
    printf("Get mode ret is: %d and mode is: %d\n", getKeyModeRetNew, mode);

    uint64_t size;
    int getSizeRet = get_key_size(id, &size);
    printf("Get key size is: %d and size is: %lu\n", getSizeRet, size);

    char* buf = (char* )malloc(17);
    if (buf == NULL) {
        return 1;
    }
    memset(buf + 16, 0x00, 1);
    int readRet = read_key(buf, id, "passpasspasspass", pass_len, key_len);
    printf("Get key ret is: %d and key is: %s\n", readRet, buf);
    free(buf);

    char dummy;
    printf("Press any button to remove key...");
    scanf("%c", &dummy);

    int removeRet = remove_key(id);
    printf("Remove key ret is: %d\n", removeRet);

    int getKeyNumRetNoKey = get_key_num(&keyNum);
    printf("Get key num: %lu\n", keyNum);

    return 0;
}
