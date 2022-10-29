/**
 * @file serverCS.c
 * @author Baosheng Chang (changb9@outlook.com)
 * @brief The CS department Server of the project
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
 * @brief the user_node for storing the course information
 * @param course_code the course code of the course
 * @param credit the credit of the course. Only integer
 * @param professor the professor of the course
 * @param lecture_day the date that lecture is held
 * @param course_name the course name
 * @param next the next node in the linked list
 */
struct cs_course_node
{
    char course_code[50]; // need to specify the length of this
    char credit[50];
    char professor[50];   // need to specify the length
    char lecture_day[50]; // need to specify the length
    char course_name[50]; // need to specify the length
    struct cs_course_node *next;
};

struct cs_course_node *head;
struct cs_course_node *current;

void delete_list()
{
    current = head;
    while (current != NULL)
    {
        head = head->next;
        free(current);
        current = head;
    };
};

/**
 * @brief append the new cs course to the front of the linked list
 *
 * @param course_code_in string. the input course code. length less than 50
 * @param credit_in string. the input course credit. length less than 50
 * @param professor_in string. the input professor name. length less than 50
 * @param lecture_day_in string. the input lecture day. length less than 50
 * @param course_name_in string. the input course name. length less than 50
 */
void CS_append_front(char course_code_in[50], char credit_in[10], char professor_in[50],
                     char lecture_day_in[50], char course_name_in[50])
{
    struct cs_course_node *temp_cs_node = (struct cs_course_node *)malloc(sizeof(struct cs_course_node));
    strlcpy(temp_cs_node->course_code, course_code_in, 50);
    strlcpy(temp_cs_node->credit, credit_in, 50);
    strlcpy(temp_cs_node->professor, professor_in, 50);
    strlcpy(temp_cs_node->lecture_day, lecture_day_in, 50);
    strlcpy(temp_cs_node->course_name, course_name_in, 50);
    temp_cs_node->next = head;
    head = temp_cs_node;
};

/**
 * @brief print the entire linked list
 *
 */
void print_all()
{
    struct cs_course_node *temp = head;
    while (temp != NULL)
    {
        current = temp;
        temp = temp->next;
        printf("code:%s|credit:%s|professor:%s|day:%s|name:|%s\n",
               current->course_code, current->credit, current->professor,
               current->lecture_day, current->course_name);
    }
}

/**
 * @brief   find the coresponding category of the course,
 *          and return the result if found.
 *
 * @param course_code string. the course code going to be queried. length < 50
 * @param category string. the category that is going to be eamined. length < 50
 *                 case sentitive
 *                 category that will return value:
 *                      Credir, Professor, Days, CourseName
 * @return char*    return the query result if found.
 *                  if course not found, return "course_not_found"
 *                  if category not found, return "category_not_found"
 */
char *find_course_info(char course_code[50], char category[50])
{
    char *output;
    struct cs_course_node *temp = head;
    while (temp != NULL)
    {
        current = temp;
        temp = temp->next;
        if (strcmp(current->course_code, course_code) == 0)
        {
            if (strcmp(category, "Credit") == 0)
                return (current->credit);
            else if (strcmp(category, "Professor") == 0)
                return (current->professor);
            else if (strcmp(category, "Days") == 0)
                return (current->lecture_day);
            else if (strcmp(category, "CourseName") == 0)
                return (current->course_name);
            else
                return ("category_not_found");
        }
    };
    return ("course_not_found");
};

/**
 * @brief handles query for multiple coruse
 *
 * @param course_code_in string. contains multiple course code, split by whitespace
 * @return char* the string containing all course information.
 */
char *multi_course_query(char *course_code_in)
{
    struct cs_course_node *temp;

    // variables for processing the input string
    char course_code[500];
    char *single_course_code;
    strlcpy(course_code, course_code_in, 500);

    char *course_info_result; // store the result from find_course_info

    char *CS_query_output = (char *)malloc(sizeof(char) * 0);
    int output_length = 0;

    single_course_code = strtok(course_code, " ");
    while (single_course_code != NULL)
    {
        if (strstr(single_course_code, "\n"))
        {
            single_course_code[strlen(single_course_code) - 2] = '\0';
        }
        printf("%s\n", single_course_code);

        //start single course query
        temp = head;
        while (temp != NULL)
        {
            current = temp;
            temp = temp->next;
            if (strcmp(current->course_code, single_course_code) == 0)
            {
                // appending course code
                output_length += strlen(single_course_code) + 2;
                CS_query_output = (char *)realloc(CS_query_output, output_length);
                strcat(CS_query_output, single_course_code);
                strcat(CS_query_output, ":");

                //appending course unit
                course_info_result = find_course_info(single_course_code, "Credit");
                output_length += strlen(course_info_result + 1);
                CS_query_output = (char *)realloc(CS_query_output, output_length);
                strcat(CS_query_output, course_info_result);
                strcat(CS_query_output, ",");

                //appending professor name
                course_info_result = find_course_info(single_course_code, "Professor");
                output_length += strlen(course_info_result + 1);
                CS_query_output = (char *)realloc(CS_query_output, output_length);
                strcat(CS_query_output, course_info_result);
                strcat(CS_query_output, ",");

                //appending days
                course_info_result = find_course_info(single_course_code, "Days");
                output_length += strlen(course_info_result + 1);
                CS_query_output = (char *)realloc(CS_query_output, output_length);
                strcat(CS_query_output, course_info_result);
                strcat(CS_query_output, ",");

                //appending course anme
                course_info_result = find_course_info(single_course_code, "CourseName");
                output_length += strlen(course_info_result + 2);
                CS_query_output = (char *)realloc(CS_query_output, output_length);
                strcat(CS_query_output, course_info_result);
                strcat(CS_query_output, "\n");
            }
        }
        single_course_code = strtok(NULL, " ");
    }
    return CS_query_output;
};

int main()
{
    char cs_course_buffer[500];
    char *code_buffer, *credit_buffer, *professor_buffer,
        *lecture_day_buffer, *course_buffer;
    char *test_result;
    char *multi_query_result;
    FILE *cs_file = fopen("cs.txt", "r");
    int result = -1;

    // read by lan and store the cs course into the linked list
    while (fgets(cs_course_buffer, 500, cs_file) != NULL)
    {
        // process the line
        code_buffer = strtok(cs_course_buffer, ",");
        credit_buffer = strtok(NULL, ",");
        professor_buffer = strtok(NULL, ",");
        lecture_day_buffer = strtok(NULL, ",");
        course_buffer = strtok(NULL, ",");
        if (strstr(course_buffer, "\n"))
        {
            course_buffer[strlen(course_buffer) - 2] = '\0';
        };
        // printf("%s|%s|%s|%s|%s\n", code_buffer, credit_buffer, professor_buffer,
        //        lecture_day_buffer, course_buffer);
        CS_append_front(code_buffer, credit_buffer, professor_buffer,
                        lecture_day_buffer, course_buffer);
    };
    fclose(cs_file);


    print_all();
    test_result = find_course_info("CS100", "Credit");
    printf("%s\n", test_result);
    test_result = find_course_info("CS101", "Credit");
    printf("%s\n", test_result);
    test_result = find_course_info("CS100", "credit");
    printf("%s\n", test_result);
    test_result = find_course_info("CS100", "Professor");
    printf("%s\n", test_result);
    test_result = find_course_info("CS435", "Days");
    printf("%s\n", test_result);
    test_result = find_course_info("CS356", "CourseName");
    printf("%s\n", test_result);

    multi_query_result = multi_course_query("CS100 0 CS310");
    printf("%s", multi_query_result);
    free(multi_query_result);
        //freeing not used pointer


    delete_list();
}