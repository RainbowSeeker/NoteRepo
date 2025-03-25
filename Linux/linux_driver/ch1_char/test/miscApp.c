#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define MMAP_SIZE (getpagesize() * 4)

int main()
{
    srand(time(NULL));

    int fd = open("/dev/my_misc_device", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    char *map_buf = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map_buf == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    printf("mmap: %s\n", map_buf);
    
    /*-------------- TEST read ----------------------------------------------*/
    char buf[MMAP_SIZE];
    for (int i = 0; i < MMAP_SIZE; i++)
    {
        buf[i] = rand() % 26 + 'a';
    }
    int ret = write(fd, buf, sizeof(buf));
    if (ret < 0 || ret != sizeof(buf))
    {
        perror("write");
        return -1;
    }

    // check
    printf("TEST write: ");
    if (memcmp(buf, map_buf, MMAP_SIZE) == 0)
        printf("buf and map_buf are the same\n");
    else
        printf("buf and map_buf are different\n");

    /*-------------- TEST read ----------------------------------------------*/
    for (int i = 0; i < MMAP_SIZE; i++)
    {
        map_buf[i] = rand() % 26 + 'a';
    }

    // from beginning
    lseek(fd, 0, SEEK_SET);
    ret = read(fd, buf, sizeof(buf));
    if (ret < 0 || ret != sizeof(buf))
    {
        perror("read");
        return -1;
    }

    // check
    printf("TEST read: ");
    if (memcmp(buf, map_buf, MMAP_SIZE) == 0)
        printf("buf and map_buf are the same\n");
    else
        printf("buf and map_buf are different\n");

    munmap(map_buf, MMAP_SIZE);
    close(fd);
    return 0;
}