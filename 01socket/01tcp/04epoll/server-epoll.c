#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <ctype.h>
#include <sys/epoll.h>

#define SER_PORT 6666
#define SER_ADDR "0.0.0.0"
#define OPEN_MAX 1024

int main()
{
    int sfd, nsfd, sockfd /*临时变量*/;
    struct sockaddr_in saddr, caddr;
    socklen_t len;
    char get_buff[BUFSIZ];
    char str[INET_ADDRSTRLEN];

    int nready, efd, res;
    int client[OPEN_MAX];
    int i, j; //i：客户端迭代变量，j：消息转换迭代变量
    int maxi;
    ssize_t n; //存放客户端消息的字节数
    struct epoll_event tep, ep[OPEN_MAX];

    //1 创建网络描述符
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    //2 绑定
    bzero(&caddr, sizeof(caddr));
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SER_ADDR);

    //端口复用
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(sfd, (void *)&saddr, sizeof(saddr));
    perror("bind");

    //3 监听网络文件描述符
    listen(sfd, 5);

    //初始化epoll
    for (i = 0; i < OPEN_MAX; i++)
        client[i] = -1;
    maxi = -1;

    efd = epoll_create(OPEN_MAX);
    if (efd == -1)
        perror("epoll");

    tep.data.fd = sfd;
    tep.events = EPOLLIN;

    res = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &tep);
    if (res == -1)
        perror("epoll_ctl");

    //4 接收连接

    for (;;)
    {
        nready = epoll_wait(efd, ep, OPEN_MAX, -1); //阻塞等待，无穷大的时间
                                                    //1.新客户端连接
                                                    //2.旧客户端发送消息
        if (nready == -1)
        {
            perror("epoll_wait");
            exit(-1);
        }

        for (i = 0; i < nready; i++)
        {
            if (!(ep[i].events & POLLIN))
                continue;

            if (ep[i].data.fd == sfd)
            {
                // fds = sizeof(caddr);
                nsfd = accept(sfd, (void *)&caddr, &len);
                printf("有客户上线了\n");
                printf("received from %s at PORT %d\n",
                       inet_ntop(AF_INET, &caddr.sin_addr, str, sizeof(str)),
                       ntohs(caddr.sin_port));

                //将客户端连接的网络描述符添加到监听描述符集合中
                for (j = 0; j < OPEN_MAX; j++)
                {
                    if (client[j] < 0)
                    {
                        client[j] = nsfd;

                        //将键列的客户端添加到监听集合中
                        tep.events = EPOLLIN;
                        tep.data.fd = nsfd;
                        res = epoll_ctl(efd, EPOLL_CTL_ADD, nsfd, &tep);

                        //更新maxi，manxi存放了当前监听描述符的个数
                        if (j > maxi)
                        {
                            maxi = j;
                        }
                        break;
                    }
                }

                //当超出最大连接数时
                if (j == OPEN_MAX)
                {
                    perror("too many clients");
                    exit(-1);
                }
            }
            else
            {
                sockfd = ep[i].data.fd;
                n = read(sockfd, get_buff, sizeof(get_buff));

                if (n == 0)
                {
                    for (j = 0; j <= maxi; j++)
                    {
                        if (client[j] == sockfd)
                        {
                            client[j] = -1;
                            break;
                        }
                    }

                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    if (res == -1)
                    {
                        perror("epoll_ctl");
                        exit(-1);
                    }
                    close(sockfd);
                    printf("client[%d] closed connection\n", j);
                }
                else
                {
                    printf("received from %s at PORT %d:",
                           inet_ntop(AF_INET, &caddr.sin_addr, str, sizeof(str)),
                           ntohs(caddr.sin_port));
                    printf("%s\n", get_buff);
                    memset(get_buff, 0, sizeof(get_buff));
                }
            }
        }
    }

    return 0;
}