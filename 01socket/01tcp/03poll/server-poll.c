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

#define SER_PORT 6666
#define SER_ADDR "0.0.0.0"

int main()
{
    int sfd, nsfd, sockfd /*临时变量*/;
    struct pollfd fds[BUFSIZ];
    struct sockaddr_in saddr, caddr;
    socklen_t len;
    char get_buff[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    int ret;
    int nready;
    int i,j;    //i：客户端迭代变量，j：消息转换迭代变量
    int maxi;
    ssize_t n;      //存放客户端消息的字节数

    //1 创建网络描述符
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    //2 绑定
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

    //4 接收连接
    fds[0].fd = sfd;
    fds[0].events = POLLIN;

    for (i = 1; i < BUFSIZ; i++)
    {
        fds[i].fd = -1;
    }
    maxi = 0;

    for(;;)
    {
        nready = poll(fds, maxi + 1, -1);   //阻塞等待，无穷大的时间
                                            //1.新客户端连接
                                            //2.旧客户端发送消息
        if (fds[0].revents & POLLIN)
        {
            // fds = sizeof(caddr);
            nsfd = accept(sfd, (void *)&caddr, &len);
            printf("有客户上线了\n");
            printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &caddr.sin_addr, str, sizeof(str)),
                    ntohs(caddr.sin_port));

            //将客户端连接的网络描述符添加到监听描述符集合中
            for (i = 1; i < BUFSIZ; i++)
            {
                if (fds[i].fd < 0)
                {
                    fds[i].fd = nsfd;
                    fds[i].events = POLLIN;
                    break;
                }
            }

            //当超出最大连接数时
            if (i == BUFSIZ)
            {
                perror("too many clients");
                exit(-1);
            }

            //更新maxi，manxi存放了当前监听描述符的个数
            if (i > maxi)
            {
                maxi = i;
            }

            //poll返回异常时
            if (--nready <= 0)
            {
                continue;
            }
        }

        //轮询查看，哪个客户端发送消息
        for (i = 1; i <= maxi; i++)
        {
            //防止赋值出错
            if ((sockfd = fds[i].fd) < 0)
            {
                continue;
            }

            //当客户端可读
            if (fds[i].revents & POLLIN)
            {
                //  printf("接收到了消息\n");
                if ((n = read(sockfd, get_buff, sizeof(get_buff))) >= 0)
                {

                    if (errno == ECONNRESET) /* 收到RST标志，异常关闭连接 */
                    {
                        printf("client[%d] aborted connection\n", i);
                        close(sockfd);
                        fds[i].fd = -1; //客户端断开连接，不再监听
                    }
                    else if(n == 0)
                    {
                        printf("client[%d] aborted connection\n", i);
                        close(sockfd);
                        fds[i].fd = -1; //客户端断开连接，不再监听
                    }
                    else
                    {
                        printf("received from %s at PORT %d:",
                                inet_ntop(AF_INET, &caddr.sin_addr, str, sizeof(str)),
                                ntohs(caddr.sin_port));
                        printf("%s\n", get_buff);
                        memset(get_buff, 0, sizeof(get_buff));
                        // for(j = 0; j < n; j++)
                        // {
                        //     get_buff[j] = toupper(get_buff[j]);
                        // }
                        // write(sockfd, get_buff, n);
                        // memset(get_buff, 0, sizeof(get_buff));
                    }

                    if(--nready <= 0);
                    {
                        break;
                    }
                    
                }
                else
                {
                    perror("read");
                    exit(-1);
                }
            }
        }

       
    }
    return 0;
}