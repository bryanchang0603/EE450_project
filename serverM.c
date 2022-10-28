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

char *string_encrypt(char *string_in)
{
    char *string_out = malloc(sizeof(char) * 50);
    free(string_out);
    string_out = malloc(sizeof(char) * 50);
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
        char_i = string_in[i];
        str_i_new[0] = char_i + 4;
        str_i_old[0] = char_i;
        if (strstr(alphbet, str_i_old))
        {
            if (strstr(alphbet_last_four, str_i_old))
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
            else
            {
                strcat(string_out, str_i_new);
            }
        }
        else if (strstr(number, str_i_old))
        {
            if (strstr(number_last_four, str_i_old))
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
            else
            {
                strcat(string_out, str_i_new);
            }
        }
        else
        {
            strcat(string_out, str_i_old);
        }
        printf("char is%c__%s___%s\n", char_i, str_i_old, str_i_new);
        printf("%s\n\n", string_out);
    }
    return (string_out);
}

int main()
{
    return (0);
};