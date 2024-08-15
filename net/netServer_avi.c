#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <winsock2.h>

#define BUF_SIZE 32768
#define PORT 8080

// 服务器192.168.1.23  8888
int main(void) {
    int sockfd, new_sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;
    int n;
    char buffer[BUF_SIZE];

    // 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("172.9.1.125");
    serv_addr.sin_port = htons(PORT);

    // 绑定套接字到指定端口
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    //初始化客户端地址结构
    memset(&serv_addr, 0, sizeof(cli_addr));
    // cli_addr.sin_family = AF_INET;
    // cli_addr.sin_addr.s_addr = inet_addr("192.168.1.24");
    // cli_addr.sin_port = htons(PORT);

    //监听
    if(listen(sockfd,64) == -1)
    {
        perror("listen failed");
        exit(1);
    }
    printf("Server is listening on port %d...\n", PORT);

    cli_addr_len = sizeof(cli_addr);
    int cli_fd = accept(sockfd,(struct sockaddr *)&cli_addr,(socklen_t *)&cli_addr_len);
    if(cli_fd == -1)
    {
        perror("accept fail");
    }
    printf("accept:%s\n",inet_ntoa(cli_addr.sin_addr));


    

    // 接收文件大小
    n = recv(cli_fd, buffer, BUF_SIZE, 0);
    if (n < 0) 
    {
        perror("recvfrom error for file size");
        exit(EXIT_FAILURE);
    }
    long file_size = atol(buffer);
    printf("Expected file size: %ld bytes\n", file_size);

    long received_bytes = 0;
    FILE *fp = fopen("received.avi", "wb");

    // 接收视频数据
    while (received_bytes < file_size) {
        n = recv(cli_fd, buffer, BUF_SIZE, 0);
        if (n < 0) {
            perror("recvfrom error for video data");
            exit(EXIT_FAILURE);
        }
        fwrite(buffer, sizeof(char), n, fp);
        received_bytes += n;
        printf("已接受到数据:%ld\n",received_bytes);
    }

    printf("接收成功\n");
    fclose(fp);
    close(sockfd);

    return 0;
}
