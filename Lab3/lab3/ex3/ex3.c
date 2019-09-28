/*************************************
 * Lab 3 Exercise 3
 * Name: 
 * Student No:
 * Lab Group:
 *************************************/

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

#include "traffic_synchronizer.h"

#include <stdio.h>
#define wait(i) pthread_mutex_lock(&mutex[i])
#define signal(i) pthread_mutex_unlock(&mutex[i])

//Using extern, you can use the global variables num_of_cars and num_of_segments from ex3_runner.c in your code.
extern int num_of_cars;
extern int num_of_segments;

// segment_struct** strs;
pthread_mutex_t *mutex;

void initialise()
{
    mutex = (pthread_mutex_t *)malloc((num_of_segments) * sizeof(pthread_mutex_t));
    for (int i = 0; i < num_of_segments; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
    }
}

void cleanup()
{
    for (int i = 0; i < num_of_segments; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
    }
    free(mutex);
}

void *car(void *car)
{
    //This function modeles a thread
    //A car:
    //   -should call enter_roundabout (...)
    //   -followed by some calls to move_to_next_segment (...)
    //   -finally call exit_roundabout (...)
    car_struct *self = (car_struct *)car;
    int cur = self->entry_seg;

    wait(cur);
    enter_roundabout(self);

    while(cur != self->exit_seg) {
        cur = NEXT(cur, num_of_segments);
        wait(cur);
        move_to_next_segment(self);
        signal(PREV(cur, num_of_segments));
    }

    exit_roundabout(self);
    signal(cur);
    signal(PREV(cur, num_of_segments));

    pthread_exit(NULL);
}
