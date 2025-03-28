#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE     (4096 * 2)

int main() 
{
    srand(time(NULL));

    int fd = open("/dev/mtd0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct mtd_info_user mtd_info;
    if (ioctl(fd, MEMGETINFO, &mtd_info) < 0) {
        perror("ioctl");
        return -1;
    }
    printf("mtd type: %d\n", mtd_info.type);
    printf("mtd flags: %d\n", mtd_info.flags);
    printf("mtd size: %d\n", mtd_info.size);
    printf("mtd erasesize: %d\n", mtd_info.erasesize);
    printf("mtd writesize: %d\n", mtd_info.writesize);

    struct erase_info_user erase_info;
    erase_info.start = 0;
    erase_info.length = BUFFER_SIZE;
    if (ioctl(fd, MEMERASE, &erase_info) < 0) {
        perror("ioctl");
        return -1;
    }

    printf("Start to run write test\n");
    uint8_t wbuf[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        wbuf[i] = (uint8_t)(rand() % 256);
    }
    lseek(fd, 0, SEEK_SET);
    if (write(fd, wbuf, BUFFER_SIZE) < 0) {
        perror("write");
        return -1;
    }
    printf("Write test done\n");

    printf("Start to run read test\n");
    uint8_t rbuf[BUFFER_SIZE];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, rbuf, BUFFER_SIZE) < 0) {
        perror("read");
        return -1;
    }
    printf("Read test done\n");

    if (memcmp(wbuf, rbuf, BUFFER_SIZE) == 0) {
        printf("All tests passed\n");
    } else {
        printf("Read data is different from write data\n");
    }

    close(fd);
    return 0;
}