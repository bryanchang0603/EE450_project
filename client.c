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
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main()
{
    char username[100] = "\0", password[100] = "\0";
    int sockfd, numbytes, auth_attempt = 0;
    char buf[MAXDATASIZE], TCP_request_buf[MAXDATASIZE];
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", TCP_port, &hints, &serverinfo)) == -1)
    {
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
        if ((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to bind \n");
        // exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(serverinfo); // all done with this structure

    printf("The client is up and running.\n");

    // authendaction process
    //  retriving username

    do
    {
        do
        {
            printf("Please enter the username:");
            scanf("%s", username);
            if (strlen(username) < 5 || strlen(username) > 50)
            {
                printf("\nusername invalid. The length must between 5 and 50 characters\n");
            }
        } while (strlen(username) < 5 || strlen(username) > 50);

        // retriving password
        do
        {
            printf("Please enter the password:");
            scanf("%s", password);
            if (strlen(password) < 5 || strlen(password) > 50)
            {
                printf("\npassword invalid. The length must between 5 and 50 characters\n");
            }
        } while (strlen(password) < 5 || strlen(password) > 50);

        strcpy(TCP_request_buf, username);
        strcat(TCP_request_buf, " ");
        strcat(TCP_request_buf, password);
        // printf("%s\n", TCP_request_buf);

        if (send(sockfd, "TCP_request_buf", strlen(TCP_request_buf), 0) == -1)
        {
            perror("auth request");
            exit(0);
        }

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("auth recv");
            exit(1);
        }
        if (strcmp(buf, "pass") != 0)
        {
            printf("incorrect username or password, please try again\n");
        }
        else
        {
            goto auth_success;
        }

        buf[numbytes] = '\0';
    } while (auth_attempt < 3);
    printf("3 attemps used, authencation failed. logging out");
    close(sockfd);
    exit(1);//terminate the client as requested

    auth_success:
        printf("authencation succeded");
    

    printf("client: received '%s'\n", buf);

    close(sockfd);
    return 0;
}