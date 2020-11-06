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
    int sfd,nsfd;
    struct sockaddr_in saddr,caddr;
    socklen_t len;
    char get_buff[BUFSIZ];
    int ret;

    //1 创建网络描述符
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    //2 绑定
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SER_ADDR);

    //端口复用
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(sfd, (void*)&saddr, sizeof(saddr));
    perror("bind");

    //3 监听网络文件描述符
    listen(sfd, 5);

    //4 接收连接
    nsfd = accept(sfd, (void*)&caddr, &len);

    //5 收发数据
    while(1)
    {
        if(strcmp("q", get_buff) == 0)
        {
            break;
        }
        ret = read(nsfd, get_buff, sizeof(get_buff));
        if(ret > 0)
            printf("%s %d\n", get_buff, strlen(get_buff));
    }

    //6 关闭网络描述符
    close(sfd);
}