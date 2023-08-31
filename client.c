#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 500

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

int main() {
    u_int16_t portHuman;
    printf("What port do you wish to connect to? ");
    scanf("%hu", &portHuman);
    u_int16_t port = (portHuman << 8) | (portHuman >> 8);

    uint32_t ip = 0x0100007f;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    printf("addr: %s\n", getIp(ip));
    printf("port: %d\n", getPort(port));

    struct sockaddr_in addr = {AF_INET, port, ip};
    connect(sock, (struct sockaddr *) &addr, sizeof(addr));
    int test = errno;
    
    printf("Connect to server: %s\n", strerror(test));

    char* message = "this is a test\0";
    int val = sendto(sock, message, strlen(message), 0, (struct sockaddr *) &addr, sizeof(addr));
    printf("Send message to server: %s\n", strerror(test));
    printf("%d\n", val);
}