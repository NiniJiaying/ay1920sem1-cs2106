/*************************************
 * Lab 2 Exercise 2
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#define BUF_SIZE 256
#define MAX_NUM_OF_ARGUMENTS 10
#define MAX_LENGTH_OF_COMMANDS 19
#define MAX_LENGTH_OF_ARGUMENTS 19

#include <stdio.h>
#include <fcntl.h>      //For stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>   //for waitpid()
#include <unistd.h>     //for fork(), wait()
#include <string.h>     //for string comparison etc
#include <stdlib.h>     //for malloc()

char** readTokens(int maxTokenNum, int maxTokenSize, int* readTokenNum, char* buffer);
void freeTokenArray(char** strArr, int size);
char* getCompletePath(char** tokens);
void executeCommand(char** tokens);

int main() {
    //TODO add your code
    char *buf;
    size_t bufsize = BUF_SIZE;
    buf = (char *) malloc (bufsize * sizeof(char));
    char** tokens;
    while(1) {
        printf("GENIE > ");
        getline(&buf, &bufsize, stdin);
        int num;
        tokens = readTokens(20, MAX_LENGTH_OF_COMMANDS + 1, &num, buf);
        if(num == 0) {
            // No command is given
            continue;
        } else if(num == 1 && strcmp(tokens[0], "quit") == 0) {
            // quit command is given
            printf("Goodbye!\n");
            freeTokenArray(tokens, num);
            exit(0);
        } else {
            int pid = fork();
            if(pid == 0) {
                executeCommand(tokens);
            } else {
                wait(NULL);
            }
        }
        freeTokenArray(tokens, num);
        printf("\n");
    }
    free(buf);
    return 0;
}


char* getCompletePath(char** tokens) {
    size_t pathSize = 25;
    char* path = malloc(pathSize * sizeof(char));
    char binPath[] = "/bin/";
    strcpy(path, binPath);
    if(!(tokens[0][0] == '/')) {
        strcat(path, tokens[0]);
    } else {
        path = tokens[0];
    }
    return path;
}

void executeCommand(char** tokens) {
    char* path = getCompletePath(tokens);
    struct stat buffer;
    if(stat(path, &buffer) != 0) {
        printf("%s not found\n", path);
        exit(-1);
    } else {
        execv(path, &tokens[0]);
        exit(0);
    }
}

char** readTokens(int maxTokenNum, int maxTokenSize, int* readTokenNum, char* buffer)
//Tokenize buffer
//Assumptions:
//  - the tokens are separated by " " and ended by newline
//Return: Tokenized substrings as array of strings
//        readTokenNum stores the total number of tokens
//Note:
//  - should use the freeTokenArray to free memory after use!
{
    char** tokenStrArr;
    char* token;
    int i;

    //allocate token array, each element is a char*
    tokenStrArr = (char**) malloc(sizeof(char*) * maxTokenNum);
    
    //Nullify all entries
    for (int i = 0; i < maxTokenNum; i++) {
        tokenStrArr[i] = NULL;
    }

    token = strtok(buffer, " \n");
    
    i = 0;
    while (i < maxTokenNum && (token != NULL)) {
         //Allocate space for token string
        tokenStrArr[i] = (char*) malloc(sizeof(char*) * maxTokenSize);

        //Ensure at most 19 + null characters are copied
        strncpy(tokenStrArr[i], token, maxTokenSize - 1);

        //Add NULL terminator in the worst case
        tokenStrArr[i][maxTokenSize-1] = '\0';
        
        i++;
        token = strtok(NULL, " \n");
    }
    
    *readTokenNum = i;
    
    return tokenStrArr;
}

void freeTokenArray(char** tokenStrArr, int size) {
    int i = 0;

    //Free every string stored in tokenStrArr
    for (i = 0; i < size; i++) {
        if (tokenStrArr[i] != NULL) {
            free(tokenStrArr[i]);
            tokenStrArr[i] = NULL;
        }
    }
    //Free entire tokenStrArr
    free(tokenStrArr);

    //Note: Caller still need to set the strArr parameter to NULL
    //      afterwards
}


