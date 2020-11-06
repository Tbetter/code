#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define SER_PORT 6666
#define SER_ADDR "127.0.0.1"

void main()
{
    int sfd;
    struct sockaddr_in saddr,caddr;
    socklen_t len;
    char put_buff[BUFSIZ] = "c to s";

    //1 创建网络描述符
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    //2 初始化
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SER_ADDR);

    //4 请求连接
    connect(sfd, (void*)&saddr, sizeof(saddr));
    perror("connect");

    //5 收发数据
    while(1)
    {
        memset(put_buff, 0, sizeof(put_buff));
        scanf("%s", put_buff);
        write(sfd, put_buff, sizeof(put_buff));
        perror("send");
        if(strcmp("q", put_buff) == 0)
        {
            break;
        }
    }

    //6 关闭网络描述符
    close(sfd);
}