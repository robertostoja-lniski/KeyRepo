if [ ! -d new_kernel ]; then
  mkdir new_kernel
fi


LINUX_KERNEL_NAME=linux-5.4.80
FILE=new_kernel/$LINUX_KERNEL_NAME.tar.xz
if [ ! -f "$FILE" ]; then
    wget https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/$LINUX_KERNEL_NAME.tar.xz -O new_kernel/$LINUX_KERNEL_NAME.tar.xz
else
  rm -rf new_kernel/$LINUX_KERNEL_NAME
fi

tar -xvf new_kernel/linux-5.4.80.tar.xz -C new_kernel/
mkdir new_kernel/linux-5.4.80/krepo
cp setup/changed_files_in_kernel/krepo/* new_kernel/linux-5.4.80/krepo
cp source/KernelEmulation.c new_kernel/linux-5.4.80/krepo/krepo.c
sed -i 's/define EMULATION 1/define EMULATION 0/' new_kernel/linux-5.4.80/krepo/krepo.c
cp include/KeyRepoHeaders.h new_kernel/linux-5.4.80/krepo
cp setup/changed_files_in_kernel/Makefile new_kernel/linux-5.4.80/
cp setup/changed_files_in_kernel/include/linux/syscalls.h new_kernel/linux-5.4.80/include/linux/syscalls.h
cp setup/changed_files_in_kernel/arch/x86/entry/syscalls/syscall_64.tbl new_kernel/linux-5.4.80/arch/x86/entry/syscalls/syscall_64.tbl
