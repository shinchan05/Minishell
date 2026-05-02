#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 extern char *external_commands[152];
 extern char input_string[25];
 extern char prompt[25];
 
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3
#define SUCCESS 0
#define FAILURE -1
extern char *external_commands[152];
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
typedef int data_t;
typedef struct node
{
	int  pid;
    char str[100];
	struct node *link;
}Slist;
extern Slist *head;
void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);

void copy_change(char *prompt, char *input_string);

int sl_delete_first(Slist **head);
int insert_at_first(Slist **head, int pid,char* str);
int check_command_type(char *command);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
void execute_external_commands(char *input_string);
int checkpipe(char *input);
void print_list(Slist *head);
void print_first(Slist *head);
#endif