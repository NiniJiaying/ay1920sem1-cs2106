/*************************************
 * Lab 4 Exercise 1
 * Name: Tan Yuanhong
 * Student No: A0177903X
 * Lab Group: 07
 *************************************/

// You can modify anything in this file. 
// Unless otherwise stated, a line of code being present in this template 
//  does not imply that it is correct/necessary! 
// You can also add any global or local variables you need (e.g. to implement your page replacement algorithm).

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

#define NEXT(x, total) ((x+1) % total)

void os_run(int initial_num_pages, page_table *pg_table){
    // The main loop of your memory manager
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    
    // create the pages in the disk first, because every page must be backed by the disk for this lab
    for (int i=0; i!=initial_num_pages; ++i) {
        disk_create(i);
        pg_table->entries[i].frame_index = i % (1<<FRAME_BITS);
    }
    
    int frame_page = 0;
    int page_in_frame[1<<FRAME_BITS];
    memset(page_in_frame, -1, sizeof(page_in_frame));
    
    while (1) {
        siginfo_t info;
        sigwaitinfo(&signals, &info);
        
        // retrieve the index of the page that the user program wants, or -1 if the user program has terminated
        int const requested_page = info.si_value.sival_int;
        
        if (requested_page == -1) {
            // If the page required is -1, the user process has exited.
            // Do necessary cleanup and return from os_run() function.
            break;
        }
        // Otherwise a page fault has occurred

        // process the signal, and update the page table as necessary

        page_table_entry victim = pg_table->entries[frame_page];
        while(victim.valid == 1 && victim.referenced == 1) {
            victim.referenced = 0;
            frame_page = NEXT(frame_page, 1<<PAGE_BITS);
            victim = pg_table->entries[frame_page];
        }
        printf("victim index: %d\n", frame_page);
        int available_frame = -1;
        for(int i=0;i<(1<<FRAME_BITS);i++) {
            if(page_in_frame[i] == -1) {
                available_frame = i;
                break;
            }
        }
        if(available_frame != -1) {
            printf("avail: %d\n", available_frame);
            victim.frame_index = available_frame;
            page_in_frame[available_frame] = frame_page;
            disk_read(victim.frame_index, requested_page);
            // tell the MMU that we are done updating the page table
            union sigval reply_value;
            // set to 0 if the page is successfully loaded, set to 1 if the page is not mapped to the user process (i.e. segfault)
            reply_value.sival_int = 0; 
            sigqueue(info.si_pid, SIGCONT, reply_value);
        } else {
            union sigval reply_value;
            // set to 0 if the page is successfully loaded, set to 1 if the page is not mapped to the user process (i.e. segfault)
            reply_value.sival_int = 1;
            printf("failed!\n");
            sigqueue(info.si_pid, SIGCONT, reply_value);
        }
    }
}
    