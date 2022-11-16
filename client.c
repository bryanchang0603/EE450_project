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

#define TCP_port "25048"
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
    char course[100] = "\0", category[50] = "\0";
    int sockfd, numbytes, auth_attempt = 0;
    char buf[MAXDATASIZE], TCP_request_buf[MAXDATASIZE];
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char temp;

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
    // printf("client: connecting to %s\n", s);
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
        strcat(TCP_request_buf, ",");
        strcat(TCP_request_buf, password);
        if (send(sockfd, TCP_request_buf, strlen(TCP_request_buf), 0) == -1)
        {
            perror("auth request");
            exit(0);
        }
        printf("%s sent an authentication request to the main server.\n", username);

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("auth recv");
            exit(1);
        }
        printf("%s received the result of authentication using TCP over port %s. ", username, TCP_port);
        buf[numbytes] = '\0';
        if (strcmp(buf, "password mismatch") == 0)
        {
            auth_attempt += 1;
            printf("Authentication failed: Password does not match\n");
        }
        else if (strcmp(buf, "user not found") == 0)
        {
            auth_attempt += 1;
            printf("Authentication failed: Username Does not exist\n");
        }
        else
        {
            goto auth_success;
        }
        printf("Attempts remaining:%d\n", 3 - auth_attempt);
    } while (auth_attempt < 3);
    printf("Authentication Failed for 3 attempts. Client will shut down.");
    close(sockfd);
    exit(1); // terminate the client as requested

auth_success:
    printf("Authentication is successful\n");

    while (1)
    {
        // resolve stdin with pervious scanf, then, read stdin with space
        //  credit to https://www.includehelp.com/c/c-program-to-read-string-with-spaces-using-scanf-function.aspx
        scanf("%c", &temp);
        printf("Please enter the course code to query:\n");
        scanf("%[^\n]", course); //compitable with the whitespace in multi course query
        if (strstr(course, " ") == NULL) // single query
        {
            printf("Please enter the category (Credit/Professor/Days/CourseName)\n");
            scanf("%c", &temp);
            scanf("%[^\n]", category);  
            strcpy(TCP_request_buf, course);
            strcat(TCP_request_buf, ",");
            strcat(TCP_request_buf, category);

            if (send(sockfd, TCP_request_buf, strlen(TCP_request_buf), 0) == -1)
            {
                perror("auth request");
                exit(0);
            }
            printf("%s sent a request to the main server.\n", username);

            if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
            {
                perror("auth recv");
                exit(1);
            }
            printf("The client received the response from the Main server using TCP over port %s.\n", TCP_port);
            buf[numbytes] = '\0';
            if (strcmp(buf, "course_not_found") == 0)
            {
                printf("Didn't find the course: %s\n", course);
            }
            else if (strcmp(buf, "category_not_found") == 0)
            {
                printf("Didn't find the catrgoty: %s\n", category);
            }
            else
            {
                printf("The %s of %s is %s\n", course, category, buf);
            }
        }
        else
        { // multiple query
            strcpy(TCP_request_buf, course);
            if (send(sockfd, TCP_request_buf, strlen(TCP_request_buf), 0) == -1)
            {
                perror("auth request");
                exit(0);
            }
            printf("%s sent a request with multiple CourseCode to the main server.\n", username);

            if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
            {
                perror("auth recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            printf("CourseCode: Credits, Professor, Days, Course Name\n");
            printf("%s\n", buf);
        }
        printf("-----Start a new request-----\n");
    }

    printf("client: received '%s'\n", buf);

    close(sockfd);
    return 0;
}