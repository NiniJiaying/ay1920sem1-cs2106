/*************************************
 * Lab 5 Ex3
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#include "my_stdio.h"
#include <sys/stat.h>

size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream) {
	if (stream->can_write != 1) {
		return -1;
	}

	int total_bytes = size * nmemb;
	int write_bytes = 0;

	while (1) {
		if (stream->write_buf_end == -1) {
			// write buffer is empty
			if (total_bytes < MY_BUFFER_SIZE) {
				memcpy(stream->write_buffer, ptr, total_bytes);
				ptr += total_bytes;
				stream->write_buf_end += total_bytes;
				write_bytes += total_bytes;
				total_bytes = 0;
				return write_bytes / size;
			} else {
				memcpy(stream->write_buffer, ptr, MY_BUFFER_SIZE);
				ptr += MY_BUFFER_SIZE;
				stream->write_buf_end = MY_BUFFER_SIZE-1;
				write_bytes += MY_BUFFER_SIZE;
				total_bytes -= MY_BUFFER_SIZE;
			}
		} else {
			if (stream->write_buf_end + total_bytes >= MY_BUFFER_SIZE-1) {
				int rem_bytes = MY_BUFFER_SIZE - stream->write_buf_end - 1;
				memcpy(stream->write_buffer + stream->write_buf_end+1, ptr, rem_bytes);
				ptr += rem_bytes;
				stream->write_buf_end += rem_bytes;
				write_bytes += rem_bytes;
				total_bytes -= rem_bytes;
			} else {
				memcpy(stream->write_buffer + stream->write_buf_end+1, ptr, total_bytes);
				ptr += total_bytes;
				stream->write_buf_end += total_bytes;
				write_bytes += total_bytes;
				total_bytes = 0;
				return write_bytes / size;
			}
		}


		if (stream->write_buf_end == MY_BUFFER_SIZE-1) {
			// write buffer is full, call write()
			if (stream->is_append) {
				lseek(stream->fd, 0, SEEK_END);
			}
			if (write(stream->fd, stream->write_buffer, MY_BUFFER_SIZE) == -1) {
				return -1;
			}
			stream->write_buf_end = -1;
		}
	}
	
	return write_bytes / size; // shouldn't reach here
}
