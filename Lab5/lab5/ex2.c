/*************************************
 * Lab 5 Ex2
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************
 Warning: Make sure your code works on
 lab machine (Linux on x86)
 *************************************/

#include "my_stdio.h"
#include <stdio.h>


/*
The function reads nmemb items of data, each size bytes long, from the
stream pointed to by stream, storing them at the location given by ptr. The function
returns the number of items read, or –1 if an error occurs.
 */

size_t my_fread(void *ptr, size_t size, size_t nmemb, MY_FILE *stream) {
	if (stream->can_read != 1) {
		return -1;
	}

	int total_bytes = size * nmemb;
	int read_bytes = 0;

	while (1) {
		if (stream->read_buf_start == -1 || stream->read_buf_start > stream->read_buf_end) {
			int read_count = read(stream->fd, stream->read_buffer, MY_BUFFER_SIZE);
			if (read_count == -1) {
				return -1;
			}
			if (read_count != 0) {
				stream->read_buf_start = 0;
				stream->read_buf_end = read_count-1;
			} else {
				stream->offset += read_bytes;
				// printf("offset: %d\n", stream->offset);
				return read_bytes / size;
			}
		}
		int buf_len = stream->read_buf_end - stream->read_buf_start + 1;
		if (buf_len >= total_bytes) {
			memcpy(ptr, stream->read_buffer + stream->read_buf_start, total_bytes);
			stream->read_buf_start += total_bytes;
			read_bytes += total_bytes;
			break;
		} else if (buf_len < size) {
			// whether to copy incomplete size_t to buf or not?
			stream->offset += read_bytes;
			// printf("offset: %d\n", stream->offset);
            return read_bytes / size;
        } else {
			memcpy(ptr, stream->read_buffer + stream->read_buf_start, buf_len);
			ptr += buf_len;
			stream->read_buf_start += buf_len;
			total_bytes -= buf_len;
			read_bytes += buf_len;
		}
	}

	stream->offset += read_bytes;
	// printf("offset: %d\n", stream->offset);
	return read_bytes / size;
}
