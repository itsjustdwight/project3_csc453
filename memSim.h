#ifndef MEMSIM_H
#define MEMSIM_H

#include <stdbool.h>

#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define PAGE_TABLE_SIZE 256
#define TLB_SIZE 16
#define OFFSET_BITS 8
#define PAGE_BITS 8
#define OFFSET_MASK 0xFF
#define PAGE_MASK 0xFF00


typedef struct {
    int page_num;
    int frame_num;
    bool valid;
} TLBEntry;

typedef struct{
    TLBEntry entries[TLB_SIZE];
    int fifo_index;
    int hits;
    int misses;
} TLB;

typedef struct{
    int frame_num;
    bool loaded;
} PageTableEntry;

typedef struct {
    PageTableEntry entries[PAGE_TABLE_SIZE];
} PageTable;

typedef struct {
    unsigned char data [FRAME_SIZE];
    int page_num;
} Frame;

typedef struct {
    Frame *frames;
    int num_frames;
    int next_free_frame;
} PhysicalMemory;

typedef struct {
    int *queue;
    int front;
    int rear;
    int size;
    int capacity;
} FIFOQueue;

typedef struct {
    int *access_time;
    int current_time;
} LRUTracker;

typedef struct {
    unsigned char *data;
    size_t size;
} BackingStore;

typedef struct {
    TLB tlb;
    PageTable page_table;
    PhysicalMemory physical_memory;
    BackingStore backing_store;
    FIFOQueue fifo_queue;
    LRUTracker lru_tracker;
    int page_faults;
    char algorithm[10];
} MemorySimulator;



// initialize TLB
void init_tlb(TLB *tlb);

// lookup TLB page
int tlb_lookup(TLB *tlb, int page_num);

// insert TLB
void tlb_insert(TLB *tlb, int page_num, int frame_num);

// Invalidates TLB entry
void tlb_invalidate(TLB *tlb, int page_num);

// initializes page table
void init_page_table(PageTable *pt);

// looks up page in page table
int page_table_lookup(PageTable *pt, int page_num);

// insert page table page
void page_table_insert(PageTable *pt, int page_num, int frame_num);

// delete page table from page
void page_table_evict(PageTable *pt, int page_num);

// initialize physical memory
void init_physical_memory(PhysicalMemory *pm, int num_frames);

// check if free frames if not reuturn false
bool has_free_frame(PhysicalMemory *pm);

// get next free frame 
int get_free_frame(PhysicalMemory *pm);

//loads page into a specific frame
void load_page_to_frame(PhysicalMemory *pm, int frame_num, int page_num, unsigned char *page_data);

//initializes fifo queue
void init_fifo_queue(FIFOQueue *queue, int capacity);

//adds frame into queue
void fifo_enqueue(FIFOQueue *queue, int frame_num);

//removes frame from queue
int fifo_dequeue(FIFOQueue *queue);


#endif
