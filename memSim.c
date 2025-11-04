#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memSim.h"

TLBEntry tlb[TLB_SIZE];
PageTableEntry page_table[PAGE_TABLE_SIZE];
char physical_mem[256][FRAME_SIZE];
Frame frame_info[256];
int num_frames;
char *pra_algorithm;

// statistic tracking
int tlb_hits = 0;
int tlb_misses = 0;
int page_faults = 0;
int total_addresses = 0;

// counters
int tlb_fifo_index = 0;
int next_frame = 0;
int time_counter = 0;

// initialize all structs in header file
void initialize_tlb() {}

void initializePT() {}

int search_tlb(int page_num) {}

void update_tlb(int page_num, int frame_num) {}

int fifo_replacement() {}

int lru_replacement() {}

int opt_replacement(int *future_refs, int future_count) {}

int handle_page_fault(intpage_num, FILE *backing_store, int *future_refs, int future_count) {}

void translate_address(int logical_address, FILE *backing_store, int *future_refs, int future_count) {}

void print_stats() {
  double page_fault_rate = (double)page_faults / total_addresses * 100.0;
  double tlb_miss_rate = (double)tlb_misses / total_addresses * 100.0;

  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", page_faults);
  printf("Page Fault Rate = %.3f\n", page_fault_rate);
  printf("TLB Hits = %d\n", tlb_hits);
  printf("TLB Misses = %d\n", tlb_misses);
  printf("TLB Hit Rate = %.3f\n", 100.0 - tlb_miss_rate);
}
