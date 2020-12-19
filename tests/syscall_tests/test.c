#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define __NR_identity 437

long test_syscall(const char * path, size_t len, uint64_t** id)
{
    return syscall(__NR_identity, path, len, id);
}

int main(int argc, char *argv[])
{
    long activity;
    size_t len = 5;
    uint64_t* id;
    activity = test_syscall("key.txt", len, &id);

    if(activity < 0)
    {
    	    printf("RETURN: %ld\n", activity);
	    perror("Sorry, Jasper. Your system call appears to have failed.");
    }

    else
    {
        printf("Congratulations, Jasper! Your system call is functional. Run the command dmesg in the terminal and find out!\n");
    }

    return 0;
}
