/*************************************
 * Lab 5 Ex4
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#include "my_stdio.h"
#include <stdio.h>

int my_fflush(MY_FILE *stream) {
	if (stream->can_read == 1) {
		// clear the read buffer
		stream->read_buf_start = -1;
		stream->read_buf_end = -1;
	} 

	if (stream->can_write == 1) {
		if (stream->write_buf_end != -1) {
			// printf("clearing the write buffer\n");
			if (write(stream->fd, stream->write_buffer, stream->write_buf_end+1) == -1) {
				return MY_EOF;
			}
			stream->write_buf_end = -1; // clear the write buffer
		}
	}

	stream->offset = 0;
	// printf("offset: %d\n", stream->offset);

	return 0;
}

int my_fseek(MY_FILE *stream, long offset, int whence) {
	if (stream->can_read == 1) {
		// clear the read buffer
		stream->read_buf_start = -1;
		stream->read_buf_end = -1;
	}
	if (stream->can_write == 1) {
		if (stream->write_buf_end != -1) {
			if (write(stream->fd, stream->write_buffer, stream->write_buf_end+1) == -1) {
				return MY_EOF;
			}
			stream->write_buf_end = -1; // clear the write buffer
			stream->offset = lseek(stream->fd, offset, whence);
		}
	}
	lseek(stream->fd, stream->offset, SEEK_SET);
	return lseek(stream->fd, offset, whence);
}