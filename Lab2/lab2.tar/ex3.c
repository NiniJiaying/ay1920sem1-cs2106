/*************************************
 * Lab 2 Exercise 3
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#define READ_END 0
#define WRITE_END 1

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

int main() {
    //TODO add your code
    char *buf;
    size_t bufsize = 256;
    buf = (char *) malloc (bufsize * sizeof(char));
    char** tokens;
    while(1) {
        printf("GENIE > ");
        getline(&buf, &bufsize, stdin);
        int num;
        tokens = readTokens(20, 19, &num, buf);
        for(int i=0;i<num;i++) {
            printf("%s\n", tokens[i]);
        }
        return 0;
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
                size_t pathSize = 25;
                char* path = malloc(pathSize * sizeof(char));
                char binPath[] = "/bin/";
                strcpy(path, binPath);
                if(!(tokens[0][0] == '/')) {
                    strcat(path, tokens[0]);
                } else {
                    path = tokens[0];
                }
                struct stat buffer;
                if(stat(path, &buffer) != 0) {
                    printf("%s not found\n", path);
                    exit(-1);
                } else {
                    execv(path, &tokens[0]);
                }
            } else {
                wait(NULL);
            }
        }
        freeTokenArray(tokens, num);
        printf("\n");
    }
    return 0;
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


