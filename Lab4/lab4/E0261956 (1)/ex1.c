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
#include <string.h>
#include <stdio.h>
#include "api.h"

#define FRAME_NUM (1<<FRAME_BITS)
#define PAGE_NUM (1<<PAGE_BITS)
#define NEXT(i) ((i+1)%(1<<FRAME_BITS))

void os_run(int initial_num_pages, page_table *pg_table){
    // The main loop of your memory manager
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    
    // create the pages in the disk first, because every page must be backed by the disk for this lab
    for (int i=0; i!=initial_num_pages; ++i) {
        disk_create(i);
    }
    
    int frame_page = -1;
    int victim = 0;
    int page_in_frame[FRAME_NUM];
    for(int i=0;i<FRAME_NUM;i++) {
        page_in_frame[i] = -1;
    }
    
    while (1) {
        siginfo_t info;
        sigwaitinfo(&signals, &info);

        if (info.si_signo == SIGUSR1) {
            // retrieve the index of the page that the user program wants, or -1 if the user program has terminated
            int const requested_page = info.si_value.sival_int;
            
            if (requested_page == -1) break;
            
            // process the signal, and update the page table as necessary
            while(page_in_frame[victim] != -1 
                && pg_table->entries[page_in_frame[victim]].referenced == 1) {
                pg_table->entries[page_in_frame[victim]].referenced = 0;
                victim = NEXT(victim);
            }

            if(page_in_frame[victim] != -1) {
                pg_table->entries[page_in_frame[victim]].valid = 0;
            }
            disk_read(victim, requested_page);
            pg_table->entries[requested_page].valid = 1;
            pg_table->entries[requested_page].referenced = 0;
            pg_table->entries[requested_page].frame_index = victim;
            page_in_frame[victim] = requested_page;
            victim = NEXT(victim);
            
            // tell the MMU that we are done updating the page table
            union sigval reply_value;
            reply_value.sival_int = 0; // set to 0 if the page is successfully loaded, set to 1 if the page is not mapped to the user process (i.e. segfault)
            sigqueue(info.si_pid, SIGCONT, reply_value);
        }
        
    }
}