/*************************************
 * Lab 5 my_stdio.h
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

/*
 * CS2106 my_stdio Library
 */

#ifndef MY_STDIO_H
#define MY_STDIO_H

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define MY_EOF (-1)
#define MY_BUFFER_SIZE 4096


typedef struct {
	int fd;
	// TODO: Define the members of your structure
	int can_read;
	int can_write;
	int is_append;
	char *read_buffer;
	int read_buf_start;
	int read_buf_end;
	char *write_buffer;
	int write_buf_end; // the last index where write buffer is valid
} MY_FILE; 

MY_FILE *my_fopen(const char *pathname, const char *mode);
int my_fclose(MY_FILE *stream);

size_t my_fread(void *ptr, size_t size, size_t nmemb, MY_FILE *stream);
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream);
int my_fflush(MY_FILE *stream);
int my_fseek(MY_FILE *stream, long offset, int whence);

#endif /* MY_STDIO_H */
