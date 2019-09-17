/*************************************
 * Lab 2 Exercise 2
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#include <stdio.h>
#include <fcntl.h> //For stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> //for waitpid()
#include <unistd.h>   //for fork(), wait()
#include <string.h>   //for string comparison etc
#include <stdlib.h>   //for malloc()

#define MAX_TOKEN_NUM 10 + 5
#define MAX_TOKEN_SIZE 19 + 5

#define COMMAND_QUIT -1
#define COMMAND_VALID 1
#define COMMAND_INVALID 0

char **readTokens(int maxTokenNum, int maxTokenSize, int *readTokenNum, char *buffer);
void freeTokenArray(char **strArr, int size);

char *getCommand();
int executeCommand(char **command, char *keyword, int tokenNum);
int keywordSwitcher(char *keyword);
void removeQuotes(char *token, int len);

int main()
{
    char keyword[25];
    int i;
    while (1)
    {
        int tokenNum = 0;
        char **tokens = readTokens(MAX_TOKEN_NUM, MAX_TOKEN_SIZE, &tokenNum, getCommand());
        if (tokenNum <= 0)
        {
            freeTokenArray(tokens, tokenNum);
            continue;
        }
        strcpy(keyword, tokens[0]);
        int executeResult = executeCommand(tokens, keyword, tokenNum);
        switch (executeResult)
        {
        case COMMAND_QUIT:
            printf("Goodbye!\n");
            freeTokenArray(tokens, tokenNum);
            return 0;
            break;
        case COMMAND_INVALID:
            printf("%s not found\n", keyword);
            break;
        default:
            // i.e. COMMAND_VALID
            break;
        }
        printf("\n");
        freeTokenArray(tokens, tokenNum);
    }
    return 0;
}

char *getCommand()
{
    char *command = NULL;
    size_t len = 0;
    printf("GENIE > ");
    getline(&command, &len, stdin);
    return command;
}

int executeCommand(char **command, char *keyword, int tokenNum)
{
    int typeOfCommand = keywordSwitcher(keyword);
    switch (typeOfCommand)
    {
    case COMMAND_QUIT:
        return COMMAND_QUIT;
        break;
    case COMMAND_INVALID:
        return COMMAND_INVALID;
        break;
    case COMMAND_VALID:
    {
        int childPid = fork();
        int i;
        if (childPid == 0)
        {
            for (i = 0; i < tokenNum; i++)
                removeQuotes(command[i], strlen(command[i]));
            command[tokenNum] = NULL;
            execv(keyword, command);
        }
        else
            waitpid(childPid, NULL, 0);
        return COMMAND_VALID;
        break;
    }
    default:
        return COMMAND_INVALID;
        break;
    }
}

int keywordSwitcher(char *keyword)
{
    struct stat buf;
    if (strcmp(keyword, "quit") == 0)
        return COMMAND_QUIT;
    else if (stat(keyword, &buf) == 0)
        return COMMAND_VALID;
    else
    {
        char defaultPath[MAX_TOKEN_SIZE] = "/bin/";
        strcpy(keyword, strcat(defaultPath, keyword));
        if (stat(keyword, &buf) == 0)
            return COMMAND_VALID;
    }
    return COMMAND_INVALID;
}

void removeQuotes(char *token, int tokenLen)
{
    int i, ptr = 0;
    for (i = 0; i < tokenLen; i++)
    {
        if (token[i] != '"') 
            token[ptr++] = token[i];
    }
    token[ptr] = '\0';
}

char **readTokens(int maxTokenNum, int maxTokenSize, int *readTokenNum, char *buffer)
//Tokenize buffer
//Assumptions:
//  - the tokens are separated by " " and ended by newline
//Return: Tokenized substrings as array of strings
//        readTokenNum stores the total number of tokens
//Note:
//  - should use the freeTokenArray to free memory after use!
{
    char **tokenStrArr;
    char *token;
    int i;

    //allocate token array, each element is a char*
    tokenStrArr = (char **)malloc(sizeof(char *) * maxTokenNum);

    //Nullify all entries
    for (int i = 0; i < maxTokenNum; i++)
    {
        tokenStrArr[i] = NULL;
    }

    token = strtok(buffer, " \n");

    i = 0;
    while (i < maxTokenNum && (token != NULL))
    {
        //Allocate space for token string
        tokenStrArr[i] = (char *)malloc(sizeof(char *) * maxTokenSize);

        //Ensure at most 19 + null characters are copied
        strncpy(tokenStrArr[i], token, maxTokenSize - 1);

        //Add NULL terminator in the worst case
        tokenStrArr[i][maxTokenSize - 1] = '\0';

        i++;
        token = strtok(NULL, " \n");
    }

    *readTokenNum = i;

    return tokenStrArr;
}

void freeTokenArray(char **tokenStrArr, int size)
{
    int i = 0;

    //Free every string stored in tokenStrArr
    for (i = 0; i < size; i++)
    {
        if (tokenStrArr[i] != NULL)
        {
            free(tokenStrArr[i]);
            tokenStrArr[i] = NULL;
        }
    }
    //Free entire tokenStrArr
    free(tokenStrArr);

    //Note: Caller still need to set the strArr parameter to NULL
    //      afterwards
}