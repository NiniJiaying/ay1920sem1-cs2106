/*************************************
 * Lab 3 Exercise 2
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************/


#include <pthread.h>

typedef struct {
  pthread_mutex_t mutex;
  pthread_mutex_t isEmpty;
  pthread_mutex_t turnstile;
  int reader_count;
  int writer_count;
} rw_lock;