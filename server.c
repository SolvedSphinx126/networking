#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUF_SIZE 100

unsigned int users = 0;

int initServer() {

    struct addrinfo hints, *res;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    char ip4[INET_ADDRSTRLEN];
    char ip6[INET6_ADDRSTRLEN];
    int status;

    if ((status = getaddrinfo(NULL, "12312", &hints, &res)) != 0) {
        printf("%s\n", gai_strerror(status));
    } else {
        printf("Got info\n");
        if(res->ai_addr->sa_family == AF_INET) {
            if (inet_ntop(AF_INET, &((struct sockaddr_in *) res->ai_addr)->sin_addr, ip4, INET_ADDRSTRLEN)) {
                printf("ipv4 = %s\n", ip4);
            } else {
                printf("%s\n", gai_strerror(errno));
            }
        } else {
            if (inet_ntop(AF_INET6, &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr, ip6, INET6_ADDRSTRLEN)) {
                printf("ipv6 = %s\n", ip6);
            } else {
                printf("%s\n", gai_strerror(errno));
            }
        }
    }

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(errno != 0){
        printf("Create socket: %s\n", gai_strerror(errno));
        exit(1);
    } else {
        printf("Create socket: Success\n");
    }

    int port = 0;
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    bind(s, res->ai_addr, res->ai_addrlen);
    if (errno != 0)
    {
        printf("Bind socket: %s %d\n", gai_strerror(errno), errno);
        exit(1);
    } else {
        if(res->ai_family == AF_INET) {
            port = (int)(ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port));
        } else {
            port = (int)(ntohs(((struct sockaddr_in6 *)res->ai_addr)->sin6_port));
        }
    }
    printf("Port: %d\n", port);

    // need error handling`
    listen(s, 20);

    return s;
}

void getIp(struct sockaddr *addr, char* ip) {
    if(addr->sa_family == AF_INET) {
        inet_ntop(addr->sa_family, &((struct sockaddr_in *)addr)->sin_addr.s_addr, ip, INET6_ADDRSTRLEN);
        if (errno != 0) {
            printf("Get printable ip of new ipv4 client: %s\n", gai_strerror(errno));
            exit(1);
        }
    } else {
        inet_ntop(addr->sa_family, &((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr, ip, INET6_ADDRSTRLEN);
        if (errno != 0) {
            printf("Get printable ip of new ipv6 client: %s\n", gai_strerror(errno));
            exit(1);
        }
    }
}

int addUser(struct pollfd **pollList, int **userIds, int addedUserFd, int *numUsers, int *nextId) {
    *pollList = reallocarray(*pollList, (*numUsers) + 1, sizeof(struct pollfd));
    *userIds = reallocarray(*userIds, (*numUsers) + 1, sizeof(int));
    // should really check for errors here
    struct pollfd addedPollFd = {};
    (*pollList)[*numUsers].fd = addedUserFd;
    (*pollList)[*numUsers].events = POLLIN;
    (*userIds)[*numUsers] = *nextId;
    (*nextId)++;
    (*numUsers)++;
    return (*nextId) - 1;
}

//TODO Crashes when there are 3 users and user 2 quits
//maybe I can't change poll list at the time that I do?
void rmUser(struct pollfd **pollList, int **userIds, int iToRemove, int *numUsers) {
    close((*pollList)[iToRemove].fd);
    (*pollList)[iToRemove] = (*pollList)[(*numUsers) - 1];
    (*userIds)[iToRemove] = (*userIds)[(*numUsers) - 1];
    (*numUsers)--;
}

int main() {
    int users = 0;
    int nextId = 0;
    int server = initServer();
    struct pollfd *usersList;
    int *userIds;
    addUser(&usersList, &userIds, server, &users, &nextId);

    //int server = pollsocks[0].fd;
    char ip[INET6_ADDRSTRLEN] = "";
    socklen_t addrsize;
    int fd;
    int bytesRecieved = 0;
    int bufSize = 512;
    char buffer[bufSize];
    struct sockaddr *addr = &(struct sockaddr) {0};

    // Move common vars out of loop for efficiency
    int pollCount = 0;
    while(1) {
        memset(buffer, '\0', bufSize);
        int pollCount = poll(usersList, users, -1);
        if (pollCount == -1) {
            printf("Poll: %s%d\n", gai_strerror(errno), errno);
            exit(1);
        } else {
            for(int i = 0; i < users; i++) {
                fd = usersList[i].fd;
                if(usersList[i].revents & POLLIN) {
                    if(usersList[i].fd == server) {
                        addrsize = sizeof(struct sockaddr);
                        int newUserFd = accept(usersList[i].fd, (struct sockaddr *)addr, &addrsize);
                        if (errno != 0) {
                            printf("Accept new client: %s %s\n", gai_strerror(errno), strerror(errno));
                            exit(1);
                        } else {
                            getIp(addr, ip);
                        }
                        int uid = addUser(&usersList, &userIds, newUserFd, &users, &nextId);
                        printf("new user at ip=%s with user id %d!\n", ip, uid);
                    } else {
                        bytesRecieved = recv(usersList[i].fd, buffer, bufSize, 0);
                        if(bytesRecieved < 0) {
                            printf("Existing client recv: %s %s\n", gai_strerror(errno), strerror(errno));
                        } else if(bytesRecieved == 0) {
                            //usersList[i] = usersList[users--];
                            printf("User %d has disconnected from the server.\n", userIds[i]);
                            rmUser(&usersList, &userIds, i, &users);
                        } else {
                            printf("User %d (ip: %s) sent: %s\n", userIds[i], ip, buffer);
                            if(strcmp(buffer, "ls") == 0) {
                                dprintf(usersList[i].fd, "Listing other connected users:\n");
                                for(int j = 0; j < users; j++) {
                                    if(usersList[j].fd != server && i != j)
                                        dprintf(usersList[i].fd, "User %d\n", userIds[j]);
                                }
                            }
                            if(memcmp(buffer, "send", 4) == 0) {
                                char *dupe = malloc(strlen(buffer) * sizeof(char));
                                strcpy(dupe, buffer);
                                strtok(dupe, " ");
                                int recipientId = -1;
                                char* recipient = strtok(NULL, " ");
                                sscanf(recipient, "%d", &recipientId);
                                dprintf(usersList[i].fd, "you sent a message to user %s\n", recipient);
                                dprintf(usersList[recipientId].fd, "User %d sent you: %s", userIds[i], strtok(NULL, ""));
                            }
                            dprintf(usersList[i].fd, "You are user %d and you said \"%s\"", userIds[i], buffer);
                            // handle errors
                        }
                    }
                }
            }
        }
    }
}
