#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);


char *PATHRes(char *cmd){
    char *token;
    char *PATH = getenv("PATH");
    //char **path = (char **)malloc(20 * sizeof(char *));
    char *buffer = (char *)malloc(50 * sizeof(char));
    int x = 0;
    
    token = strtok (PATH,":");
    while (token != NULL)
    {
        printf ("%s\n",token);
        token = strtok (NULL, ":");
        //path[x] = token;
        x++;
        
        sprintf(buffer, "%s", token);
        sprintf(buffer + strlen(buffer), "/");
        sprintf(buffer + strlen(buffer), "%s", cmd);
        
        if(access(buffer, X_OK) == 0){
            return buffer;
        }
       
    }
    
    printf("Command not found");
    return NULL;

    

    //char *command = strcat("/", cmd);
//    char *PATH = getenv("PATH");
//    char *p = strdup(PATH);
//    char *colon = NULL;
//    char *s = p;
//
//    do{
//        colon = strchr(p, ':');
//
//
//    if (colon != NULL) {
//        colon[0] = 0;
//    }
//
//        printf("Path in $PATH: %s\n", s);
//        s = colon + 1;
//
////        if(strcmp(strcat(PATH, command), s)){
////
////            printf("Found command");
////
////        }
//
//
//    }while (colon != NULL);
//
//
//
//    //printf("%s", PATH);
//
//
//    return PATH;
}

char *getPrompt(){
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
    
    return prompt;
    
}

void my_execute(char **cmd){
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

int main() {
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;


    while (1) {

        printf("%s ", getPrompt());
        PATHRes("ls");

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
            
            char *parmList[] = {"/bin/ls", "-l", NULL};

            
            instr.tokens = parmList;
            my_execute(instr.tokens);
            //free and reset variables
            free(token);
            free(temp);

            token = NULL;
            temp = NULL;
            
            
            
        } while ('\n' != getchar());    //until end of line is reached

        //addNull(&instr);
        printTokens(&instr);
        //clearInstruction(&instr);
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
