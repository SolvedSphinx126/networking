#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


#define BUF_SIZE 500

struct server {
    struct sockaddr_in addr;
    int fd;
};

struct server connectToServer(char* ip, u_int16_t port) {
    struct server s = {};
    s.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    printf("addr: %s\n", ip);
    printf("port: %d\n", ntohs(port));

    s.addr.sin_family = AF_INET;
    s.addr.sin_port = port;

    inet_pton(AF_INET, ip, &(s.addr.sin_addr.s_addr));
    connect(s.fd, (struct sockaddr *) &s.addr, sizeof(s.addr));
    printf("Connect to server: %s\n", strerror(errno));

    return s;
}

int main() {
    u_int16_t port;
    char* ip;

    printf("What ip would you like to connect to? ");
    scanf("%ms", &ip);
    
    printf("What port do you wish to connect to? ");
    scanf("%hu", &port);

    struct server s = connectToServer(ip, htons(port));

    char* message;
    printf("What is your message? ");
    scanf(" %m[^\n]", &message);
    
    sendto(s.fd, message, strlen(message), 0, (struct sockaddr *) &s.addr, sizeof(s.addr));
    printf("Send message to server: %s\n", strerror(errno));
    //printf("%d\n", val);
}
