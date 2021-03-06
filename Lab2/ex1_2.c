/*************************************
 * Lab 2 Exercise 1
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for fork(), wait()
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void readFromFile(char who[50], FILE* fptr);

int main(int argc, char **argv)
{
    char *fileName = NULL;
    int nChild = 0;
    char who[50];
    FILE* file;

    if (argc < 3)
    {
        printf("Usage: %s <filename> <number_of_processes>\n", argv[0]);
        return 1;
    }

    fileName = argv[1];
    nChild = atoi(argv[2]);

    file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("Unable to open file %s, exiting\n", fileName);
        return -1;
    }

    char charBuf = 0;
    char strBuf[128];
    fread(&charBuf, sizeof(char), 1, file);
    sprintf(who, "Parent [%d]", getpid());
    sprintf(strBuf, "%s: %c\n", who, charBuf);
    write(STDIN_FILENO, strBuf, strlen(strBuf));
    // readFromFile(who, fd);
    

    for (int i = 1; i <= nChild; i++)
    {
        int cur = fork();
        
        if(cur == 0) {
            // if currently in a child process
            char childWho[50];
            sprintf(childWho, "Child %d[%d]", i, getpid());
            readFromFile(childWho, file);
            fclose(file);
            exit(0);
        }
    }

    readFromFile(who, file);
    fclose(file);

    //TODO add your code
    for (int i = 1; i <= nChild; i++)
    {
        int pid = wait(NULL);
        printf("Parent: Child %d[%d] done.\n", i, pid);
    }

    printf("Parent: Exiting\n");
}

void readFromFile(char who[50], FILE* fptr)
{
    ssize_t readBytes = 1;
    char charBuf = 0;
    char strBuf[128];

    while (readBytes > 0)
    {
        usleep(1000);
        charBuf = 0;
        readBytes = fread(&charBuf, sizeof(char), 1, fptr);

        if (readBytes != 1)
        {
            if (readBytes == 0)
            {
                // printf("EOF\n");
                return;
            }
        }
        sprintf(strBuf, "%s: %c\n", who, charBuf);
        write(STDOUT_FILENO, strBuf, strlen(strBuf));
    }
}
