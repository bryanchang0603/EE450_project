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

// TODO add phase 3 for the EE server

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
#define UDP_port_M 24048
#define UDP_port_C 21048
#define UDP_port_CS 22048
#define UDP_port_EE 23048
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
        // printf("%s, %s\n", str_i_old, str_i_new);
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

    // some of the vairables copied for beej's code
    int sockfd, new_fd; // listen on sock_fd, new connection of fd
    struct addrinfo hints, *serverinfo, *p;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1, course_code_index = 0, multi_result_found = 0;
    char s[INET6_ADDRSTRLEN], buff_out[MAXBUFFER], buff_in[MAXBUFFER], temp_buff_in[MAXBUFFER],
        temp_buff_out[MAXBUFFER], buff_out_ordered[MAXBUFFER];
    char username[60] = "";
    char course_code_list[10][10];
    int rv;
    char temp_course_code[10], *temp_course_buff, *username_buff, *temp_category_buff;

    // the following variables are for UDP CS connections
    int numbytes_udp_cs;
    char multi_query_list_cs[MAXBUFFER];

    // the following variables are for UDP EE connections
    int numbytes_udp_ee;
    char multi_query_list_ee[MAXBUFFER];

    // the following variables are for UDP auth connections
    int numbytes_udp_auth;
    char uncrypt_auth_str[150] = "\0", crypt_auth_str[150] = "";

    // the following vairables are for single udp socket
    int sockfd_udp, addr_len;
    struct sockaddr_in auth_addr, cs_addr, ee_addr, main_addr;

    // the tcp socket setup is from beej's code
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // checking serverM client UDP port, used code from Piazza
    unsigned int clientPort;
    struct sockaddr_in clinetAddress;
    bzero(&clinetAddress, sizeof(clinetAddress));
    clinetAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t len = sizeof(clinetAddress);
    char ipAddress[INET_ADDRSTRLEN];

    if ((rv = getaddrinfo("localhost", TCP_port, &hints, &serverinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo :%s\n", gai_strerror(rv));
        return 1;
    }

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

    printf("The main server is up and running.\n");
    // beej's TCP server setup finish

    while (1)
    {
        sin_size = sizeof client_addr;

        // accepting incomming connect
        // copied from beej's code for child TCP server socket
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);

        if (!fork())
        {
            close(sockfd);
            sockfd = -1;

            // setting up single udp socket following geeksforgeeks and beej's code
            if ((sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                perror("socket creation failed");
                exit(EXIT_FAILURE);
            }

            // setting up other server's socket address
            // Idea received from geekforgeek
            memset(&auth_addr, 0, sizeof(auth_addr));
            memset(&cs_addr, 0, sizeof(cs_addr));
            memset(&ee_addr, 0, sizeof(ee_addr));
            memset(&main_addr, 0, sizeof(main_addr));

            auth_addr.sin_port = htons(UDP_port_C);
            auth_addr.sin_family = AF_INET;
            auth_addr.sin_addr.s_addr = INADDR_ANY;
            cs_addr.sin_port = htons(UDP_port_CS);
            cs_addr.sin_family = AF_INET;
            cs_addr.sin_addr.s_addr = INADDR_ANY;
            ee_addr.sin_port = htons(UDP_port_EE);
            ee_addr.sin_family = AF_INET;
            ee_addr.sin_addr.s_addr = INADDR_ANY;
            main_addr.sin_port = htons(UDP_port_M);
            main_addr.sin_family = AF_INET;
            main_addr.sin_addr.s_addr = INADDR_ANY;

            if (bind(sockfd_udp, (struct sockaddr *)&main_addr, sizeof(main_addr)) < 0)
            {
                perror("bind");
                exit(1);
            }
            //checking serverM's UDP  port
            // if (getsockname(sockfd_udp, (struct sockaddr *)&clinetAddress, &len) != 0)
            // {
            //     perror("getsockname");
            // }
            // clientPort = ntohs(clinetAddress.sin_port);
            // printf("%u\n", clientPort);

            // the sendto, send, recv, and recvfrom below are modified based on geekforgeek code and beej's code
            //  the authencation process at server side
            while (1)
            {
            auth_rec:
                // initalize variables to empty
                strcpy(uncrypt_auth_str, "\0");
                memset(crypt_auth_str, 0, sizeof crypt_auth_str);
                memset(temp_buff_in, 0, sizeof temp_buff_in);

                // receive username & password from client
                if ((numbytes_udp_auth = recv(new_fd, uncrypt_auth_str, MAXBUFFER - 1, 0)) == -1)
                {
                    perror("auth recv");
                    exit(1);
                }
                if (numbytes_udp_auth == 0)
                {
                    goto auth_rec;
                }

                // encypt the received password and send to serverC
                uncrypt_auth_str[numbytes_udp_auth] = '\0';
                strcpy(temp_buff_in, uncrypt_auth_str);
                username_buff = strtok(temp_buff_in, ","); // learnt from geekforgeek
                strcpy(username, username_buff);
                printf("The main server received the authentication for %s using TCP over port %s.\n", username, TCP_port);
                strcpy(crypt_auth_str, string_encrypt(uncrypt_auth_str));
                if ((numbytes_udp_auth = sendto(sockfd_udp, crypt_auth_str, strlen(crypt_auth_str), 0,
                                                (const struct sockaddr *)&auth_addr, sizeof(auth_addr))) == -1)
                {
                    perror("auth_talker: sendto");
                    exit(1);
                }
                printf("The main server sent an authentication request to serverC.\n");

                // receive auth result from serverC
                if ((numbytes_udp_auth = recvfrom(sockfd_udp, buff_out, MAXBUFFER - 1, MSG_CONFIRM,
                                                  (struct sockaddr *)&auth_addr, &addr_len)) == -1)
                {
                    perror("recvfrom");
                    exit(1);
                }
                buff_out[numbytes_udp_auth] = '\0';
                printf("The main server received the result of the authentication request from ServerC using UDP over port %d\n", UDP_port_M);

                // send result received from serverC to client
                if (send(new_fd, buff_out, strlen(buff_out), 0) == -1)
                {
                    perror("auth request");
                    exit(0);
                }
                printf("The main server sent the authentication result to the client.\n");
                if (strcmp(buff_out, "pass") == 0)
                {
                    goto auth_success;
                    break;
                }
            };

        auth_success:
            // the query process
            // the sendto, send, recv, and recvfrom below are modified based on geekforgeek code and beej's code
            while (1)
            {
            query_start:
                memset(buff_in, 0, sizeof(buff_in));
                if ((numbytes_udp_cs = recv(new_fd, buff_in, MAXBUFFER - 1, 0)) == -1)
                {
                    perror("auth recv");
                    exit(1);
                }
                if (numbytes_udp_cs == 0)
                {
                    goto auth_rec;
                }
                buff_in[numbytes_udp_cs] = '\0';
                if (strstr(buff_in, " ") == NULL) // single query
                {
                    strcpy(temp_buff_in, buff_in);
                    temp_course_buff = strtok(temp_buff_in, ",");
                    temp_category_buff = strtok(NULL, ",");
                    printf("The main server received from %s to query course %s about %s using TCP over port %s.\n", username, temp_course_buff, temp_category_buff, TCP_port);

                    if (strstr(buff_in, "CS") != NULL) // checking if is CS, then send to serverCS
                    {
                        if ((numbytes_udp_cs = sendto(sockfd_udp, buff_in, strlen(buff_in), 0,
                                                      (const struct sockaddr *)&cs_addr, sizeof(cs_addr))) == -1)
                        {
                            perror("cs_talker: sendto");
                            exit(1);
                        }
                        printf("The main server sent a request to serverCS.\n");

                        // receive single query result from serverCS
                        if ((numbytes_udp_cs = recvfrom(sockfd_udp, buff_out, MAXBUFFER - 1, MSG_CONFIRM,
                                                        (struct sockaddr *)&cs_addr, &addr_len)) == -1)
                        {
                            perror("recvfrom");
                            exit(1);
                        }
                        printf("The main server received the response from serverCS using UDP over port %d.\n", UDP_port_M);
                        buff_out[numbytes_udp_cs] = '\0';

                        // send result to client
                        if (send(new_fd, buff_out, strlen(buff_out), 0) == -1)
                        {
                            perror("auth request");
                            exit(0);
                        }
                        printf("The main server sent the query information to the client.\n");
                    }
                    else if (strstr(buff_in, "EE") != NULL) // checking if is CS, then send to serverCS
                    {
                        if ((numbytes_udp_ee = sendto(sockfd_udp, buff_in, strlen(buff_in), 0,
                                                      (const struct sockaddr *)&ee_addr, sizeof(ee_addr))) == -1)
                        {
                            perror("ee_talker: sendto");
                            exit(1);
                        }
                        printf("The main server sent a request to serverEE.\n");

                        // receive single query result from serverCS
                        if ((numbytes_udp_ee = recvfrom(sockfd_udp, buff_out, MAXBUFFER - 1, MSG_CONFIRM,
                                                        (struct sockaddr *)&ee_addr, &addr_len)) == -1)
                        {
                            perror("recvfrom");
                            exit(1);
                        }
                        printf("The main server received the response from serverCS using UDP over port %d.\n", UDP_port_M);
                        buff_out[numbytes_udp_ee] = '\0';

                        // send result to client
                        if (send(new_fd, buff_out, strlen(buff_out), 0) == -1)
                        {
                            perror("tcp send");
                            exit(0);
                        }
                        printf("The main server sent the query information to the client.\n");
                    }
                    else // course does not contain "CS" or "EE", will not send query to back server
                    {
                        strcpy(buff_out, "course_not_found");
                        if (send(new_fd, buff_out, strlen(buff_out), 0) == -1)
                        {
                            perror("tcp send");
                            exit(0);
                        }
                        printf("course not found sent to client\n");
                    }
                }
                else // multi course query
                {

                    // initalize variables
                    course_code_index = 0;
                    memset(course_code_list, 0, sizeof(course_code_list));
                    memset(multi_query_list_cs, 0, sizeof(multi_query_list_cs));
                    memset(multi_query_list_ee, 0, sizeof(multi_query_list_ee));
                    memset(temp_buff_out, 0, sizeof(temp_buff_out));
                    memset(buff_out, 0, sizeof(buff_out));
                    memset(buff_out_ordered, 0, sizeof(buff_out_ordered));

                    // split received courses to CS course, EE course, other courses
                    temp_course_buff = strtok(buff_in, " ");
                    while (temp_course_buff != NULL)
                    {
                        memset(temp_course_code, 0, sizeof(temp_course_code));
                        strcpy(temp_course_code, temp_course_buff);

                        // store EE course to EE query list
                        if (strstr(temp_course_code, "EE") != NULL)
                        {
                            if (strstr(multi_query_list_ee, temp_course_code) == NULL)
                            {
                                strcat(multi_query_list_ee, temp_course_code);
                                strcat(multi_query_list_ee, " ");

                                // for result ordering in later stage
                                strcpy(course_code_list[course_code_index], temp_course_buff);
                                course_code_index += 1;
                            }
                        }

                        // store CS course to CS query list
                        else if (strstr(temp_course_code, "CS") != NULL)
                        {
                            if (strstr(multi_query_list_cs, temp_course_code) == NULL)
                            {
                                strcat(multi_query_list_cs, temp_course_code);
                                strcat(multi_query_list_cs, " ");

                                // for result ordering in later stage
                                strcpy(course_code_list[course_code_index], temp_course_buff);
                                course_code_index += 1;
                            }
                        }
                        // invalid course category, append course_not_found prompt to buffer
                        else
                        {
                            strcat(temp_buff_out, "course did not found:");
                            strcat(temp_buff_out, temp_course_code);
                            strcat(temp_buff_out, "\n");
                            // for result ordering in later stage
                            strcpy(course_code_list[course_code_index], temp_course_buff);
                            course_code_index += 1;
                        }
                        temp_course_buff = strtok(NULL, " ");
                    }

                    printf("%s | %s\n", multi_query_list_cs, multi_query_list_ee);

                    // send one udp message to serverCS and one udp message to serverEE
                    //  received result will be stored in buff_out
                    if ((numbytes_udp_cs = sendto(sockfd_udp, multi_query_list_cs, strlen(multi_query_list_cs), 0,
                                                  (const struct sockaddr *)&cs_addr, sizeof(cs_addr))) == -1)
                    {
                        perror("cs_talker: sendto");
                        exit(1);
                    }
                    printf("cs_talker: sent %d bytes to %s\n", numbytes_udp_cs, "serverCS");

                    if ((numbytes_udp_cs = recvfrom(sockfd_udp, buff_out, MAXBUFFER - 1, MSG_CONFIRM,
                                                    (struct sockaddr *)&cs_addr, &addr_len)) == -1)
                    {
                        perror("cs_talker: recvfrom");
                        exit(1);
                    }
                    printf("CS group result received\n");
                    strcat(buff_out, temp_buff_out);

                    if ((numbytes_udp_ee = sendto(sockfd_udp, multi_query_list_ee, strlen(multi_query_list_ee), 0,
                                                  (const struct sockaddr *)&ee_addr, sizeof(cs_addr))) == -1)
                    {
                        perror("ee_talker: sendto");
                        exit(1);
                    }
                    printf("ee_talker: sent %d bytes to %s\n", numbytes_udp_ee, "serverEE");

                    if ((numbytes_udp_ee = recvfrom(sockfd_udp, temp_buff_out, MAXBUFFER - 1, MSG_CONFIRM,
                                                    (struct sockaddr *)&ee_addr, &addr_len)) == -1)
                    {
                        perror("ee_talker: recvfrom");
                        exit(1);
                    }
                    printf("EE group result received\n");

                    strcat(buff_out, temp_buff_out);
                    buff_out[strlen(buff_out)] = '\0';
                    // ordering the buff_out based on buff_in
                    for (int i = 0; i < 10; i++)
                    {
                        strcpy(temp_buff_out, buff_out);
                        if (strlen(course_code_list[i]) != 0)
                        {

                            temp_course_buff = strtok(temp_buff_out, "\n");
                            while (temp_course_buff != NULL)
                            {
                                if (strstr(temp_course_buff, course_code_list[i]) != NULL)
                                {
                                    strcat(buff_out_ordered, temp_course_buff);
                                    strcat(buff_out_ordered, "\n");
                                    multi_result_found = 1;
                                }
                                temp_course_buff = strtok(NULL, "\n");
                            }
                            if (!(multi_result_found))
                            {
                                printf("adding missing course%s\n", course_code_list[i]);
                                strcat(buff_out_ordered, "Didn't find the course: ");
                                strcat(buff_out_ordered, course_code_list[i]);
                                strcat(buff_out_ordered, "\n");
                            }
                            multi_result_found = 0;
                        }
                    }

                    // sending ordered string to user
                    if (send(new_fd, buff_out_ordered, strlen(buff_out_ordered), 0) == -1)
                    {
                        perror("TCP group query send");
                        exit(0);
                    }
                    printf(" group query sent to client\n");
                }
            };
            close(sockfd_udp);
            close(new_fd);
            printf("socket closed");
            exit(0);
        }
    }

    return (0);
};