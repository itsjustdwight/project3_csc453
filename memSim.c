#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

