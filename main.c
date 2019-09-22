
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
typedef struct
{
	char * name[10];
	char * instruction[10];
} alias;
void run(instruction * instr_ptr, int c_alias,  alias * name);
void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
void piping(instruction * instr_ptr,int c_pipe);
int ErrorCheck( instruction * instr_ptr );
void OutRedir(instruction *insr_ptr,int i);
void InRedir(instruction *insr_ptr,int i);
void my_execute(char **cmd);
char *getPrompt();
char *PATHRes(char *cmd);
char*getEnviornment(const char*name);
char *redirectChars = "<>|&";


int main()
{
	alias store_alias;
	//  for(a;a<11;a++)
	//  {
	// 		 store_alias.name[a]="";
	// 		 store_alias.instruction[a]="";
	// }
	char* token = NULL;
	char* temp = NULL;
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	int c_alias=0;
	while (1) {
		printf("%s ", getPrompt());
    int valid = 0; //checks if the command is valid
    int c_pipe = 0; //tracks the amount of pipes
			int correct=0;
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
           if (token[i] == '|') // checks if it is a multiple pipeline
          c_pipe++;
        //   else if (token[i] == '>')
        //     c_input++;
        // else if (token[i] == '<')
        //   c_output++;
        // else if (token[i] == '&')
        //   c_and++;
				// }

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
    valid=ErrorCheck(&instr); //does the errorchecking
    printf("Validity:%d\n",valid);
		if(valid) //checks if it passed the error checking
		{
			 correct++;
				run(&instr,c_alias,&store_alias);
				c_alias++;
		}
		else
		{
		printf("Invalid Command\n");
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
void run(instruction * instr_ptr, int c_alias, alias * ali)
{
	int i;

	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if ((instr_ptr->tokens)[i] != NULL)
			{
				if(strcmp(instr_ptr->tokens[i], "|")==0)
				{
					 piping(&instr_ptr,i);
					}
					else if (strcmp(instr_ptr->tokens[i], "<")==0)
					{
									 InRedir(&instr_ptr,i);
					// strcpy(cmd1,instr_ptr->tokens[i-1]);
					// strcpy(cmd2,instr_ptr->tokens[i+1]);

						}
					else if (strcmp(instr_ptr->tokens[i], ">")==0)
					{
							 OutRedir(&instr_ptr,i);
					// strcpy(cmd1,instr_ptr->tokens[i-1]);
					// strcpy(cmd2,instr_ptr->tokens[i+1]);
					}
					else if (strcmp(instr_ptr->tokens[i], "&")==0)
					{
					}
					else if (strcmp(instr_ptr->tokens[i], "alias")==0)
					{
						char *string;
						string=instr_ptr->tokens[i+1];
  					char *p,*q;
  					p = strtok (string,"=");

  					while (p!= NULL)
  					{
    				printf ("%s\n",p);
    			  q= strtok (NULL, "'");
						printf ("%s\n",q);
					printf ("count%d\n",c_alias);
						break;
  					}
						if(c_alias+1<11)
						{
							strcpy(ali->name[c_alias],p);
									strcpy(ali->instruction[c_alias],q);
										printf ("Name%s\nInstruction%s\n",ali->name[c_alias],ali->instruction[c_alias]);
						}
						else
						{
							printf("Alias is full\n");
						}
						// printf("hEEEERRE");
						// char * token=strtok(instr_ptr->tokens[i+1], "=");
						// char * token2=strtok(instr_ptr->tokens[i+1], "'");
						// char * token3=strtok(instr_ptr->tokens[i+1], "'");
						// printf("First:%s\nSecond:%s\nThird:%s\n",token,token2,token3 );
					}
						if (strcmp(instr_ptr->tokens[i], "echo")==0&&instr_ptr->tokens[i+1] != NULL)
							{
								;
								// int k=1;
								// 	for(k;k<instr_ptr->numTokens;k++)
								// 	{
								// 		if ((instr_ptr->tokens)[k] != NULL)
								// 		{
								//
 							 	// 		printf("%s\n",getEnviornment((instr_ptr->tokens)[k]));
								// 		}
								//
								// 	}
							}
							else if(instr_ptr->tokens[i+1] == NULL)
							{
								printf("\n"); //if you type echo, it shows  a new line
							}

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
    if ((instr_ptr->tokens)[i] != NULL) //checks if null
    {
      if (i==0&&(strcmp(instr_ptr->tokens[i], "|")==0
      ||strcmp(instr_ptr->tokens[i], "<")==0||strcmp((instr_ptr->tokens)[i], ">")==0
      )) //if the first input is valid
      {
				  if ((instr_ptr->tokens)[1] != NULL)
					{
						if(strcmp(instr_ptr->tokens[i], "&")==0&&(strcmp(instr_ptr->tokens[i+1], "|")==0
	      			||strcmp(instr_ptr->tokens[i+1], "<")==0||strcmp((instr_ptr->tokens)[i+1], ">")==0))
							return 0;
						}
        	return 0;
      }
     else if ((strcmp((instr_ptr->tokens)[i], "|")==0 ||strcmp((instr_ptr->tokens)[i], "<")==0||strcmp((instr_ptr->tokens)[i], ">")==0)) //||strcmp((instr_ptr->tokens)[i], "&")==0)
     {
				int j=i+1;
        if(instr_ptr->tokens[j]!=NULL)// checking if the next input is valid or not
         {
          if ((strcmp((instr_ptr->tokens)[j], "|")==0 ||strcmp((instr_ptr->tokens)[j], "<")==0||strcmp((instr_ptr->tokens)[j], ">")==0)) //||strcmp((instr_ptr->tokens)[j], "&")==0)
          {
          return 0; //its false
        }
        }
				// else if (strcmp(instr_ptr->tokens[i], "&")==0&&strcmp(instr_ptr->tokens[j], "&")!=0)
				// {
				// 	return 1;
				// }
        else
        {
          return 0; //false
        }
      }
  	}
    else
    {
    return 0;
  }
  }
return 1;//true
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
void InRedir(instruction * instr_ptr, int i)
{
	int fd= open(instr_ptr->tokens[i+1],O_RDONLY);
	if(fork()==0)
	{
		close (STDIN_FILENO);
		dup(fd);
		close(fd);
   my_execute(instr_ptr->tokens);
	}
	else
	{
		close (fd);
	}
}
void OutRedir(instruction * instr_ptr,int i)
{
	int fd=open(instr_ptr->tokens[i+1],O_RDONLY|O_CREAT|O_TRUNC);
	if (fork()==0)
	{
		close(STDOUT_FILENO);
		dup(fd);
		close(fd);
    my_execute(instr_ptr->tokens);
	}
	else
	{
		close(fd);
	}
}

void piping(instruction * instr_ptr, int c_pipe)
{
	int fd[2];
	int status;
	pid_t pid = fork();
	if (pid==0)
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
      my_execute(instr_ptr->tokens);
		}
		else
		{
			close(STDIN_FILENO);
			dup(fd[0]);
			close (fd[0]);
			close (fd[1]);
  		my_execute(instr_ptr->tokens);
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
      waitpid(pid, &status, 0);

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
	strcat(prompt, " >");

	return prompt;

}


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
char*getEnviornment(const char*name)
{

    char *v = NULL;
    char *result = (char *)malloc(100 * sizeof(char));

    if (name == NULL)
    {
        return NULL;
    }

    else {
        if (strcmp(name, "$PATH") == 0){
            v = getenv( "PATH" );

        }
        else if (strcmp(name, "~") == 0){
            v = getenv( "HOME" );

        }
        else if (strcmp(name, "$USER") == 0){
            v = getenv( "USER" );

        }
        else if (strcmp(name, "$SHELL") == 0){
            v = getenv( "SHELL" );

        }
        else if (strcmp(name, "$PWD") == 0){
            v = getenv( "PWD" );

        }

    }
    strcpy(result, v);
    return  result;
}
