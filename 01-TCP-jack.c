#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void * jack_recv(void *arg);

int main(int argc, char *argv[])
{
    // 1. 创建套接字（socket）
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2. 配置对方的网络信息（我们要发送数据给对方，一定要知道对方的IP和端口号）
    struct sockaddr_in rose_info;
    rose_info.sin_family = AF_INET;
    rose_info.sin_port = htons(50000);
    rose_info.sin_addr.s_addr = inet_addr("192.168.72.246");
    socklen_t len = sizeof(struct sockaddr_in);

    // 3. 建立连接
    pthread_t tid;
    if(connect(sockfd, (struct sockaddr *)&rose_info, len) == -1)
    {
        perror("connect");
        return -1;
    }
    else
    {
        printf("连接成功\n");
        pthread_create(&tid, NULL, jack_recv, (void *)&sockfd);
    }

    // 4. 发送数据
    char buf[1024] = {0};
    while(1)
    {
        bzero(buf, sizeof(buf));
        // printf("请输入要发送的数据：");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0';
        send(sockfd, buf, strlen(buf), 0);
        if(strcmp(buf, "exit") == 0)
        {
            break;
        }
    }
    
    
    // 5. 关闭套接字
    pthread_join(tid, NULL);
    
    close(sockfd);
    exit(0);
}

void * jack_recv(void *arg)
{
    int jack_sockfd = *(int *)arg;
    char buf[1024] = {0};
    while(1)
    {
        bzero(buf, sizeof(buf));
        recv(jack_sockfd, buf, sizeof(buf), 0);
        printf("rose: %s\n", buf);
        if(strcmp(buf, "exit") == 0)
        {
            pthread_exit(NULL);
        }
    }
}