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
char *builtins = "|cd|exit|echo|alias|unalias"; //cd:1, exit:4, echo:9, alias:11, unalias:17
int builtinsIndices[] = {1,4,9,14,20};
char *ALIAS[10];

typedef struct
{
    int pid;
    char *token;
} Background;
Background backgroundProcesses[10];
int backgroundCounter;


void popToken(instruction* instr_ptr)
{
    //pop the last token
    if (instr_ptr->numTokens > 0) {
        int i = instr_ptr->numTokens - 1;
        char *out = (instr_ptr->tokens)[i];
        printf("popping %s\n", out);
        free(out);
        (instr_ptr->tokens)[i] = NULL;
        //instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, i * sizeof(char*));
        instr_ptr->numTokens = i;
        
        }
}

//void alias(){
//
//
//}
//
//void InRedir(char * before,char *after)
//{
//    int fd= open(before,O_RDONLY);
//    if(fork()==0)
//    {
//        close (STDIN_FILENO);
//        dup(fd);
//        close(fd);
//        //execute
//    }
//    else
//    {
//        close (fd);
//    }
//}
//void OutRedir(char * before,char *after)
//{
//    int fd=open(before,O_RDONLY|O_CREAT|O_TRUNC);
//    if (fork()==0)
//    {
//        close(STDOUT_FILENO);
//        dup(fd);
//        close(fd);
//        //execute
//        exit(fd);
//    }
//    else
//    {
//        close(fd);
//    }
//}
//
//void piping(char * before,char *after, int c_pipe)
//{
//    int fd[2];
//
//    if (fork()==0)
//    {
//        if(c_pipe==1) //single pipeline
//        {
//            pipe(fd);
//            if(fork()==0)
//            {
//                close (STDOUT_FILENO);
//                dup(fd[1]);
//                close (fd[0]);
//                close (fd[1]);
//                //execute
//            }
//            else
//            {
//                close(STDIN_FILENO);
//                dup(fd[0]);
//                close (fd[0]);
//                close (fd[1]);
//                //execute
//            }
//        }
//        else if (c_pipe>1) //multipiping
//        {
//            pipe(fd);
//            if(fork()==0)
//            {
//                if(fork()==0)
//                {
//
//                }
//                close (STDOUT_FILENO);
//                dup(fd[0]);
//                close (fd[0]);
//                close (fd[1]);
//            }
//            else
//            {
//                close(STDIN_FILENO);
//                dup(fd[0]);
//                close (fd[0]);
//                close (fd[1]);
//            }
//        }
//    }
//    else
//    {
//        close(fd[1]);
//        close(fd[0]);
//
//    }
//}


void cd(char *path){
    
    if( chdir(path)==0 )
    {
        setenv("PWD", getcwd(NULL,0), 1);
        
    } else{
        printf("invalid directory\n");
    }
}

int lastCharacter(char *str, char ch) {
    int index = -1;
    int i;
    for(i = 0; i <= strlen(str); i++)
        {
            if(str[i] == ch)
                {
                    index = i;
                    }
            }
    return index;
}

char *dropLastPathComponent(char *pwd) {
    
    int last = lastCharacter(pwd, '/');
    if (last > 0) {
        pwd[last] = 0;
    }
    return pwd;
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


    
        if(access(buffer, X_OK) == 0){

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

void my_execute(instruction* instr_ptr){
    //cmd[] ="/bin/ls";
    int no_hang = 0;
    
    char *command = PATHRes(instr_ptr->tokens[0]);
    
    //strcmp(&cmd[tokenCount][0], "&");
    
    
    
    if(instr_ptr->numTokens > 1 && instr_ptr->tokens[instr_ptr->numTokens - 1][0] == '&'){
//        printf("NO HANG: %s\n", cmd[tokenCount - 1]);
//         printf("NO HANG: %d\n", tokenCount);
        //instr_ptr->tokens[instr_ptr->numTokens - 1][0] = 0;
        popToken(instr_ptr);
        no_hang = 1;
        }
    
    int status;
    pid_t pid = fork();
    if(pid == -1){
        //error
        exit(1);
    } else if(pid == 0){
        
        //child process
        execv(command, instr_ptr->tokens);
        printf("Problem executing %s\n", instr_ptr->tokens[0]);
        exit(1);
    } else{
        if(no_hang == 0){
        waitpid(pid, &status, 0);
        } else if (no_hang == 1){
          waitpid(pid, &status, WNOHANG);
            backgroundProcesses[backgroundCounter].pid = pid;
            backgroundProcesses[backgroundCounter].token = (char *)malloc(sizeof(char) * strlen(instr_ptr->tokens[0])); //free this later
            strcpy(backgroundProcesses[backgroundCounter].token, instr_ptr->tokens[0]);
            backgroundCounter++;
            
            
        }
    }
}

int main() {
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;


    while (1) {
        //check if backgroud is done
        //backgroundcounter-- when done
        
        printf("%s: ", getPrompt());
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

            int p = 0;
            for(p; p < instr.numTokens; p++){
                
                switch ((char)instr.tokens[p]) {
                    case '&':
                        //background execution, call with WNOHANG, add to queue
                        
                        
                        break;
                        
                    case '~':
                        //expand to home directory
                        instr.tokens[p] = getEnviornment("~");
                        break;
                    case '.':
                        if((char)instr.tokens[p][1] == '.'){
                            char *pwd = getenv("PWD");
                            char *pwdcpy;
                            strcpy(pwdcpy, pwd);
                        instr.tokens[p] = dropLastPathComponent(pwdcpy);
                        } else {
                            char *pwd = getenv("PWD");
                            char *pwdcpy;
                            strcpy(pwdcpy, pwd);
                            instr.tokens[p] = pwdcpy;
                        }
                   
                        
//                    case '':
//                    default:
//                        break;
                }
                
            }              //shortcut expansion
            
            // check for built-in commands
            int index;
            if(index = (strstr(builtins, instr.tokens[0]) - builtins)){
                switch (index) {
                    case 1:
                        printf("%s", instr.tokens[1]);
                        cd(instr.tokens[1]);
                        break;
                    
                    case 4:
                        
                        
                        
                        break;
                    case 9:
                        
                        break;
                        
                    case 14:
                        
                        break;
                        
                    case 20:
                        
                        break;
                        
                    default:
                        break;
                }
                
            }
            
            my_execute(&instr);
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

void popLastToken(instruction* instr_ptr){
    
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





