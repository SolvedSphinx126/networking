#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

// support ipv6
struct pollfd connectToServer(char* ip, u_int16_t port) {
    struct pollfd poll;
    struct sockaddr_in addr; 

    // Create a socket
    poll.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    printf("addr: %s\n", ip);
    printf("port: %d\n", ntohs(port));

    // Set up the address structure for the server
    addr.sin_family = AF_INET;
    addr.sin_port = port;

    inet_pton(AF_INET, ip, &(addr.sin_addr.s_addr));
    // Attempt to connect to the server
    if(connect(poll.fd, (struct sockaddr *) &addr, sizeof(addr)) == 1)
        printf("Connect to server: %s\n", strerror(errno));

    return poll;
}

int main() {
    u_int16_t port;
    char* ip;
    struct pollfd pollArr[2];

    // Get the IP address and port from the user
    printf("What ip would you like to connect to? ");
    scanf("%ms", &ip);
    
    printf("What port do you wish to connect to? ");
    scanf("%hu", &port);

    // Create a socket and set up poll structures
    pollArr[0] = connectToServer(ip, htons(port));
    pollArr[0].events = POLLIN;
    pollArr[1].fd = 1;    // File descriptor for standard input (stdin)
    pollArr[1].events = POLLIN;

    int bufSize = 500;
    char message[bufSize];

    printf("type ls to get list of connected users from the server\n");
    printf("type \"send <id> <message>\" to send a message to another user\n");
    printf("type q to quit the program\n");

    while (1) {
        memset(message, '\0', bufSize);
        int rets = poll(pollArr, 2, -1);
        if(rets == -1) {
            printf("Send message to server: %s\n", strerror(errno));
        } else {
            if(pollArr[0].revents & POLLIN) {
                // Server sent a message, receive and print it
                recv(pollArr[0].fd, message, bufSize, 0);
                printf("%s\n", message);
            }
            else if (pollArr[1].revents & POLLIN) {
                // User typed a message, read it and send it to the server
                scanf(" %[^\n]s", message);
                if(strcmp(message, "q") == 0) {
                    printf("Exiting\n");
                    exit(0);
                }
                send(pollArr[0].fd, message, strlen(message), 0);
                if(errno != 0)
                    printf("Send message to server: %s\n", strerror(errno));
            }
        }
    }
}
