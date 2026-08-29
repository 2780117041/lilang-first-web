#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void * rose_send(void *arg);

int main(int argc, char *argv[])
{
    // 1. 创建套接字（socket）
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2. 配置自己的网络信息（明确自己是谁，才会知道后续数据进来以后给谁）
    struct sockaddr_in rose_info;
    rose_info.sin_family = AF_INET;
    rose_info.sin_port = htons(50000);
    rose_info.sin_addr.s_addr = inet_addr("192.168.72.3");
    socklen_t len = sizeof(struct sockaddr_in);

    // 2.1 设置端口复用
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(optval));

    // 3. 绑定（让自己的网络信息配置生效）
    if(bind(sockfd, (struct sockaddr *)&rose_info, len) == -1)
    {
        perror("bind");
        return -1;
    }

    // 4. 监听
    if(listen(sockfd, 4) == -1)
    {
        perror("connect");
        return -1;
    }

    // 5. 接受连接
    pthread_t tid;
    struct sockaddr_in jack_info;
    int jack_sockfd = accept(sockfd, (struct sockaddr *)&jack_info, &len);
    if(jack_sockfd == -1)
    {
        perror("connect");
        return -1;
    }
    else
    {
        printf("IP: %s, 端口号：%d链接成功\n", inet_ntoa(jack_info.sin_addr), ntohs(jack_info.sin_port));
        
        pthread_create(&tid, NULL, rose_send, (void *)&jack_sockfd);
    }

    // 6. 接受数据
    char buf[1024] = {0};
    while(1)
    {
        bzero(buf, sizeof(buf));
        recv(jack_sockfd, buf, sizeof(buf), 0);
        printf("jack: %s\n", buf);
        if(strcmp(buf, "exit") == 0)
        {
            break;
        }
    }
    
    pthread_join(tid, NULL);

    // 7. 关闭套接字
    close(jack_sockfd);
    close(sockfd);
    
    exit(0);
}

void * rose_send(void *arg)
{
    int jack_sockfd = *(int *)arg;
    char buf[1024] = {0};
    while(1)
    {
        bzero(buf, sizeof(buf));
        // printf("请输入要发送的数据：");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0';
        send(jack_sockfd, buf, strlen(buf), 0);
        if(strcmp(buf, "exit") == 0)
        {
            pthread_exit(NULL);
        }
    }
}
