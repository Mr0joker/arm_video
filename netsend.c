#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
// 客户端
#define BUF_SIZE 32768
#define PORT 8888

// 发送给服务器192.168.1.23  8888
int send_avi(void)
{
    int sock, read_size, send_len;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr;
    FILE *fp;

    // 创建TCP套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    // 设置套接字地址和端口
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.23");
    serv_addr.sin_port = htons(PORT);

    // bind
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind fail");
        exit(1);
    }

    // 打开视频文件
    if ((fp = fopen("./output.avi", "rb")) == NULL)
    {
        perror("fopen error");
        exit(1);
    }

    // 发送视频数据
    printf("Start sending video...\n");
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("Video size: %ld bytes\n", file_size);

    send_len = sprintf(buffer, "%ld", file_size);
    if (send(sock, buffer, send_len, 0) < 0)
    {
        perror("sendto error");
        exit(1);
    }
    time_t start_time = time(NULL);
    int i = 0;
    while (1)
    {
        read_size = fread(buffer, sizeof(char), BUF_SIZE, fp);
        if (read_size <= 0)
        {
            break;
        }
        if (send(sock, buffer, read_size, 0) < 0)
        {
            perror("sendto error");
            exit(1);
        }
        i += read_size;
        printf("已发送发送%d\n", i);
        // usleep(10000); // 10ms
    }

    time_t end_time = time(NULL);
    printf("Video sent successfully in %ld seconds!\n", end_time - start_time);

    // 关闭套接字和文件
    fclose(fp);
    close(sock);

    return 0;
}

int send_photo(char *filename)
{
    int sock, read_size, send_len;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr;
    FILE *fp;
    printf("%s\n", filename);
    // 创建TCP套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    // 设置套接字地址和端口
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.23");
    serv_addr.sin_port = htons(PORT);

    // bind
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind fail");
        exit(1);
    }

    // 打开图片文件
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        perror("fopen error");
        exit(1);
    }

    // 发送视频数据
    printf("Start sending video...\n");
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("Video size: %ld bytes\n", file_size);

    send_len = sprintf(buffer, "%ld", file_size);
    if (send(sock, buffer, send_len, 0) < 0)
    {
        perror("sendto error");
        exit(1);
    }
    time_t start_time = time(NULL);
    int i = 0;
    while (1)
    {
        read_size = fread(buffer, sizeof(char), BUF_SIZE, fp);
        if (read_size <= 0)
        {
            break;
        }
        if (send(sock, buffer, read_size, 0) < 0)
        {
            perror("sendto error");
            exit(1);
        }
        i += read_size;
        printf("已发送发送%d\n", i);
        // usleep(10000); // 10ms
    }

    time_t end_time = time(NULL);
    printf("Video sent successfully in %ld seconds!\n", end_time - start_time);

    // 关闭套接字和文件
    fclose(fp);
    close(sock);

    return 0;
}

// int send_message(char *message)
// {
//     // 1.创建一个套接字
//     int sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock == -1)
//     {
//         perror("create socket failed");
//         return -1;
//     }
//     // 指定要连接的服务器端的地址:IP+PROT
//     struct sockaddr_in server;
//     server.sin_family = AF_INET;
//     server.sin_port = htons(atoi("8080"));
//     server.sin_addr.s_addr = inet_addr("172.9.1.199");
//     // 2.根据IP和端口号连接指定计算机的端口
//     int ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
//     if (ret == -1)
//     {
//         perror("connect server failed");
//         return -1;
//     }
//     char buf[1024] = {0};
//     char *mess = message;

//     // 3.通过套接字向服务器端发送信息
//     send(sock, mess, sizeof(mess), 0);
//     // 接收服务器回给我们的数据
//     recv(sock, buf, 1024, 0);

//     printf("%s\n", buf);

//     // 4.关闭套接字
//     close(sock);
// }
