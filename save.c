#include<stdio.h>
#include"header.h"
#include<signal.h>
int ret=0;
int st;
char *external_commands[152];
Slist *head = NULL;
Slist node;
void signal_handler(int signum)
{
  
  if(signum==SIGINT)
  {
    if(ret==0)
  {
    printf("\n%s\n",prompt);
  }
  }
  if(signum==SIGTSTP)
  {
    if(ret==0)
    {
      printf("\n%s\n",prompt);
    }
    else
    {
        if (insert_at_first(&head,ret,input_string) == FAILURE)
          {
            printf("INFO : Insert first failure\n");
          }
    }
  }
  if(signum==SIGCHLD)
  {
    waitpid(-1,&st,WNOHANG);
  }
}

void scan_input(char *prompt, char *input_string)
{
    //register the signal of sigint tosigint sigtstp
    // fflush(stdout);
    signal(SIGINT,signal_handler);
    signal(SIGTSTP,signal_handler);
    extract_external_commands(external_commands);
    while(1)
    { 
        printf("%s",prompt);
        int st=scanf("%[^\n]",input_string);
        getchar();
        if(st <= 0)
          {
              int c;
              while((c = getchar()) != '\n' && c != EOF);
              continue;
          }
        int len=strlen(input_string);
        if(strncmp(input_string,"PS1=",4)==0)
        {
                if(input_string[len-2]==' ')
                  {
                      printf("Entered prompt contains spaces:\nprompt updation cant hapeen\n");
                      exit(0);
                  }
           //checking if spaces present in between the string 
           if(strchr(input_string+4,' ')!=NULL&&*(strchr(input_string + 4, ' ') + 1) != '\0')
           {
            printf("Entered prompt contains spaces:\nprompt updation cant happen\n");
            //return 0;
           } 
           else
           {
             strcpy(prompt,input_string+4);
             //break;
           }
        }
        else{
       char *command=get_command(input_string);
        int type=check_command_type(command);
        if(type==BUILTIN)
        {
        //we only do three operations pwd,cd,ls;
       
        execute_internal_commands(input_string);    
        }
        else if(type==EXTERNAL)
        {
         // int st;
           ret=fork();
          if(ret>0)
          {
            waitpid(ret,&st,WUNTRACED);
          }
           
          else if(ret==0)
          {
             
            signal(SIGINT,SIG_DFL);
            signal(SIGTSTP,SIG_DFL);
           
            execute_external_commands(input_string);
          }
          
          ret=0;
        }
        else if(type==NO_COMMAND)
        {
          printf("command not found\n");
        }
      }
    }
    
}
