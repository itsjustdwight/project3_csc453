#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memSim.h"


// statistic tracking
int total_addresses = 0;

// counters
int time_counter = 0;

// all function definitions

//initialize tlb
void init_tlb(TLB *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
	tlb->entries[i].page_num = -1;
	tlb->entries[i].frame_num = -1;
	tlb->entries[i].valid = false;
    }
    
    tlb->fifo_index = 0;
    tlb->hits = 0;
    tlb->misses = 0;
}

// lookup TLB page
int tlb_lookup(TLB *tlb, int page_num) {}

// insert into TLB
void tlb_insert(TLB *tlb, int page_num, int frame_num) {}

// invalidates entry in TLB
void tlb_invalidate(TLB *tlb, int page_num) {}

// initializes page table
void init_page_table(PageTable *pt) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
	pt->entries[i].frame_num = -1;
	pt->entries[i].loaded = false;
    }
}

// looks up page in page table
int page_table_lookup(PageTable *pt, int page_num) {}

// delete page table from page
void page_table_evict(PageTable *pt, int page_num) {}

// initialize physical memory
void init_physical_memory(PhysicalMemory *pm, int num_frames) {}

// check if free frames, if not return false
bool has_free_frame(PhysicalMemory *pm) {}

// get next free frame
int get_free_frame(PhysicalMemory *pm) {}

// loads page into a specific frame
void load_page_to_frame(PhysicalMemory *pm, int frame_num, int page_num, unsigned char *page_data) {}

// initializes fifo queue
void init_fifo_queue(FIFOQueue *queue, int capacity) {}

// adds frame into queue
void fifo_enqueue(FIFOQueue *queue, int frame_num) {}

// removes frame from queue
int fifo_dequeue(FIFOQueue *queue) {}

void print_stats(MemorySimulator *sim) {
  double page_fault_rate = (double)sim->page_faults / total_addresses * 100.0;
  double tlb_miss_rate = (double)sim->tlb.misses / total_addresses * 100.0;

  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", sim->page_faults);
  printf("Page Fault Rate = %.3f\n", page_fault_rate);
  printf("TLB Hits = %d\n", sim->tlb.hits);
  printf("TLB Misses = %d\n", sim->tlb.misses);
  printf("TLB Hit Rate = %.3f\n", 100.0 - tlb_miss_rate);
}
