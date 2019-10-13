
/*************************************
 * Lab 3 Exercise 1
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************/
#include <pthread.h>
#include "rw_lock.h"

void initialise(rw_lock* lock)
{
  sem_init(&(lock->mutex), 0, 1);
  sem_init(&(lock->isEmpty), 0, 1);
  lock->reader_count = 0;
  lock->writer_count = 0;
}

void writer_acquire(rw_lock* lock)
{
  sem_wait(&(lock->isEmpty));
  lock->writer_count++;
}

void writer_release(rw_lock* lock)
{
  lock->writer_count--;
  sem_post(&(lock->isEmpty));
}

void reader_acquire(rw_lock* lock)
{
  sem_wait(&(lock->mutex));
  if(lock->reader_count == 0) sem_wait(&(lock->isEmpty));
  lock->reader_count++;
  sem_post(&(lock->mutex));
}

void reader_release(rw_lock* lock)
{
  sem_wait(&(lock->mutex));
  lock->reader_count--;
  if(lock->reader_count == 0) sem_post(&(lock->isEmpty));
  sem_post(&(lock->mutex));
}

void cleanup(rw_lock* lock)
{
  sem_destroy(&(lock->mutex));
  sem_destroy(&(lock->isEmpty));
}
