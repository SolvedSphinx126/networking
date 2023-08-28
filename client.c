#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>

#define BUF_SIZE 500


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    const struct sockaddr addr = {AF_INET, };
    //addr.sa_family = AF_INET;
    //addr.sa_data = "127.0.0.1";
    //connect(sock, );
}