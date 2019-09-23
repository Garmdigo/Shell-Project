
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
// typedef struct
// {
// 	char * name[10];
// 	char * instruction[10];
// } alias;
void run(instruction * instr_ptr, int *c_alias, char *store_name[],	char *store_instruction[]);
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
void echo(const char*name);
char *redirectChars = "<>|&";


int main()
{
	// alias store_alias;
	// store_alias = (struct alias *) malloc(sizeof(struct alias));
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
			char *store_name[10];
			char *store_instruction[10];

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
		if(valid) //checks if it passed the error checking
		{
			 correct++;
				run(&instr,&c_alias,store_name,store_instruction);
		}
		else
		{
		printf("Invalid Command\n");
		}
		addNull(&instr);
		//printTokens(&instr);
		clearInstruction(&instr);
	}
	// free(store_alias);

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
void run(instruction * instr_ptr, int *c_alias,	char *store_name[], char *store_instruction[])
{
	int i;
	int counter = *c_alias;

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
					else if (strcmp(instr_ptr->tokens[i], "alias")==0)
					{
							if(instr_ptr->tokens[i+1]!=NULL)
							{
								char *string;
								string=instr_ptr->tokens[i+1];
								char *p,*q;
								int a=0;
								int equal=0;
								int q1=0;
								int count=0;
								int size=strlen(string);
									for(a;a<size;a++)
									{
										if(string[a] == '=')
											equal=1;
										if(string[a] == '\'')
										{
											count++;
											if(count%2==0)
											q1=1;
										}
									}
									if(q1==1&&counter<=10)
									{
										p = strtok (string,"="); //gets the name
    			  				q= strtok (NULL, "'"); //gets instruction
										if(counter==0)
										{
										store_instruction[counter] = (char *) malloc((strlen(p) + 1)*sizeof(char));
												store_name[counter] = (char *) malloc((strlen(q) + 1)*sizeof(char));
											strcpy(store_name[counter],p);
												strcpy(store_instruction[counter],q);
												int temp=counter+1;
												*c_alias=temp;
											}
											else if (counter>0&&counter+1<=10)
											{
												int t;
												bool exist=0;
												for (t=0;t<counter;t++)
												{
													if(strcmp(p,store_name[t])==0)
													{
														printf("Alias already exist\n");
														exist=1;
														break;
													}
												}
												if(exist==0)
												{
													store_instruction[counter] = (char *) malloc((strlen(p) + 1)*sizeof(char));
														store_name[counter] = (char *) malloc((strlen(q) + 1)*sizeof(char));
													strcpy(store_name[counter],p);
														strcpy(store_instruction[counter],q);
														int temp=counter+1;
														*c_alias=temp;
												}
											}
										}
										else
											printf("Error: Invalid command\n");
										}
					else 	if(instr_ptr->tokens[i+1]==NULL)
					printf("Invalid command\n");

					}
					else if (strcmp(instr_ptr->tokens[i], "unalias")==0)
				{
						if(instr_ptr->tokens[i+1]!=NULL)
						{
					if(counter==0)
					printf("Error: Alias list is empty\n");
					else
					{
						int q=0;
						if(instr_ptr->tokens[q+1]!=NULL)
						{
						for (q; q < 10; q++)
						{
							if (instr_ptr->tokens[q+1]!=NULL&& strcmp(store_name[q],instr_ptr->tokens[q+1]) == 0){
								strcpy(store_name[q] ,"");
								strcpy(store_instruction[q],"");
								int temp=counter--;
								*c_alias=temp;
								printf("Alias has been removed\n");
								break;
							}
							}

					}
				}
			}
			else
			printf("Error: no alias was selected\n");
				}
					else	if (strcmp(instr_ptr->tokens[i], "echo")==0&&instr_ptr->tokens[i+1] != NULL)
							{
										int k=i+1;

									for(k;k<instr_ptr->numTokens;k++)
									echo(instr_ptr->tokens[k]);

							}
							else if(instr_ptr->tokens[i+1] == NULL&&strcmp(instr_ptr->tokens[i], "echo")==0)
							{
								printf("\n"); //if you type echo, it shows  a new line
							}
							else
							{
										int t;
										int IsAlias=0;
										for (t=0;t<counter;t++)
										{
											if(strcmp(instr_ptr->tokens[i],store_name[t])==0)
											{
												strcpy(instr_ptr->tokens[i],store_instruction[t]);
												IsAlias=1;
												break;
											}
										}
										if(IsAlias==0&&strcmp(instr_ptr->tokens[i], "alias")==0)
										printf("Invalid command\n");


					// 	else
					// 	{
					//
					// 		if(instr_ptr->tokens[i-1] != NULL&&strcmp(instr_ptr->tokens[i-1], "alias")!=0)
					// 		{
					// 		int t;
					// 		int IsAlias=0;
					// 		for (t=0;t<counter;t++)
					// 		{
					// 			if(strcmp(instr_ptr->tokens[i],store_name[t])==0)
					// 			{
					// 				strcpy(instr_ptr->tokens[i],store_instruction[t]);
					// 				printf("%s",instr_ptr->tokens[i]);
					// 				IsAlias=1;
					// 				break;
					// 			}
					// 		}
					// 		if(IsAlias==0)
					// 		printf("Invalid comman1d\n");
					//
					// 		printf("Invalid comma2212n1d\n");
					// 	}
					// 	else
					// 	{
					// 		if(instr_ptr->numTokens==1)
					// 		{
					// 		int t;
					// 		int IsAlias=0;
					// 		for (t=0;t<counter;t++)
					// 		{
					// 			if(strcmp(instr_ptr->tokens[i],store_name[t])==0)
					// 			{
					// 				strcpy(instr_ptr->tokens[i],store_instruction[t]);
					// 				printf("%s",instr_ptr->tokens[i]);
					// 				IsAlias=1;
					// 				break;
					// 			}
					// 		}
					// 		if(IsAlias==0)
					// 		printf("Invalid comman1d\n");
					//
					// 					printf("Invalid 2121212\n");
					// 	}
					// }
					//
					// }

						// printf("Enter %s\n",instr_ptr->tokens[i]);
					}

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
void echo(const char*name)
{
    char * v;

    if (name == NULL)
    {
        printf("%s", name);
    }
    else if (name[0] == '$')
    {
        char * v;
        if (strcmp(name, "$PATH") == 0)
            v = getenv( "PATH" );
        else if (strcmp(name, "$HOME") == 0)
            v = getenv( "HOME" );
        else if (strcmp(name, "$USER") == 0)
            v = getenv( "USER" );
        else if (strcmp(name, "$SHELL") == 0)
            v = getenv( "SHELL" );
        else if (strcmp(name, "$PWD") == 0)
            v = getenv( "PWD" );
        else
        {
            v = "Error Occurred";
        }
            printf("%s ", v);

    }
    else
    {
        printf("%s ", name);
    }
}
