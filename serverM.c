/**
 * @file serverM.c
 * @author Baosheng Chang (changb9@outlook.com)
 * @brief The Main Server of the project
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
#include <signal.h>

#define TCP_port "25048"
#define UDP_port_M "24048"
#define UDP_port_C "21048"
#define UDP_port_CS "22048"
#define UDP_port_EE "23048"
#define BACKLOG 5
#define MAXBUFFER 5000

/**
 * @brief encrypt the input string based on the requirement
 *
 * @param string_in the string going to be encrpyted. should be less than length 150
 * @return char*  the excrypted string
 */
char *string_encrypt(char *string_in)
{
    char *string_out = malloc(sizeof(char) * 150);
    free(string_out); // celar the pervious data in memory
    string_out = malloc(sizeof(char) * 150);
    strcpy(string_out, "");
    char alphbet[60] = "abcdefghijklmnopqrstuvwxyzABECDFGHIJKLMNOPQRSTUVWXYZ";
    char alphbet_last_four[10] = "wxyzWXZY";
    char number[10] = "0123456789";
    char number_last_four[5] = "67890";
    int i = 0;
    char char_i;
    char str_i_old[2] = "\0";
    char str_i_new[2] = "\0";
    for (i = 0; i < strlen(string_in); i++)
    {
        // read the char, and covert to string
        char_i = string_in[i];
        str_i_new[0] = char_i + 4;
        str_i_old[0] = char_i;
        printf("%s, %s\n", str_i_old, str_i_new);
        if (strstr(alphbet, str_i_old)) // alphbet offest
        {
            if (strstr(alphbet_last_four, str_i_old)) // the loopback alphbet offest
            {
                switch (char_i)
                {
                case 'w':
                    strcat(string_out, "a");
                    break;
                case 'x':
                    strcat(string_out, "b");
                    break;
                case 'y':
                    strcat(string_out, "c");
                    break;
                case 'z':
                    strcat(string_out, "d");
                    break;
                case 'W':
                    strcat(string_out, "A");
                    break;
                case 'X':
                    strcat(string_out, "B");
                    break;
                case 'Y':
                    strcat(string_out, "C");
                    break;
                case 'Z':
                    strcat(string_out, "D");
                    break;
                default:
                    printf("error in character encryption");
                }
            }
            else // normal offest
            {
                strcat(string_out, str_i_new);
            }
        }
        else if (strstr(number, str_i_old)) // number offest
        {
            if (strstr(number_last_four, str_i_old)) // loopback offest
            {
                switch (char_i)
                {
                case '6':
                    strcat(string_out, "0");
                    break;
                case '7':
                    strcat(string_out, "1");
                    break;
                case '8':
                    strcat(string_out, "2");
                    break;
                case '9':
                    strcat(string_out, "3");
                    break;
                case '0':
                    strcat(string_out, "4");
                    break;
                default:
                    strcat(string_out, "_");
                }
            }
            else // normal offset
            {
                strcat(string_out, str_i_new);
            }
        }
        else // no offest on special characters
        {
            strcat(string_out, str_i_old);
        }

        // debug log
        //  printf("char is%c__%s___%s\n", char_i, str_i_old, str_i_new);
        //  printf("%s\n\n", string_out);
    }
    return (string_out);
}

// TCP Server based on Beej's code

void sigchld_handler(int s)
{
    int saved_errorno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errorno;
}

/**
 * @brief Get the in IPv4 addr object
 *          Used Beej's code
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
    int sockfd, new_fd; // listen on sock_fd, new connection of fd
    struct addrinfo hints, *serverinfo, *p;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN], buff_out[MAXBUFFER], buff_in[MAXBUFFER];
    int rv;

    // the following variables are for UDP CS connections
    int sockfd_udp_cs;
    struct addrinfo hints_udp_cs, *serverinfo_udp_cs, *p_udp_cs;
    int rv_udp_cs;
    int numbytes_udp_cs;

    // the following variables are for UDP auth connections
    int sockfd_udp_auth;
    struct addrinfo hints_udp_auth, *serverinfo_udp_auth, *p_udp_auth;
    int rv_udp_auth;
    int numbytes_udp_auth;
    char uncrypt_auth_str[150] = "\0", crypt_auth_str[150] = "";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo("localhost", TCP_port, &hints, &serverinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo :%s\n", gai_strerror(rv));
        return 1;
    }

    printf("%d\n", (serverinfo == NULL));
    for (p = serverinfo; p != NULL; p = p->ai_next)
    { // bind to the first result
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(serverinfo);

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind \n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server waiting for connections... \n");

    while (1)
    {
        sin_size = sizeof client_addr;

        // accepting incomming connect
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);

        printf("server: got connection form %s\n", s);
        if (!fork())
        {
            close(sockfd);
            sockfd = -1;

            // start UDP connection to CS server
            memset(&hints_udp_cs, 0, sizeof hints_udp_cs);
            hints_udp_cs.ai_family = AF_INET;
            hints_udp_cs.ai_socktype = SOCK_DGRAM;
            if ((rv_udp_cs = getaddrinfo("localhost", UDP_port_CS,
                                      &hints_udp_cs, &serverinfo_udp_cs)) != 0)
            {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            }

            for (p_udp_cs = serverinfo_udp_cs; p_udp_cs != NULL; p_udp_cs->ai_next)
            {
                if ((sockfd_udp_cs = socket(p_udp_cs->ai_family, p_udp_cs->ai_socktype,
                                         p_udp_cs->ai_protocol)) == -1)
                {
                    perror("udp_client: socket");
                    continue;
                }
                break;
            }

            if (p_udp_cs == NULL)
            {
                fprintf(stderr, "udp_client: failed to create socket\n");
                return 2;
            }
            freeaddrinfo(serverinfo_udp_cs); // CS UDP connection finish

            // start UDP connection to auth server
            memset(&hints_udp_auth, 0, sizeof hints_udp_auth);
            hints_udp_auth.ai_family = AF_INET;
            hints_udp_auth.ai_socktype = SOCK_DGRAM;
            if ((rv_udp_auth = getaddrinfo("localhost", UDP_port_C,
                                           &hints_udp_auth, &serverinfo_udp_auth)) != 0)
            {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_udp_auth));
            }

            for (p_udp_auth = serverinfo_udp_auth; p_udp_auth != NULL; p_udp_auth->ai_next)
            {
                if ((sockfd_udp_auth = socket(p_udp_auth->ai_family, p_udp_auth->ai_socktype,
                                              p_udp_auth->ai_protocol)) == -1)
                {
                    perror("udp_client_auth: socket");
                    continue;
                }
                break;
            }

            if (p_udp_auth == NULL)
            {
                fprintf(stderr, "udp_client_auth: failed to create socket\n");
                return 2;
            }
            freeaddrinfo(serverinfo_udp_auth); // auth UDP connection finish

            if ((numbytes_udp_auth = recv(new_fd, uncrypt_auth_str, MAXBUFFER - 1, 0)) == -1)
            {
                perror("auth recv");
                exit(1);
            }
            uncrypt_auth_str[numbytes_udp_auth] = '\0';
            memset(crypt_auth_str, 0, sizeof crypt_auth_str);
            strcpy(crypt_auth_str, string_encrypt(uncrypt_auth_str));

            if ((numbytes_udp_auth = sendto(sockfd_udp_auth, crypt_auth_str, strlen(crypt_auth_str), 0,
                                            p_udp_auth->ai_addr, p_udp_auth->ai_addrlen)) == -1)
            {
                perror("auth_talker: sendto");
                exit(1);
            }
            printf("talker: sent %d bytes to %s\n", numbytes_udp_auth, "serverC");

            // if ((numbytes_udp_cs = sendto(sockfd_udp_cs, "EE450", strlen("EE450"), 0,
            //                               p_udp_cs->ai_addr, p_udp_cs->ai_addrlen)) == -1)
            // {
            //     perror("cs_talker: sendto");
            //     exit(1);
            // }

            if ((numbytes_udp_cs = sendto(sockfd_udp_cs, crypt_auth_str, strlen(crypt_auth_str), 0,
                                            p_udp_cs->ai_addr, p_udp_cs->ai_addrlen)) == -1)
            {
                perror("auth_talker: sendto");
                exit(1);
            }

            printf("talker: sent %d bytes to %s\n", numbytes_udp_cs, "serverCS");
            close(sockfd_udp_auth);
            close(sockfd_udp_cs);

            close(new_fd);
            exit(0);
        }
    }

    return (0);
};