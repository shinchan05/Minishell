#include"header.h"
#include <sys/wait.h>
//char *external_commands[152];
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","fg","bg","jobs", NULL};
char cmd[100];	
extern int ret;
extern int st;
extern Slist node;
//Slist *head = NULL;
char *get_command(char *input_string)
{
  int i;
  for( i=0;input_string[i]!=' '&& input_string[i] != '\0';i++)
  {
    cmd[i]=input_string[i];
  }
  cmd[i]='\0';
  return cmd;
}
int check_command_type(char *command)
{
  for(int i=0;builtins[i]!=NULL;i++)
  {
    if(strcmp(builtins[i],command)==0)
    {
      return BUILTIN;
    }
  }
  for(int i=0;external_commands[i]!=NULL;i++)
  {
    if(strcmp(external_commands[i],command)==0)
    {
      return EXTERNAL;
    }
  }
  return NO_COMMAND;
}
void extract_external_commands(char **external_commands)
{
   FILE *fp;
   fp=fopen("external.txt","r");
   char buf[50];
   int i=0;
   while(fscanf(fp,"%s",buf)!=EOF)
   {
    external_commands[i]=malloc(strlen(buf)+1);
    strcpy(external_commands[i],buf);
    i++;
   }
   fclose(fp);
}
//size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
void execute_internal_commands(char *input_string)
{
  char buf[50];
   //we only do it for 3 cmds
   if(strcmp(input_string,"exit")==0)
   {
    exit(0);
   }
   else if(strcmp(input_string,"pwd")==0)
   {
    getcwd(buf,50);
    printf("%s\n",buf);
   }
   else if(strncmp(input_string,"cd",2)==0)
   {
     chdir(input_string+3);
     getcwd(buf,50);
    printf("%s\n",buf);
   }
   else if(strncmp(input_string,"echo",4)==0)
   {
    if(strcmp(input_string+5,"$$")==0)
        {
          printf("%d\n",getpid());
        }
        else if(strcmp(input_string+5,"$?")==0)
        {
          if(WIFEXITED(st))
          {
            printf("%d\n",WEXITSTATUS(st));
          }
          else if(WIFSIGNALED(st))
          {
              printf("%d\n", WTERMSIG(st));
          }
          else if(WIFSTOPPED(st))
        printf("%d\n", WSTOPSIG(st));
        }
        else if(strcmp(input_string+5,"$shell")==0)
        {
          printf("%s\n",getenv("SHELL"));
        }
    }
   else if(strcmp(input_string,"jobs")==0)
   {

        	print_list(head);
   }
   else if(strcmp(input_string,"fg")==0)
   {
    print_first(head);
    signal(SIGINT, SIG_IGN);
    kill(node.pid,SIGCONT);
    ret=0;
    waitpid(node.pid,&st,WUNTRACED);
    signal(SIGINT, SIG_DFL); 
    sl_delete_first(&head);
   }
   else if(strcmp(input_string,"bg")==0)
   {
      
        signal(SIGCHLD,signal_handler);
        kill(node.pid,SIGCONT);
        sl_delete_first(&head);
    }
}
void execute_external_commands(char *input_string)
{
  char arr[10][20];   // 2D array to store words
    char *argv[10];     // pointer array for execvp

    int row = 0, col = 0;

    for(int i = 0; input_string[i] != '\0'; i++)
    {
        if(input_string[i] == ' ')
        {
            arr[row][col] = '\0';
            row++;
            col = 0;
        }
        else
        {
            arr[row][col++] = input_string[i];
        }
    }

    arr[row][col] = '\0';   // last word
    int words = row + 1;

    for(int i = 0; i < words; i++)
    {
        argv[i] = arr[i];
    }

    argv[words] = NULL;
    if(checkpipe(input_string)==0)
    {
      if(argv[0] != NULL)
      {
          execvp(argv[0], argv);
      }
    }
    else//pipe present
    {
      //npipe logic
      int pos[10];
        pos[0] = 0;
        int ind = 1;
      int i;
        for(i = 0; argv[i] != NULL; i++)
        {
            if(strcmp(argv[i], "|") == 0)
            {
                argv[i] = NULL;
                pos[ind] = i + 1;
                ind++;
            }
        }

        int fd[2];

        for(i = 0; i < ind; i++)
        {
            if(i != ind - 1)
            {
                pipe(fd);
            }
            int pid = fork();
			if (pid > 0)
            {
                if(i != ind - 1)
                {
                    dup2(fd[0], 0);
                    close(fd[0]);
                    close(fd[1]);
                }
            }

            else if(pid == 0) 
            {
                if(i != ind - 1)
                {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                }

                execvp(argv[pos[i]], argv + pos[i]);
                perror("execvp");
                exit(0);
            }
            
        }

        for(i = 0; i < ind; i++)
        {
            wait(NULL);
        }   
    }
    exit(0);
}
int checkpipe(char *input)
{
  for(int i=0;input[i]!='\0';i++)
  {
    if(input[i]=='|')
    {
      return 1;//pipe yes
    }
    
  }
  return 0;//no pipe
}
int insert_at_first(Slist **head, int pid,char* str)
{
  /* Creating the new node */
	Slist *new = malloc(sizeof(Slist));

	/* Check whether new node created or not */
	if (new == NULL)
	{
		return FAILURE;
	}

	/* Fill the parts of the node */
	new->pid = pid;
	new->link = NULL;
  strcpy(new->str,input_string);
	/* If list is empty */
	if (*head == NULL)
	{
		/*If *head is empty then create the first node */
		*head = new;
		return SUCCESS;
	}
	else
	{
		/* *head is not empty then store value i.e link in the temp variable */
		Slist *temp = *head;

		/* Stroring that new link in the head inserting the element at first */
		*head = new;

		/* Creating the link to next data */
		new -> link = temp;
	}
	return SUCCESS;
}
void print_list(Slist *head)
{
	if (head == NULL)
	{
		printf("INFO : List is empty\n");
	}
    else
    {
	    while (head)		
	    {
		    //printf("%d -> ", head -> pid);
        printf("%s\n",head->str);
		    head = head -> link;
	    }
	   // printf("NULL\n");
    }
}

int sl_delete_first(Slist **head)
{
if(*head==NULL)
{
    return FAILURE;
}
Slist *temp=*head;

*head=temp->link;

free(temp);
temp=NULL;
return SUCCESS;
}

void print_first(Slist *head)
{
 if (head == NULL)
	{
		printf("INFO : List is empty\n");
	}
    else
    {
      printf("%s\n",head->str);
    } 
}