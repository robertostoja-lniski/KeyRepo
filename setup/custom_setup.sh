rm -rf linux*
tar -xvf ../linux-5.4.80.tar.xz -C .
src=changed_files_in_kernel
mkdir linux-5.4.80/krepo
cp $src/krepo/* linux-5.4.80/krepo
cp src/KeyRepo/source/KernelEmulation.c linux-5.4.80/krepo
cp src/KeyRepo/source/KeyRepoHeaders.h linux-5.4.80/krepo
cp $src/Makefile linux-5.4.80/
cp $src/include/linux/syscalls.h linux-5.4.80/include/linux/syscalls.h
cp $src/arch/x86/entry/syscalls/syscall_64.tbl linux-5.4.80/arch/x86/entry/syscalls/syscall_64.tbl
