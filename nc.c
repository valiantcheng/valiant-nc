#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void run_client(const char *address, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Socket creation failed");
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        error("Invalid address/ Address not supported");
    }

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Connection failed");
    }

    printf("Connected to %s:%d\n", address, port);
    printf("Type 'exit' to close the connection.\n");

    // 数据传输
    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 检查是否输入了 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }

        send(sockfd, buffer, strlen(buffer), 0);
	perror("Error TCP send");
        
        int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) {
            printf("Connection closed by server\n");
            break;
        }
        buffer[n] = '\0';
        printf("Server: %s", buffer);
    }

    close(sockfd);
}

void run_server(int port) {
    int sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Socket creation failed");
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 绑定端口
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Bind failed");
    }

    // 监听连接
    if (listen(sockfd, 5) < 0) {
        error("Listen failed");
    }

    printf("Listening on port %d...\n", port);

    // 接受客户端连接
    client_len = sizeof(client_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (newsockfd < 0) {
        error("Accept failed");
    }

    printf("Connected to client\n");
    printf("Type 'exit' to close the connection.\n");

    // 数据传输
    while (1) {
        int n = recv(newsockfd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) {
            printf("Connection closed by client\n");
            break;
        }
        buffer[n] = '\0';
        printf("Client: %s", buffer);

        // 检查是否接收到 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }

        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 检查是否输入了 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            send(newsockfd, buffer, strlen(buffer), 0);
            break;
        }

        send(newsockfd, buffer, strlen(buffer), 0);
    }

    close(newsockfd);
    close(sockfd);
}

void run_udp_client(const char *address, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("Socket creation failed");
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        error("Invalid address/ Address not supported");
    }

    addr_len = sizeof(server_addr);

    printf("Connected to %s:%d\n", address, port);
    printf("Type 'exit' to close the connection.\n");

    // 数据传输
    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 检查是否输入了 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }

        // 发送数据到服务器
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
        perror("Error UDP sendto");

        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (n <= 0) {
            printf("Connection closed by server\n");
            break;
        }
        buffer[n] = '\0';
        printf("Server: %s", buffer);
    }

    close(sockfd);
}

void run_udp_server(int port) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("Socket creation failed");
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 绑定端口
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Bind failed");
    }

    printf("Listening on port %d...\n", port);

    client_len = sizeof(client_addr);

    // 数据传输
    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n <= 0) {
            printf("Connection closed by client\n");
            break;
        }
        buffer[n] = '\0';
        printf("Client: %s", buffer);

        // 检查是否接收到 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }

        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 检查是否输入了 "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Closing connection...\n");
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
            break;
        }

        // 发送数据到客户端
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -l <port> udp|tcp (for server) or %s <address> <port> udp|tcp (for client)\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int is_udp = strcmp(argv[3], "udp") == 0;

    if (strcmp(argv[1], "-l") == 0) {
        int port = atoi(argv[2]);
        if (is_udp) {
            run_udp_server(port);
        } else {
            run_server(port);
        }
    } else {
        const char *address = argv[1];
        int port = atoi(argv[2]);
        if (is_udp) {
            run_udp_client(address, port);
        } else {
            run_client(address, port);
        }
    }

    return 0;
}
