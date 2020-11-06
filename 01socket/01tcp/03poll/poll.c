#include <unistd.h>
#include <stdio.h>
#include <poll.h>

void main()
{
    struct pollfd fds[2];
    int tm = 1;
    int err;
    int fd = 23;
    fds[0].fd = 0;
    fds[0].events = POLLOUT;
    fds[1].fd = 1;
    fds[1].events = POLLIN;

    while(1)
    {
        err = poll(fds, 2, -1);

        if(err == 0)
        {
            printf("timeout\n");
        }

        //测试 标准输入是否可写  1
        if(fds[0].revents & POLLOUT)
        {
            printf("stdin can use\n");
        }

        //测试 标准输出是否可读 有内容时可读
        if(fds[1].revents & POLLIN)
        {
            printf("stdout can use\n");
        }

        sleep(1);
    }
}