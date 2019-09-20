#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//commit
typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

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

char *getPrompt(){
    //test with CD command
    char *prompt = calloc(50,sizeof(char));
    //memset(prompt, 0, 150);

    char *user = calloc(50,sizeof(char));
    //memset(user, 0, 25);
    user = getenv("USER");

    char *hostname = calloc(50,sizeof(char));
    //memset(hostname, 0, 50);
    
    gethostname(hostname, 50);


    char *dir = calloc(50,sizeof(char));
    //memset(dir, 0, 25);

    dir = getenv("PWD");


    strcat(prompt, user);
    strcat(prompt, "@");
    strcat(prompt, hostname);
    strcat(prompt, ":");
    strcat(prompt, dir);

    return prompt;

}

void my_execute(char **cmd){
    //cmd[] ="/bin/ls";
    char *command = PATHRes(cmd[0]);
    
    int status;
    pid_t pid = fork();
    if(pid == -1){
        //error
        exit(1);
    } else if(pid == 0){
        
        //child process
        execv(command, cmd);
        printf("Problem executing %s\n", cmd[0]);
        exit(1);
    } else{
        
        waitpid(pid, &status, 0);
    }
    
    
}

int main() {
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;


    while (1) {

        printf("%s ", getPrompt());
        //PATHRes("ls");

        // loop reads character sequences separated by whitespace
        do {
            //scans for next token and allocates token var to size of scanned token
            scanf("%ms", &token);
            temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

            int i;
            int start = 0;
            for (i = 0; i < strlen(token); i++) {
                //pull out special characters and make them into a separate token in the instruction
                if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
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
                }
            }

            if (start < strlen(token)) {
                memcpy(temp, token + start, strlen(token) - start);
                temp[i-start] = '\0';
                addToken(&instr, temp);
            }
            
//            char *parmList[] = {"/bin/ls", "-l", NULL};

            
            //instr.tokens = parmList;
            my_execute(instr.tokens);
            //free and reset variables
            free(token);
            free(temp);

            token = NULL;
            temp = NULL;
            
            
            
        } while ('\n' != getchar());    //until end of line is reached

        addNull(&instr);
        printTokens(&instr);
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

void clearInstruction(instruction* instr_ptr)
{
    int i;
    for (i = 0; i < instr_ptr->numTokens; i++)
        free(instr_ptr->tokens[i]);

    free(instr_ptr->tokens);

    instr_ptr->tokens = NULL;
    instr_ptr->numTokens = 0;
}
