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

	if (!(stream->read_buf_start == -1 || stream->read_buf_start > stream->read_buf_end)) {
		lseek(stream->fd, -(stream->read_buf_end - stream->read_buf_start + 1), SEEK_CUR);
		stream->read_buf_start = -1;
		stream->read_buf_end = -1;
		return 0;
	}

	if (stream->can_write == 1) {
		if (stream->write_buf_end != -1) {
			if (stream->is_append) {
				lseek(stream->fd, 0, SEEK_END);
			}
			if (write(stream->fd, stream->write_buffer, stream->write_buf_end+1) == -1) {
				return MY_EOF;
			}
			stream->write_buf_end = -1; // clear the write buffer
		}
		return 0;
	}

	return 0;
}

int my_fseek(MY_FILE *stream, long offset, int whence) {
	my_fflush(stream);
	return lseek(stream->fd, offset, whence);
}
