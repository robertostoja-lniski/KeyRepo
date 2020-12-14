#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>

static kuid_t getuid(void)
{
    return current_uid();
}

static kgid_t getgid(void)
{
    return current_gid();
}

static kgid_t getegid(void)
{
    return current_egid();
}

static kuid_t geteuid(void)
{
    return current_euid();
}

SYSCALL_DEFINE0(krepo)
{
    printk("Syscall works!\n");
    printk("Real user ID: %d\n", getuid());
    printk("Effective user ID: %d\n", geteuid());
    printk("Real group ID: %d\n", getgid());
    printk("Effective group ID: %d\n", getegid());

    return 0;
}

