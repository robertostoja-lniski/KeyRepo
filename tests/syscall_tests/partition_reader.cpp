#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
int main() {
    ifstream in_file("/dev/sdb2", ios::binary);
    in_file.seekg(0, ios::end);
    int file_size = in_file.tellg();
    cout<<"Size of the file is "<< file_size <<" bytes\n";

    int fd = open("/dev/sdb2", O_RDONLY);
    off_t size = lseek(fd, 0, SEEK_END);
    close(fd);

    cout<<"Size of the file is "<< size <<" bytes\n";

    struct stat *info;
    info =(struct stat *)malloc(sizeof(struct stat));
    if (!stat) {
        printf("Kmalloc failed\n");
        return -1;
    }

    stat("/dev/sdb2", info);
    size_t part_size = info->st_size;
    printf("Part size is %zu\n", part_size);

    free(info);

//    struct statvfs* buf = (struct statvfs* )malloc(sizeof(struct statvfs));
//    int statvfs("/dev/sdb2", buf);
//    std::cout <<

    auto fdp = fopen("/dev/sdb2", O_RDONLY);
    off_t fsize = fseek(fdp, 0, SEEK_END);
    fclose(fdp);
}