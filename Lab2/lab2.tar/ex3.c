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

#define BUF_SIZE 3000
#define MAX_NUM_OF_ARGUMENTS 10
#define MAX_LENGTH_OF_COMMANDS 19
#define MAX_LENGTH_OF_ARGUMENTS 19
#define MAX_NUM_OF_TOKENS 256
#define MAX_NUM_OF_JOBS 10

#include <stdio.h>
#include <fcntl.h> //For stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> //for waitpid()
#include <unistd.h>   //for fork(), wait()
#include <string.h>   //for string comparison etc
#include <stdlib.h>   //for malloc()

// ls -l | wc -l | echo "Genie is here"

char **readTokens(int maxTokenNum, int maxTokenSize, int *readTokenNum, char *buffer);
void freeTokenArray(char **strArr, int size);
char *getCompletePath(char **tokens);
void executeCommand(char **tokens);
char ***divideTokensByPipe(char **tokens, int numOfTokens, int *totalJobs, int **tokenCountByJob);

int main()
{
    char *buf;
    size_t bufsize = BUF_SIZE;
    buf = (char *)malloc(bufsize * sizeof(char));
    char **tokens;

    while (1)
    {
        printf("GENIE > ");
        getline(&buf, &bufsize, stdin);
        int numOfTokens;
        tokens = readTokens(20, MAX_LENGTH_OF_COMMANDS + 1, &numOfTokens, buf);

        if (numOfTokens == 0)
        {
            // No command is given
            continue;
        }
        else if (numOfTokens == 1 && strcmp(tokens[0], "quit") == 0)
        {
            // quit command is given
            printf("Goodbye!\n");
            freeTokenArray(tokens, numOfTokens);
            exit(0);
        }
        else
        {
            int totalJobs;
            int *tokenCountByJob;
            char ***tokensDividedByPipe = divideTokensByPipe(tokens, numOfTokens, &totalJobs, &tokenCountByJob);

            if (totalJobs == 1)
            {
                int pid = fork();
                if (pid == 0)
                {
                    executeCommand(tokens);
                }
                else
                {
                    wait(NULL);
                }
            }
            else
            {
                for (int i = 0; i < totalJobs-1; i++)
                {
                    int pipeFd[2], len;
                    if (pipe(pipeFd) == -1)
                    {
                        printf("failed to create pipes\n");
                    }
                    pid_t child = fork();
                    if (child == (pid_t)(-1))
                    {
                        printf("fork failed\n");
                    }
                    if (child == (pid_t)(0))
                    {
                        // child
                        close(1); // close stdout
                        if (dup(pipeFd[WRITE_END]) == -1)
                        {
                            printf("dup failed\n");
                        }
                        executeCommand(tokensDividedByPipe[i]);
                    }
                    else
                    {
                        // parent
                        pid_t pid = fork();
                        if(pid == (pid_t)(-1)) {
                            printf("fork failed\n");
                        }
                        if(pid == (pid_t)(0)) {
                            // left side of pipe
                            close(0); /* close stdin */
                            if (dup(pipeFd[READ_END]) == -1)
                            {
                                printf("dup failed\n");
                            }
                            executeCommand(tokensDividedByPipe[i+1]);
                        } else {
                            wait(NULL);
                        }
                    }
                    /*
                    if( (pid = fork()) > 0) {
                        // parent
                        close(pipeFd[READ_END]);
                        // write(pipeFd[WRITE_END], stuff, cnt);
                        close(pipeFd[WRITE_END]);
                    } else {
                        // child
                        close(pipeFd[WRITE_END]);
                        // len = read(pipeFd[READ_END], buf, sizeofbuf);
                        close(pipeFd[READ_END]);
                    }*/
                }
            }
        }

        freeTokenArray(tokens, numOfTokens);
        printf("\n");
    }

    free(buf);
    return 0;
}

char ***divideTokensByPipe(char **tokens, int numOfTokens, int *totalJobs, int **tokenCountByJob)
{
    int *tokensCount = (int *)malloc((sizeof(int *) * MAX_NUM_OF_JOBS));
    char ***tokensDividedByPipe = (char ***)malloc(sizeof(char **) * MAX_NUM_OF_JOBS);
    int last = 0;
    int jobCount = 0;
    for (int i = 0; i < numOfTokens; i++)
    {
        if (strlen(tokens[i]) == 1 && tokens[i][0] == '|')
        {
            char **temp = (char **)malloc(sizeof(char *) * (i - last + 1));
            for (int j = 0; j < i - last; j++)
            {
                temp[j] = (char *)malloc(sizeof(char) * strlen(tokens[i + j]));
                strcpy(temp[j], tokens[last + j]);
            }
            tokensDividedByPipe[jobCount] = temp;
            tokensCount[jobCount++] = i - last;
            last = i + 1;
        }
    }
    tokensDividedByPipe[jobCount] = (char **)malloc(sizeof(char *) * (numOfTokens - last));
    for (int i = 0; i < numOfTokens - last; i++)
    {
        tokensDividedByPipe[jobCount][i] = (char *)malloc(sizeof(char) * strlen(tokens[last + i]));
        strcpy(tokensDividedByPipe[jobCount][i], tokens[last + i]);
    }
    tokensCount[jobCount++] = numOfTokens - last;
    if (totalJobs != NULL)
        *totalJobs = jobCount;
    if (tokenCountByJob != NULL)
        *tokenCountByJob = tokensCount;
    return tokensDividedByPipe;
}

char *getCompletePath(char **tokens)
{
    size_t pathSize = 25;
    char *path = malloc(pathSize * sizeof(char));
    char binPath[] = "/bin/";
    strcpy(path, binPath);
    if (!(tokens[0][0] == '/'))
    {
        strcat(path, tokens[0]);
    }
    else
    {
        path = tokens[0];
    }
    return path;
}

void executeCommand(char **tokens)
{
    char *path = getCompletePath(tokens);
    struct stat buffer;
    if (stat(path, &buffer) != 0)
    {
        printf("%s not found\n", path);
        exit(-1);
    }
    else
    {
        execv(path, &tokens[0]);
    }
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
