/*************************************
 * Lab 3 Exercise 3
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************/

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

#include "traffic_synchronizer.h"

#include <stdio.h>
#define wait(i) pthread_mutex_lock(&mutex[i])
#define signal(i) pthread_mutex_unlock(&mutex[i])
#define next(i) NEXT(i, num_of_segments)
#define prev(i) PREV(i, num_of_segments)

//Using extern, you can use the global variables num_of_cars and num_of_segments from ex3_runner.c in your code.
extern int num_of_cars;
extern int num_of_segments;

// segment_struct** strs;
pthread_mutex_t *mutex;
pthread_mutex_t *safeToEnter;
sem_t limit_sem;

void initialise()
{
    sem_init(&limit_sem, 0, num_of_segments-1);
    mutex = (pthread_mutex_t *)malloc((num_of_segments) * sizeof(pthread_mutex_t));
	safeToEnter = (pthread_mutex_t *) malloc ( (num_of_segments) * sizeof(pthread_mutex_t) );
    for (int i = 0; i < num_of_segments; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
		pthread_mutex_init(&safeToEnter[i], NULL);
    }
}

void cleanup()
{
    for (int i = 0; i < num_of_segments; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
        pthread_mutex_destroy(&safeToEnter[i]);
    }
    sem_destroy(&limit_sem);
    free(mutex);
    free(safeToEnter);
}


void* car(void* car)
{
    //This function modeles a thread
    //A car:
    //   -should call enter_roundabout (...)
    //   -followed by some calls to move_to_next_segment (...)
    //   -finally call exit_roundabout (...)
    car_struct *self = (car_struct *)car;

    sem_wait(&limit_sem);
    pthread_mutex_lock(&safeToEnter[self->entry_seg]);
    wait(self->entry_seg);
    pthread_mutex_lock(&safeToEnter[next(self->entry_seg)]);
    enter_roundabout(self);
    pthread_mutex_unlock(&safeToEnter[self->entry_seg]);

    while(self->current_seg != self->exit_seg) {
        wait(NEXT(self->current_seg, num_of_segments));
	    if(next(self->current_seg) != self->exit_seg)
            pthread_mutex_lock(&safeToEnter[next(next(self->current_seg))]);
        move_to_next_segment(self);
        pthread_mutex_unlock(&safeToEnter[self->current_seg]);
	    signal(prev(self->current_seg));
    }

    exit_roundabout(self);
    signal(self->exit_seg);
    // pthread_mutex_unlock(&safeToEnter[self->exit_seg]);
    // signal(PREV(self->current_seg, num_of_segments));
    sem_post(&limit_sem);

    pthread_exit(NULL);
}
