
LINUX_KERNEL_NAME=linux-5.4.80
FILE=new_kernel/$LINUX_KERNEL_NAME.tar.xz

cp source/KernelEmulation.c new_kernel/linux-5.4.80/krepo/krepo.c
sed -i 's/define EMULATION 1/define EMULATION 0/' new_kernel/linux-5.4.80/krepo/krepo.c
cp include/KeyRepoHeaders.h new_kernel/linux-5.4.80/krepo
