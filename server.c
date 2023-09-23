#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUF_SIZE 500

struct client {
    struct sockaddr_in addr;
    int fd;
};

int getPort(in_port_t port) {
    uint16_t a = (port << 8);
    uint16_t b = (port >> 8);
    return (int) a | b;
}

char * getIp(in_addr_t addr) {
    u_int8_t a = addr & 255;
    addr >>= 8;

    u_int8_t b = addr & 255;
    addr >>= 8;

    u_int8_t c = addr & 255;

    addr >>= 8;

    u_int8_t d = addr & 255;
    char* ret = malloc(sizeof(char) * 15);
    sprintf(ret, "%d.%d.%d.%d", a, b, c, d);
    return ret;
    // leaks memory don't care yet tho
}

int initServer() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    const struct sockaddr_in addr = {};
    
    bind(sock, (struct sockaddr *restrict) &addr, (socklen_t) sizeof(addr));
    printf("Bind socket to addr: %s\n", strerror(errno));

    listen(sock, 16);
    printf("Start listening: %s\n", strerror(errno));

    struct sockaddr_in addr2 = {};
    int sa_len;
    sa_len = sizeof(addr2);
    getsockname(sock, (struct sockaddr_in *) &addr2, (socklen_t *restrict) &sa_len);
    printf("addr: %s\n", getIp(addr2.sin_addr.s_addr));
    printf("port: %d\n", getPort(addr2.sin_port));
    printf("Get sock info: %s\n", strerror(errno));
    return sock;
}

struct client acceptClient(int* serverFd) {
    struct client c = {};
    int sa_len = sizeof(c.addr);
    c.fd = accept(*serverFd, (struct sockaddr *restrict) &c.addr, (socklen_t *restrict) &sa_len);
    printf("Accept connection: %s\n", strerror(errno));
    return c;
}

// needs to handle message that's too long
char* recieveMessage(int bufSize, struct client c) {
    char* strBuf = malloc(bufSize*sizeof(char));
    struct sockaddr_in sender = {};
    int temp = sizeof(sender);
    if (recv(c.fd, strBuf, bufSize, 0) > 0) {
        char* res = malloc(strlen(strBuf) + 1);
        strcpy(res, strBuf);
        free(strBuf);
        return res;
    }
    else {
        printf("Recieve message error: %s\n", strerror(errno));
        return NULL;
    }
}

int main() {
    int serverFd = initServer();
    char* client_ip_addr = malloc(16 * sizeof(char));
    while (1) {
        struct client c = acceptClient(&serverFd);
        char* msg = recieveMessage(500, c);
	inet_ntop(AF_INET, &(c.addr.sin_addr), client_ip_addr, 16);
        printf("%s: %s\n", client_ip_addr, msg);
        free(msg);
    }
}
    
