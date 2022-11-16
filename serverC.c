/**
 * @file serverC.C
 * @author Baosheng Chang (changb9@outlook.com)
 * @brief The Crediential Server of the project
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

#define UDP_port_C "21048"
#define MAXBUFFER 5000

/**
 * @brief The user_node for storing the user information.
 * linked list will be used for storing user information
 * @param username the username of the account
 * @param password the password of the account
 */
struct user_node
{
    char username[50];
    char password[50];
    struct user_node *next;
};

struct user_node *head;
struct user_node *current;

/**
 * @brief append the new user_node to the begining of the linked list
 *
 * @param username_in string. max_len = 50 input username.
 * @param password_in string. max_len = 50 input password.
 */
void auth_append_front(char username_in[50], char password_in[50])
{
    struct user_node *tempuser_node = (struct user_node *)malloc(sizeof(struct user_node));
    strcpy(tempuser_node->username, username_in);
    strcpy(tempuser_node->password, password_in);
    tempuser_node->next = head;
    head = tempuser_node;
};
/**
 * @brief delete the linked list and free all memory allocated to it
 *
 */

void delete_list()
{
    current = head;
    while (current != NULL)
    {
        head = head->next;
        free(current);
        current = head;
    }
};

/**
 * @brief print all user's username and password
 *
 */
void print_all()
{
    struct user_node *temp = head;
    while (temp != NULL)
    {
        current = temp;
        temp = temp->next;
        printf("user:%s|pass:%s\n", current->username, current->password);
    };
}

/**
 * @brief the authencation process. check if the user is in the linked lisk
 *
 * @param username_in the input username
 * @param password_in the input password
 * @return int: the result of the authencation.
 *              1 indicates the authencation pass
 *              2 indicates password missmatch
 *              0 indicates username did not found
 */
char *check_username(char *username_in, char *password_in)
{
    struct user_node *temp = head;
    while (temp != NULL)
    {
        current = temp;
        temp = temp->next;
        if (strcmp(current->username, username_in) == 0)
        {
            if (strcmp(current->password, password_in) == 0)
            {
                return ("pass"); // pass
            }
            else
            {
                return ("password mismatch"); // password mismatch
            };
        }
    };
    return ("user not found"); // user not found
};

// The following code are based on Beej's code
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main()
{
    char cred_buffer[200];
    char auth_result[50];
    char *username_buff, *password_buff;
    FILE *cred_file = fopen("cred.txt", "r");
    int result = -1;

    // following parameters are for UDP server
    int sockfd;
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage client_addr;
    char request_buff[MAXBUFFER];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    // read by line and store the username and pass into the linked list
    while (fgets(cred_buffer, 200, cred_file) != NULL)
    {
        // extract info
        username_buff = strtok(cred_buffer, ",");
        password_buff = strtok(NULL, "");
        if (strstr(password_buff, "\n"))
        {
            password_buff[strlen(password_buff) - 2] = '\0';
        };
        // printf("%s  user\n", username_buff);
        // printf("%s  pass\n", password_buff);
        auth_append_front(username_buff, password_buff);
    }
    fclose(cred_file);

    // setting up UDP server
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo("localhost", UDP_port_C, &hints, &serverinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = serverinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(serverinfo);
    printf("The ServerC is up and running using UDP on port 21048.\n");

    addr_len = sizeof client_addr;
    while (1)
    {
        if ((numbytes = recvfrom(sockfd, request_buff, MAXBUFFER - 1, 0,
                                 (struct sockaddr *)&client_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("The ServerC received an authentication request from the Main Server.\n");

        // printf("listener: got packet from %s\n",
        //        inet_ntop(client_addr.ss_family,
        //                  get_in_addr((struct sockaddr *)&client_addr),
        //                  s, sizeof s));
        request_buff[numbytes] = '\0';
        username_buff = strtok(request_buff, ",");
        password_buff = strtok(NULL, "");
        
        strcpy(auth_result, check_username(username_buff, password_buff));
        if ((numbytes = sendto(sockfd, auth_result, strlen(auth_result), MSG_CONFIRM,
                               (struct sockaddr *)&client_addr, addr_len)) == -1)
        {
            perror("auth_talker: sendto");
            exit(1);
        }
        printf("The ServerC finished sending the response to the Main Server.\n");
    };

    close(sockfd);

    // print_all();
    // result = check_username("eqfiv", "Xl!v7si8w");
    // printf("%d \n", result);
    // result = check_username("1", "Xl!2");
    // printf("%d \n", result);
    // result = check_username("eqfiv", "Xl!2");
    // printf("%d \n", result);
    // result = check_username("vskiv", "gSrxve8@tswmxmz5i");
    // printf("%d \n", result);
    // result = check_username("eqf8iv", "Xl!v7si8w");
    // printf("%d \n", result);
    delete_list();
}