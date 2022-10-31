/**
 * @file client.c
 * @author Baosheng Chang (changb9@outlook.com)
 * @brief The client program of the project
 * @version 0.1
 * @date 2022-10-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define TCP_port "25000048"
#define MAXDATASIZE 5000

// the following code are form Beej's book
/**
 * @brief Get the in addr object's IPv4 address
 * 
 * @param sa 
 * @return void* 
 */
void *get_in_addr(struct sockaddr *sa){
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(){
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo("localhost", TCP_port, &hints, &serverinfo)) == -1){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next)
    { // bind to the first result
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        if((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1){
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to bind \n");
        exit(1);
    }

}