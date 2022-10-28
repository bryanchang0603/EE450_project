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

int main(){
    char cred_buffer[100] = {};
    FILE* cred_file = fopen("cred.txt", "r");
    while (fgets(cred_buffer, 100, cred_file) != NULL){
        printf("%s", cred_buffer);
    }
    fclose(cred_file);
}