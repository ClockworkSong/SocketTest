#include <iostream>

#include <cstdio> // perror()
#include <ctime> // ctime()
#include <unistd.h> // close()

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int PORT = 8056;

uint8_t buf[8] = {0x1, 0x2, 0x3, 0x4, 0x11, 0x22, 0x33, 0xff};

int main()
{
    //1. 调用socket函数创建套接字
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);// IPv4 Internet protocols
    if( socket_fd == -1) {
        perror("socket error:");
        return -1;
    }

    //2. 调用bind函数给创建的套接字，分配IP地址和端口
    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int socket_bind = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(sockaddr_in));
    if(socket_bind == -1) {
        perror("bind error:");
        return -1;
    }

    //3. 调用listen函数进行监听，等待客户端连接
    int socket_listen = listen(socket_fd, SOMAXCONN);// 最大连接数量
    if(socket_listen == -1) {
        perror("listen error:");
        return -1;
    }
    printf("size of buf = %d\n", sizeof(buf));
    // 客户端信息
    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(sockaddr_in);

    while (true)
    {
        printf("waiting for connect...\n");

        //4. 等待客户请求到来: 当请求到来后，调用accept函数接受连接请求，返回一个对应于此次连接的新的套接字，做好相互通信准备
        int socket_accept = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (socket_accept == -1) {
            perror("accept error:");
            break;
        }

        printf("accept ok!\n");

        printf("connect from ip := %s, port := %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        //5. 调用write/read或send/recv进行数据的读写，通过accept返回的套接字和客户端进行通信
        int ret = write(socket_accept, buf, sizeof(buf));
        if (ret == -1) {
            perror("send error:");
            close(socket_accept);
            break;
        }

        close(socket_accept);
    }

    //6. 关闭服务套接口close
    close(socket_fd);

    return 0;
}
