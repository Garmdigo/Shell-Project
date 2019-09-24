#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <stdbool.h>
//commit
typedef struct
{
    char** tokens;
    int numTokens;
} instruction;


typedef struct
{
    char **tokens;
    char **cmds;
    int count;
} Aliases;

Aliases aliases;

void addAlias(char *alias, char *instruction)
{
    // extend alias table
    
    if (aliases.count == 0) {
        aliases.tokens = (char**) malloc(sizeof(char*));
        aliases.cmds= (char**) malloc(sizeof(char*));
    } else {
        aliases.tokens = (char**) realloc(aliases.tokens, (aliases.count+1) * sizeof(char*));
        aliases.cmds= (char**) realloc(aliases.cmds,(aliases.count+1) * sizeof(char*));
    }
    
    //allocate tokens and commands
    aliases.tokens[aliases.count] = (char *)malloc((strlen(alias)+1) * sizeof(char));
    aliases.cmds[aliases.count]= (char *)malloc((strlen(instruction)+1) * sizeof(char));
    
    strcpy(aliases.tokens[aliases.count], alias);
    strcpy(aliases.cmds[aliases.count], instruction);
    
    //printf("added alias -- %s: %s\n", aliases.tokens[aliases.count], aliases.cmds[aliases.count]);
    
    aliases.count++;
}

int findAlias(char *alias) {
    int i;
    if (aliases.count == 0) return -1;
    for (i = 0; i < aliases.count; i++) {
        char *checking = aliases.tokens[i];
        if (strcmp(checking, alias) == 0) {
            // matches
            return i;
        }
    }
    return -1;
}


void unAlias(char *alias)
{
    int i = findAlias(alias);
    if ( i > 0) {
        char *found = aliases.tokens[i];
        found[0] = 0;
    }
    
}

void replaceInstr(instruction* instr_ptrSource, instruction* instr_ptrReplacement, int at) {
    
}

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

char *redirectChars = "<>|&";
char *builtins = "||cd|exit|echo|alias|unalias|";
int builtinsIndices[] = {1,4,9,14,20};
int executed = 0;
int valid = 0;

enum {
    SH_WAIT = 0,
    SH_NOHANG = 1,
    SH_NOWAIT = 2
};

typedef struct
{
    int pid;
    char *token;
} Background;

Background *backgroundProcesses = NULL;
int backgroundCounter = 1;

void my_echo(const char*name)
{
    char *v;
    
    if (name == NULL)
    {
        printf("%s", name);
    }
    
    
    else if (name[0] == '$')
    {
        char *v;
        name = toupper(name);
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
            v = "invalid argument";
        }
        printf("%s \n", v);
        
    }
    else
    {
        printf("%s \n", name);
    }
}

void popToken(instruction* instr_ptr)
{
    //pop the last token off
    if (instr_ptr->numTokens > 0) {
        int i = instr_ptr->numTokens - 1;
        char *out = (instr_ptr->tokens)[i];
        //printf("popping %s\n", out);
        free(out);
        (instr_ptr->tokens)[i] = NULL;
        //instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, i * sizeof(char*));
        instr_ptr->numTokens = i;
        
    }
}



instruction* decompose(instruction* instr_ptr, int *last)
{
    //break commands into seperate token lists based on special characters
    instruction *result = (instruction*) calloc(32,sizeof(instruction));
    int i, iidx = 0;
    
    for (i = 0; i < instr_ptr->numTokens; i++) {
        char *x = (instr_ptr->tokens)[i];
        if (x != NULL) {
            if (strchr(redirectChars, x[0])) {
                // is a redirect char
                addToken(&result[++iidx], x);
                ++iidx;
            } else {
                addToken(&result[iidx], x);
            }
        }
    }
    *last = iidx;
    
    
    //// to traverse backwards...
    //instruction *it;
    //for (i = *last; i > -1; --i) {
    //if ((it = &result[i]) != NULL) {
    ////...
    //
    //}
    //}
    
    //    while (it->tokens != NULL){
    //        //...
    //
    //        printTokens(it);
    //        it = &result[++j];
    //    }
    return result;
}


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
                //     return 1;
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

void cd(char *path){ //cd funcion
    
    if( chdir(path)==0 )
    {
        char buff[MAXPATHLEN];
        setenv("PWD", getcwd(buff, MAXPATHLEN), 1);
        
    } else if(strcmp(path, "~") != 0){
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
    //
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
    
    if(v == NULL){
        printf("Envionment variable not found\n");
        return NULL;
    }
    strcpy(result, v);
    return  result;
}


char *PATHRes(char *cmd){
    
    //printf("%s\n", cmd);
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
            
            //printf("buffer: %s\n", buffer);
            //printf("token: %s\n", token);
            
            return buffer;
            
        } else {buffer[0] = 0;}
        
        token = strtok (NULL, ":");
    }
    
    printf("Command not found\n");
    return NULL;
    
}

char* trimRight(char* s) {
    
    int len = strlen(s);
    if(len == 0) {
        return s;
    }
    
    char* pos = s + len - 1;
    while(pos >= s && isspace(*pos)) {
        *pos = '\0';
        pos--;
    }
    return s;
}


char *getPrompt(){
    
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

void my_execute(instruction* instr_ptr, int fdi, int fdo, int waitType){
    char buffer[128];
    addNull(instr_ptr);
    
    if(waitType)
        
        
        //printTokens(instr_ptr);
    //    int length = strlen(instr_ptr->tokens[0]);
    //    if((instr_ptr->tokens[0][length] == '\n')){
    
    sprintf(buffer, "%s", instr_ptr->tokens[0]);
    //    }
    
    //printf("%s %d %d\n", instr_ptr->tokens[0], fdi, fdo);
    //cmd[] ="/bin/ls";
    int no_hang = 0;
    char *command = PATHRes(instr_ptr->tokens[0]);
    
    //strcmp(&cmd[tokenCount][0], "&");
    
    //printTokens(instr_ptr);
    
    
    //if(instr_ptr->numTokens > 1 && instr_ptr->tokens[instr_ptr->numTokens - 1][0] == '&'){
    ////printf("NO HANG: %s\n", cmd[tokenCount - 1]);
    ////printf("NO HANG: %d\n", tokenCount);
    ////instr_ptr->tokens[instr_ptr->numTokens - 1][0] = 0;
    //popToken(instr_ptr);
    //no_hang = 1;
    //}
    int bCounter;
    int status;
    pid_t pid = fork();
    if(pid == -1){
        //in redirection
        exit(1);
    } else if(pid == 0){
        if (fdi > -1) { //input redirect
            close (STDIN_FILENO);
            dup(fdi);
            close(fdi);
        }
        
        if (fdo > -1) { //output redirection
            close (STDOUT_FILENO);
            dup(fdo);
            close(fdo);
        }
        
        //child process
        execv(command, instr_ptr->tokens);
        printf("Problem executing %s\n", instr_ptr->tokens[0]);
        exit(1);
    } else{
        switch(waitType){
            case SH_WAIT:
                waitpid(pid, &status, 0);
                break;
            case SH_NOHANG:
                waitpid(pid, &status, WNOHANG);
                
                for(bCounter = 0; bCounter <= 10; bCounter++){
                    
                    if(backgroundProcesses[bCounter].pid == 0){
                        backgroundProcesses[backgroundCounter].pid = pid;
                        backgroundProcesses[backgroundCounter].token = (char *)malloc(sizeof(char) * strlen(instr_ptr->tokens[0])); //keeping track of background processes
                        strcpy(backgroundProcesses[backgroundCounter].token, instr_ptr->tokens[0]);
                        
                        if(bCounter <= backgroundCounter){
                            backgroundCounter++;
                        }
                        break;
                    }
                }
                printf("[%d] [%d]\n", backgroundCounter, pid);
                break;
            case SH_NOWAIT:
                break;
        }
        //if(no_hang == 0){
        //waitpid(pid, &status, 0);
        //} else if (no_hang == 1){
        //waitpid(pid, &status, WNOHANG);
        //backgroundProcesses[backgroundCounter].pid = pid;
        //backgroundProcesses[backgroundCounter].token = (char *)malloc(sizeof(char) * strlen(instr_ptr->tokens[0])); //free this later
        //strcpy(backgroundProcesses[backgroundCounter].token, instr_ptr->tokens[0]);
        //backgroundCounter++;
        //
        //
        //}
    }
}


void execute_wrapper(instruction* instr) { //wrapper func sets up IO redirection, pipes, background execution before calling my_execute
    int last;
    char *fileName;
    FILE* in;
    FILE* out;
    int pipefds[2];
    int pipestate = 0;
    int backgroundOnly = 0;
    
    
    instruction *result = decompose(instr, &last);
    
    int j = 0;
    instruction *it;
    instruction *delayed = NULL;
    
    int fni = -1;
    int fno = -1;
    int pipeFlag = 0;
    
    
    for (j = last; j > -1; --j) {
        
        if ((it = &result[j])->tokens != NULL) {
            //printTokens(it);
            int m = j -1;
            instruction *next = (m > -1) ? &result[m] : NULL;
            int redir = ((next) && (strchr(next->tokens[0], '>') || strchr(next->tokens[0], '<' ))); //check to see if redirect character is present in the next instruction list
            //printf("redir: %d\n", redir);
            
            if(strchr(it->tokens[0], '/') || redir){
                //decide if its a file or a command
                fileName = it->tokens[0];
                
            } else if(strlen(it->tokens[0]) > 1) {
                // is next a pipe? if so don't execute yet -- delayed.
                int m = j -1;
                if (m > -1) {
                    instruction *next = &result[m];
                    if (next->tokens[0][0] == '|') {
                        delayed = it;
                    }
                }
                
                if (backgroundOnly) {
                    my_execute(it, fni, fno, SH_NOHANG);
                } else {
                    
                    if(pipeFlag){
                        pipeFlag = 0;
                        //printf("I'm the first process\n");
                        
                        int fd[2];
                        int status;
                        int pid = fork();
                        
                        if (pid == 0) { //Child (cmd1 | cmd2)
                            pipe(fd);
                            int pid2 = fork();
                            if (pid2 == 0) {
                                //cmd1 (Writer)
                                close(STDOUT_FILENO); dup(fd[1]);
                                close(fd[0]);
                                close(fd[1]);
                                addNull(it);
                                //printf("I'm the first process\n");
                                execv(it->tokens[0], it->tokens);
                                //printf("error in process a.");
                            }
                            else {
                                close(STDIN_FILENO);
                                dup(fd[0]);
                                close(fd[0]);
                                close(fd[1]);
                                addNull(delayed);
                                //printf("I'm the second process\n");
                                //waitpid(pid2, &status, 0);
                                execv(delayed->tokens[0], delayed->tokens);
                                //printf("error in process a.");
                                
                                                          }
                            
                        } else {
                            //waitpid(pid, &status, 0);
                            //close(fd[0]);
                        }
                    }else{
                        if(it != delayed){
                       my_execute(it, fni, fno, SH_WAIT); //normal execute
                        }
                    }
                }
            }
            if (it->numTokens == 1) {  // redirect character check
                char tknChar = it->tokens[0][0];
                switch (tknChar){
                        
                    case '<':
                        in = fopen(fileName, "r");
                        fni = fileno(in);
                        fileName = NULL;
                        
                        break;
                        
                    case '>':
                        out = fopen(fileName, "w");
                        fno = fileno(out);
                        fileName = NULL;
                        break;
                        
                    case '|':
                        
                        pipeFlag = 1;
                        
                        break;
                        
                    case '&':
                        // when appersand appears everything goes to background
                        backgroundOnly = 1;
                        break;
                        
                }
            }
        }
        
    }
    
    
    // check if last token &
    // check for redirects
    
}

int main() {
    char* token = NULL;
    char* temp = NULL;
    int status;
    int t;
    int ignore = 0;
    
    aliases.cmds = NULL;
    aliases.count = 0;
    aliases.tokens = NULL;
    backgroundProcesses = calloc(10, sizeof(Background));
    
    instruction instr;
    instruction alias;
    instr.tokens = NULL;
    instr.numTokens = 0;
    
    
    while (1) {
        //check if backgroud is done
        
        //backgroundcounter-- when done
        
        if(backgroundCounter > 1){
            
            for(t = 0; t < backgroundCounter; t++){
                
                if(backgroundProcesses[t].pid == 0){
                    continue;
                }
                pid_t result = waitpid(backgroundProcesses[t].pid, &status, WNOHANG);
                if (result == -1) {
                    //printf("error with background process");
                } else if (result == backgroundProcesses[t].pid) {
                    
                    printf("[%d] %s\n", t, backgroundProcesses[t].token);
                    backgroundProcesses[t].pid = 0;
                    
                    //prompt may not appear about before output because asynchronous nature of background processes
                }
            }
        }
        
    START:   printf("%s >", getPrompt());
        //PATHRes("ls");
        
        // loop reads character sequences separated by whitespace
        do {
            //scans for next token and allocates token var to size of scanned token
            scanf("%ms", &token);
            //trimRight(token);
            //printf("%s\n", token);
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
            
            
            
            
            
            //free and reset variables
            free(token);
            free(temp);
            
            token = NULL;
            temp = NULL;
            
            
            
        } while ('\n' != getchar()); //until end of line is reached
        
        valid=ErrorCheck(&instr); //does the error checking
        
        if(valid == 0){
            printf("Invalid command\n");
            clearInstruction(&instr);
            goto START; // <- very nuaghty
        }
        //shortcut expansion
        int p = 0;
        
        for(p; p < instr.numTokens; p++){
            int aliasPosition = 0;
            aliasPosition = findAlias(instr.tokens[p]);
            
            
            if(aliasPosition != -1 && strcmp(instr.tokens[p],"unalias") !=0 ){
                //sub in alias
                //printf("found: %s\n", aliases.cmds[aliasPosition]);
                
                
                //token = aliases.cmds[aliasPosition];
                
                //goto TOKENIZER;
                
                
                                instr.tokens[p] = realloc(instr.tokens[p], strlen(aliases.cmds[aliasPosition] + 1));
                                strcpy(instr.tokens[p], aliases.cmds[aliasPosition]);
            }
            switch ((char)instr.tokens[p]) {
                    
                case '~':
                    //expand to home directory
                    instr.tokens[p] = getEnviornment("~");
                    break;
                case '.':
                    if((char)instr.tokens[p][1] == '.'){
                        char *pwd = getenv("$PWD");
                        char *pwdcpy;
                        strcpy(pwdcpy, pwd);
                        instr.tokens[p] = dropLastPathComponent(pwdcpy);
                    } else { // case of ..
                        char *pwd = getenv("$PWD");
                        char *pwdcpy;
                        strcpy(pwdcpy, pwd);
                        instr.tokens[p] = pwdcpy;
                    }
                    
            }
            
            //relative path expansion
            if(strchr(instr.tokens[p], '/') != NULL){
                if(instr.tokens[p][0] != '/'){
                    char path[128];
                    //absolute, don't do anything
                    char *pwd = getEnviornment("$PWD");
                    
                    sprintf(path, "%s/%s", pwd, instr.tokens[p]);
                    //printf("checking validity of path %s\n", path);
                    
                    if(access(path, F_OK) == 0){
                        instr.tokens[p] = realloc(instr.tokens[p], strlen(path) + 1);
                        strcpy(instr.tokens[p], path);
                    } else { printf("Path is invalid: %s\n", instr.tokens[p]);}
                    //makeFilePath(instr.tokens[p]);
                }
            }
        }
        
        //switch to check for built-in commands
        int index;
        int status;
        char key[128];
        int k = 0;
        
        sprintf(key, "|%s|", instr.tokens[0]);
        if((index = (strstr(builtins, key) - builtins)) > 0){
            //printTokens(&instr)
            
            switch (index) {
                case 1:
                    //cd
                    //printf("%s\n", instr.tokens[1]);
                    
                    if(instr.tokens[2] != NULL){
                        printf("Error: Too many arguments");
                        break;
                    }
                    if(instr.tokens[1] == NULL){
                        cd(getEnviornment("~"));
                    } else{ cd(instr.tokens[1]); }
                    
                    break;
                    
                case 4:
                    //exit
                    printf("Exiting...\n");
                    waitpid(-1, &status, 0);
                    printf("Executed %d commands\n", executed);
                    exit(1);
                    
                    
                    break;
                case 9:
                    
                    for(k;k<instr.numTokens;k++){
                        my_echo(instr.tokens[k]);
                    }
                    
                    k = 0;
                    //printf("%s\n", instr.tokens[1]);
                    
                    //                    if(strcmp(instr.tokens[1], "..") == 0){
                    //                        printf("echo ..\n");
                    //                    }else if(instr.tokens[1][0] == '$'){
                    //                        printf("echo %s\n", getEnviornment(instr.tokens[1]));
                    //                    }
                    break;
                    
                case 14:
                    
                    if(instr.tokens[1] == NULL || instr.tokens[3] == NULL){
                        printf("Incorrect format for alias\n");
                    }
                    char *it = instr.tokens[3];
                    int length = strlen(instr.tokens[3]);
                    it[length -1] = 0;
                    it++;
                    
                    //printf("tokens[1]%s\n", instr.tokens[1]);
                    //printf("it: %s\n", it);
                    addAlias(instr.tokens[1],it);
                    //alias(&instr, &c_alias, store_name, store_instruction);
                    
                    
                    break;
                    
                case 20:
                    if(instr.tokens[1] != NULL){
                        unAlias(instr.tokens[1]);
                    }else{printf("Enter an alias to remove\n");}
                    break;
                    
                default:
                    break;
            }
            
        }else{execute_wrapper(&instr);}
        
        valid=ErrorCheck(&instr); //does the errorchecking
        //printf("Validity:%d\n",valid);
        if(valid){executed++;} //checks if it passed the error checking
        
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

void popLastToken(instruction* instr_ptr){
    
}
void printTokens(instruction* instr_ptr)
{
    int i;
    printf("Tokens:\n");
    for (i = 0; i < instr_ptr->numTokens; i++) {
        if ((instr_ptr->tokens)[i] != NULL)
            printf("%s|", (instr_ptr->tokens)[i]);
    }
    //printf(".\n");
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


