/*************************************
 * Lab 2 Exercise 3
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

#define debug(x) fprintf(stderr, "debug %s\n", x);

#define MAX_TOKEN_NUM 200
#define MAX_SINGLE_COMMAND_TOKEN_NUM 19 + 5
#define MAX_TOKEN_SIZE 19 + 5
#define MAX_PIPE_NUM 10 + 2
#define MAX_NUM_OF_JOBS 10 + 2

#define READ_END 0
#define WRITE_END 1

#define COMMAND_QUIT -1
#define COMMAND_VALID 1
#define COMMAND_INVALID 0

char **readTokens(int maxTokenNum, int maxTokenSize, int *readTokenNum, char *buffer);
void freeTokenArray(char **strArr, int size);

char *getCommand();
int executeCommand(char **command, char *keyword, int tokenNum);
char **duplicateCommand(char **tokens, int leftIndex, int rightIndex);
int keywordSwitcher(char *keyword);
void removeQuotes(char *token, int len);

int main()
{
    char keyword[25];
    int i;
    while (1)
    {
        int hasQuit = 0, hasInvalidCmd = 0;
        int tokenNum = 0;
        char **tokens = readTokens(MAX_TOKEN_NUM, MAX_TOKEN_SIZE, &tokenNum, getCommand());

        if (tokenNum <= 0)
        {
            freeTokenArray(tokens, tokenNum);
            continue;
        }

        int pipeFd[MAX_PIPE_NUM][2];
        int currentPipeIndex = 0;
        for (i = 0; i < MAX_PIPE_NUM; i++)
            pipe(pipeFd[i]);
        // Connect stdout of parent to the write end of the first pipe
        int outFd = dup(1);
        int inFd = dup(0);
        dup2(pipeFd[currentPipeIndex][WRITE_END], STDOUT_FILENO);

        int startIndex = 0;
        for (i = 0; i < tokenNum; i++)
        {
            if (strcmp(tokens[i], "|") == 0)
            {
                // Found a pipe
                char **command = duplicateCommand(tokens, startIndex, i);
                strcpy(keyword, command[0]);
                int executeResult = executeCommand(command, keyword, i - startIndex);
                freeTokenArray(command, i - startIndex);

                if (executeResult == COMMAND_INVALID)
                {
                    hasInvalidCmd = 1;
                    break;
                }

                if (executeResult == COMMAND_QUIT)
                {
                    hasQuit = 1;
                    break;
                }

                dup2(pipeFd[currentPipeIndex][READ_END], STDIN_FILENO);
                dup2(pipeFd[currentPipeIndex + 1][WRITE_END], STDOUT_FILENO);
                close(pipeFd[currentPipeIndex][WRITE_END]);
                currentPipeIndex++;
                startIndex = i + 1;
            }
        }

        dup2(outFd, STDOUT_FILENO);
        close(outFd);

        if (hasQuit)
        {
            printf("Goodbye!\n");
            freeTokenArray(tokens, tokenNum);
            return 0;
        }

        if (hasInvalidCmd)
        {
            printf("%s not found\n", keyword);
        }
        else
        {
            // Execute the last command in the pipe,
            // or the first command if there isn't any pipes
            char **command = duplicateCommand(tokens, startIndex, tokenNum);
            strcpy(keyword, command[0]);
            int executeResult = executeCommand(command, keyword, tokenNum - startIndex);
            freeTokenArray(command, tokenNum - startIndex);
            if (executeResult == COMMAND_INVALID)
            {
                printf("%s not found\n", keyword);
            }
            else if (executeResult == COMMAND_QUIT)
            {
                printf("Goodbye!\n");
                freeTokenArray(tokens, tokenNum);
                return 0;
            }
        }
        dup2(inFd, STDIN_FILENO);
        close(inFd);
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

char **duplicateCommand(char **tokens, int leftIndex, int rightIndex)
{
    char **command = (char **)malloc(sizeof(char *) * MAX_SINGLE_COMMAND_TOKEN_NUM);
    int i;
    for (i = leftIndex; i < rightIndex; i++)
    {
        command[i - leftIndex] = (char *)malloc(sizeof(char) * MAX_TOKEN_SIZE);
        removeQuotes(command[i - leftIndex], strlen(tokens[i]));
        strcpy(command[i - leftIndex], tokens[i]);
    }
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