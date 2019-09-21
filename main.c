
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;
void run(instruction * instr_ptr);
void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
void piping(char * before,int c_pipe);
int ErrorCheck( instruction * instr_ptr );
void OutRedir(char * before);
void InRedir(char * before);
void my_execute(char **cmd);
char *getPrompt();
char *PATHRes(char *cmd);

int main()
{
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;


	while (1) {

		printf("%s ", getPrompt());
    int valid = 0; //checks if the command is valid
    int c_pipe = 0; //tracks the amount of pipes
    int c_input = 0;
    int c_output = 0;
    int c_fslash = 0;
    int c_and = 0;
      char * after;
      char *before;
		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

			int i;
			int start = 0;

			for (i = 0; i < strlen(token); i++)
      {
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&')
        {
					if (i-start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}
          char specialChar[2];
          specialChar[0] = token[i];
          specialChar[1] = '\0';
          addToken(&instr,specialChar);
          start = i + 1;
           if (token[i] == '|')
          c_pipe++;
          else if (token[i] == '>')
            c_input++;
        else if (token[i] == '<')
          c_output++;
        else if (token[i] == '&')
          c_and++;
				}
			}

			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);
			}
      //printf("Validity:%d",valid);
			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;


		} while ('\n' != getchar());    //until end of line is reached
    valid=ErrorCheck(&instr);
    printf("Validity:%d\n",valid);
		if(valid)
		{
				run(&instr);
		}
		else
		{
		printf("False\n");
		}

		addNull(&instr);
		//printTokens(&instr);
		clearInstruction(&instr);
	}

	return 0;
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
	instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++) {
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}
void run(instruction * instr_ptr)
{
	int i;
	char *cmd1;
	char *cmd2;

	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if ((instr_ptr->tokens)[i] != NULL)
			{
				if(strcmp(instr_ptr->tokens[i], "|")==0)
				{
						// strcpy(cmd1,instr_ptr->tokens[i-1]);
						// strcpy(cmd2,instr_ptr->tokens[i+1]);
								printf("Hey2\n");
					}
					else if (strcmp(instr_ptr->tokens[i], "<")==0)
					{
					// strcpy(cmd1,instr_ptr->tokens[i-1]);
					// strcpy(cmd2,instr_ptr->tokens[i+1]);
							//printf("hey brah %s:",instr_ptr->tokens[i+2]);
							// if (instr_ptr->tokens[i+2] == NULL)
							// {
							// 	printf("yay\n");
							// }
							// else
							// {
							printf("NO\n");
						}

					}
					 if (strcmp(instr_ptr->tokens[i], ">")==0)
					{
					// strcpy(cmd1,instr_ptr->tokens[i-1]);
					// strcpy(cmd2,instr_ptr->tokens[i+1]);
					}
					else if (strcmp(instr_ptr->tokens[i], "&")==0)
					{
						// strcpy(cmd1,instr_ptr->tokens[i-1]);
						// strcpy(cmd2,instr_ptr->tokens[i+1]);
					}
					else
					{
						// printf("Enter %s\n",instr_ptr->tokens[i]);
					}

					}
}
int ErrorCheck( instruction * instr_ptr )
{
    // printf("Entranace%c\n","chr");
  int i; //for loop
  for (i = 0; i < instr_ptr->numTokens; i++)
   {
    if ((instr_ptr->tokens)[i] != NULL)
    {
      if (i==0&&(strcmp(instr_ptr->tokens[i], "|")==0
      ||strcmp(instr_ptr->tokens[i], "<")==0||strcmp((instr_ptr->tokens)[i], ">")==0
      ||strcmp((instr_ptr->tokens)[i], "&")==0))
      {
        return 0;
      }
     else if ((strcmp((instr_ptr->tokens)[i], "|")==0 ||strcmp((instr_ptr->tokens)[i], "<")==0||strcmp((instr_ptr->tokens)[i], ">")==0||strcmp((instr_ptr->tokens)[i], "&")== 0))
     {
				int j=i+1;
        if(instr_ptr->tokens[j]!=NULL)
         {
          if ((strcmp((instr_ptr->tokens)[j], "|")==0 ||strcmp((instr_ptr->tokens)[j], "<")==0||strcmp((instr_ptr->tokens)[j], ">")==0||strcmp((instr_ptr->tokens)[j], "&")== 0))
          {
          return 0;
        }
        }
        else
        {
          return 0;
        }
      }
  	}
    else
    {
    return 0;

  }
  }
return 1;
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}
void InRedir(char * before)
{
	int fd= open(before,O_RDONLY);
	if(fork()==0)
	{
		close (STDIN_FILENO);
		dup(fd);
		close(fd);
  my_execute
	}
	else
	{
		close (fd);
	}
}
void OutRedir(char * before)
{
	int fd=open(before,O_RDONLY|O_CREAT|O_TRUNC);
	if (fork()==0)
	{
		close(STDOUT_FILENO);
		dup(fd);
		close(fd);
    //execute
    exit(fd);
	}
	else
	{
		close(fd);
	}
}

void piping(char * before, int c_pipe)
{
	int fd[2];

	if (fork()==0)
	{
    if(c_pipe==1) //single pipeline
  	{
		pipe(fd);
		if(fork()==0)
		{
			close (STDOUT_FILENO);
			dup(fd[1]);
			close (fd[0]);
			close (fd[1]);
      //execute
		}
		else
		{
			close(STDIN_FILENO);
			dup(fd[0]);
			close (fd[0]);
			close (fd[1]);
      //execute
		}
  }
	else if (c_pipe>1) //multipiping
	{
			pipe(fd);
			if(fork()==0)
			{
        close (STDOUT_FILENO);
        dup(fd[0]);
        close (fd[0]);
        close (fd[1]);
        if(fork()==0)
        {
          close (STDOUT_FILENO);
          dup(fd[0]);
          close (fd[0]);
          close (fd[1]);
          //execute
        }
        else
        {
          close(STDIN_FILENO);
          dup(fd[0]);
          close (fd[0]);
          close (fd[1]);
        }
			}
			else
			{
				close(STDIN_FILENO);
				dup(fd[0]);
				close (fd[0]);
				close (fd[1]);
        //execute
			}
		}
  }
	else
	{
    waitpid(NULL);

	}
}
void my_execute(char **cmd)
{
    //cmd[] ="/bin/ls";
    int status;
    pid_t pid = fork();
    if(pid == -1){
        //error
        exit(1);
    } else if(pid == 0){

        //child process
        execv(cmd[0], cmd);
        printf("Problem executing %s\n", cmd[0]);
        exit(1);
    } else{

        waitpid(pid, &status, 0);
    }


}
char *getPrompt() {
	//test with CD command
	char *prompt = malloc(50);
	//memset(prompt, 0, 150);

	char *user = malloc(50);
	//memset(user, 0, 25);
	user = getenv("USER");

	char *hostname = malloc(50);
	//memset(hostname, 0, 50);
	gethostname(hostname, 50);


	char *dir = malloc(50);
	//memset(dir, 0, 25);

	dir = getenv("PWD");


	strcat(prompt, user);
	strcat(prompt, "@");
	strcat(prompt, hostname);
	strcat(prompt, ":");
	strcat(prompt, dir);
	strcat(prompt, ">");

	return prompt;

}
char *redirectChars = "<>|&";

char *PATHRes(char *cmd){

    printf("%s", cmd);
    char *token;
    char *p = getenv("PATH");
    int length = strlen(p) + 1;
    char *PATH = (char *)malloc(length * sizeof(char));

    //char **pathList;

    strcpy(PATH, p);
    strcat(PATH, ":\0");
    //make copy of PATH to work on
    //char **path = (char **)malloc(20 * sizeof(char *));
    char *buffer = (char *)malloc((length + 20) * sizeof(char));
    int x = 0;

    token = strtok (PATH,":");
    while (token != NULL)
    {

        //printf ("%s\n",token);

        //path[x] = token;
        x++;

//        strcat(buffer, token);
//        strcat(buffer, "/");
//        strcat(buffer, cmd);

        sprintf(buffer, "%s/%s", token, cmd);
         //printf("buffer: %s\n", buffer);

        //memset(buffer, 0, strlen(buffer));

        if(access(buffer, X_OK) == 0){
           //if you dont find it, memset it. Otherwise just use free
           // memset(buffer, 0, strlen(buffer));
            printf("buffer: %s\n", buffer);
            printf("token: %s\n", token);

            return buffer;

        } else {buffer[0] = 0;}

       token = strtok (NULL, ":");
    }

    printf("Command not found");
    return NULL;

}
