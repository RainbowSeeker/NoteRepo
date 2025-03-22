#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>

#define IO_WAY    2 // 0: blocking IO, 1: non-blocking IO (O_NONBLOCK multiplexing), 2: signal IO (SIGIO)

int read_blocking(int fd, char *buf, int len)
{
    int ret = read(fd, buf, len);
    if (ret < 0)
    {
        perror("read");
        return -1;
    }
    buf[ret] = '\0';
    printf("read: %s\n", buf);
    return 0;
}

int read_nonblocking(int fd, char *buf, int len)
{
    if (!(fcntl(fd, F_GETFL) & O_NONBLOCK)) {
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0)
        {
            perror("fcntl");
            return -1;
        }
    }
    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    int ret = poll(fds, 1, 1000);
    switch (ret)
    {
    case -1:
        perror("poll");
        return -1;
    case 0:
        printf("timeout\n");
        return 0;
    default:
        if (fds[0].revents & POLLIN)
        {
            read_blocking(fd, buf, len);
        }
        break;
    }
    return 0;
}

static int fd;
static char rbuf[1024] = {0};
void sigio_handler(int signo)
{
    printf("sigio_handler\n");
    read_blocking(fd, rbuf, sizeof(rbuf));
}

int main()
{
    fd = open("/dev/chardev0", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
#if IO_WAY == 2
    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);
    signal(SIGIO, sigio_handler);
#endif

    while (1)
    {
        write(fd, "hello world from user\n", 22);
#if IO_WAY == 0
        read_blocking(fd, rbuf, sizeof(rbuf));
#elif IO_WAY == 1
        read_nonblocking(fd, rbuf, sizeof(rbuf));
#endif
        sleep(1);
    }
    
    close(fd);
    return 0;
}