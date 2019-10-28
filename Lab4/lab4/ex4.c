/*************************************
 * Lab 4 Exercise 4
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
#include <stdlib.h>
#include "api.h"

#define FRAME_NUM (1<<FRAME_BITS)
#define PAGE_NUM (1<<PAGE_BITS)
#define NEXT(i) ((i+1)%(FRAME_NUM))

typedef struct node {
    int val;
    struct node* next;
} node_t; // source: https://www.learn-c.org/en/Linked_lists

void os_run(int initial_num_pages, page_table *pg_table){
    // The main loop of your memory manager
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);

    int frame_page = -1;
    int victim = 0;
    int page_in_frame[FRAME_NUM];
    int mapped_page[PAGE_NUM];
    int page_created[PAGE_NUM];
    memset(page_in_frame, -1, sizeof(page_in_frame));
    memset(mapped_page, 0, sizeof(mapped_page));
    memset(page_created, 0, sizeof(page_created));

    node_t *head = NULL;
    head = malloc(sizeof(node_t));
    node_t *ptr = head;
    
    // create the pages in the disk first, because every page must be backed by the disk for this lab
    for (int i=0; i!=initial_num_pages; ++i) {
        mapped_page[i] = 1;
    }

    // initialize the linked list
    for (int i=initial_num_pages; i<PAGE_NUM; i++) {
        ptr->val = i;
        if (i != PAGE_NUM-1) {
            node_t *nxt = malloc(sizeof(node_t));
            ptr->next = nxt;
            ptr = nxt;
        } else {
            ptr->next = NULL;
        }
    }
    
    while (1) {
        siginfo_t info;
        sigwaitinfo(&signals, &info);
        if(info.si_signo == SIGUSR1) {
            // retrieve the index of the page that the user program wants, or -1 if the user program has terminated
            int const requested_page = info.si_value.sival_int;
            
            if (requested_page == -1) break;

            if (requested_page < 0 || requested_page >= PAGE_NUM
                || mapped_page[requested_page] == 0) {
                union sigval reply_value;
                // set to 1 if the page is not mapped to the user process (i.e. segfault)
                reply_value.sival_int = 1;
                sigqueue(info.si_pid, SIGCONT, reply_value);
                continue;
            }
            
            // process the signal, and update the page table as necessary
            while(page_in_frame[victim] != -1 
                    && pg_table->entries[page_in_frame[victim]].referenced == 1) {
                pg_table->entries[page_in_frame[victim]].referenced = 0;
                victim = NEXT(victim);
            }

            if(page_in_frame[victim] != -1) {
                pg_table->entries[page_in_frame[victim]].valid = 0;
                pg_table->entries[page_in_frame[victim]].referenced = 0;
                if(pg_table->entries[page_in_frame[victim]].dirty == 1) {
                    if (page_created[page_in_frame[victim]] == 0) {
                        disk_create(page_in_frame[victim]);
                        page_created[page_in_frame[victim]] = 1;
                    }
                    disk_write(victim, page_in_frame[victim]);
                    mapped_page[page_in_frame[victim]] = 1;
                }
            }

            if (page_created[requested_page] == 0) {
                disk_create(requested_page);
                page_created[requested_page] = 1;
            }
            disk_read(victim, requested_page);
            pg_table->entries[requested_page].valid = 1;
            pg_table->entries[requested_page].referenced = 0;
            pg_table->entries[requested_page].frame_index = victim;
            pg_table->entries[requested_page].dirty = 0;
            mapped_page[requested_page] = 1; // ex3
            page_in_frame[victim] = requested_page;
            victim = NEXT(victim);
            
            // tell the MMU that we are done updating the page table
            union sigval reply_value;
            reply_value.sival_int = 0; // set to 0 if the page is successfully loaded, set to 1 if the page is not mapped to the user process (i.e. segfault)
            sigqueue(info.si_pid, SIGCONT, reply_value);

        } else if(info.si_signo == SIGUSR2) {
            if(info.si_value.sival_int == -1) {
                // mmap
                if(head == NULL) {
                    printf("error when creating a page on disk\n");
                } else {
                    mapped_page[head->val] = 1;
                    union sigval reply_value;
                    reply_value.sival_int = head->val;
                    head = head->next;
                    sigqueue(info.si_pid, SIGCONT, reply_value);
                }
            } else {
                // unmap
                int unmap_page = info.si_value.sival_int;
                if(mapped_page[unmap_page] != 0) {
                    // unmap_page is mapped to a frame
                    pg_table->entries[unmap_page].valid = 0;
                    pg_table->entries[unmap_page].referenced = 0;
                    pg_table->entries[unmap_page].dirty = 0;
                    disk_delete(unmap_page);
                    page_created[unmap_page] = 0;
                    mapped_page[unmap_page] = 0;
                    node_t *new_node = malloc(sizeof(node_t));
                    new_node->val = unmap_page;
                    new_node->next = head;
                    head = new_node;
                }
                
                union sigval reply_value;
                reply_value.sival_int = 0;
                sigqueue(info.si_pid, SIGCONT, reply_value);
            }
        } else {
            printf("unexpected signal\n");
            break;
        }
    }
}