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

/**
 * @brief The node for storing the user information.
 * linked list will be used for storing user information
 *
 */
struct node
{
    char username[50];
    char password[50];
    struct node *next;
};

struct node *head;
struct node *current;

/**
 * @brief append the new node to the begining of the linked list
 *
 * @param username_in String max_len = 50 input username.
 * @param password_in String max_len = 50 input password.
 */
void append_front(char username_in[50], char password_in[50])
{
    struct node *tempNode = (struct node *)malloc(sizeof(struct node));
    strlcpy(tempNode->username, username_in, 50);
    strlcpy(tempNode->password, password_in, 50);
    tempNode->next = head;
    head = tempNode;
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
    struct node *temp = head;
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
int check_username(char *username_in, char *password_in)
{
    while (head != NULL)
    {
        current = head;
        head = head->next;
        if (strcmp(current->username, username_in) == 0)
        {
            if (strcmp(current->password, password_in) == 0)
            {
                return (1); // pass
            }
            else
            {
                return (2); // password mismatch
            };
        }
    };
    return (0); // user not found
};

int main()
{
    char cred_buffer[200];
    char *username_buff, *password_buff;
    FILE *cred_file = fopen("cred.txt", "r");
    int result = -1;

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
        append_front(username_buff, password_buff);
    }
    print_all();
    result = check_username("eqfiav", "Xl!v7si8w");
    printf("%d \n", result);
    delete_list();
    fclose(cred_file);
}