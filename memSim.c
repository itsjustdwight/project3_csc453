#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memSim.h"

// Global time counter
int time_counter = 0;

// ============ TLB Functions ============

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

int tlb_lookup(TLB *tlb, int page_num) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].valid && tlb->entries[i].page_num == page_num) {
            tlb->hits++;
            return tlb->entries[i].frame_num;
        }
    }
    tlb->misses++;
    return -1;
}

void tlb_insert(TLB *tlb, int page_num, int frame_num) {
    tlb->entries[tlb->fifo_index].page_num = page_num;
    tlb->entries[tlb->fifo_index].frame_num = frame_num;
    tlb->entries[tlb->fifo_index].valid = true;
    tlb->fifo_index = (tlb->fifo_index + 1) % TLB_SIZE;
}

void tlb_invalidate(TLB *tlb, int page_num) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].valid && tlb->entries[i].page_num == page_num) {
            tlb->entries[i].valid = false;
            return;
        }
    }
}

// ============ Page Table Functions ============

void init_page_table(PageTable *pt) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pt->entries[i].frame_num = -1;
        pt->entries[i].loaded = false;
        pt->entries[i].last_access_time = 0;
    }
}

int page_table_lookup(PageTable *pt, int page_num) {
    if (pt->entries[page_num].loaded) {
        return pt->entries[page_num].frame_num;
    }
    return -1;
}

void page_table_insert(PageTable *pt, int page_num, int frame_num) {
    pt->entries[page_num].frame_num = frame_num;
    pt->entries[page_num].loaded = true;
    pt->entries[page_num].last_access_time = time_counter;
}

void page_table_evict(PageTable *pt, int page_num) {
    pt->entries[page_num].loaded = false;
    pt->entries[page_num].frame_num = -1;
}

// ============ Physical Memory Functions ============

void init_physical_memory(PhysicalMemory *pm, int num_frames) {
    pm->num_frames = num_frames;
    pm->next_free_frame = 0;
    pm->frames = (Frame *)malloc(num_frames * sizeof(Frame));
    for(int i = 0; i < num_frames; i++) {
        pm->frames[i].page_num = -1;
        pm->frames[i].last_access_time = 0;
        memset(pm->frames[i].data, 0, FRAME_SIZE);
    }
}

bool has_free_frame(PhysicalMemory *pm) {
    return pm->next_free_frame < pm->num_frames;
}

int get_free_frame(PhysicalMemory *pm) {
    if (has_free_frame(pm)) {
        return pm->next_free_frame++;
    }
    return -1;
}

void load_page_to_frame(PhysicalMemory *pm, int frame_num, int page_num, unsigned char *page_data) {
    pm->frames[frame_num].page_num = page_num;
    pm->frames[frame_num].last_access_time = time_counter;
    memcpy(pm->frames[frame_num].data, page_data, FRAME_SIZE);
}

// ============ FIFO Queue Functions ============

void init_fifo_queue(FIFOQueue *queue, int capacity) {
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = -1;
    queue->queue = (int *)malloc(capacity * sizeof(int));
}

void fifo_enqueue(FIFOQueue *queue, int frame_num) {
    if (queue->size < queue->capacity) {
        queue->rear = (queue->rear + 1) % queue->capacity;
        queue->queue[queue->rear] = frame_num;
        queue->size++;
    }
}

int fifo_dequeue(FIFOQueue *queue) {
    if (queue->size == 0) {
        return -1;
    }
    int frame_num = queue->queue[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return frame_num;
}

// ============ LRU Functions ============

void init_lru_tracker(LRUTracker *lru, int num_frames) {
    lru->access_time = (int *)malloc(num_frames * sizeof(int));
    lru->current_time = 0;
    for (int i = 0; i < num_frames; i++) {
        lru->access_time[i] = 0;
    }
}

void lru_update(LRUTracker *lru, int frame_num) {
    lru->current_time++;
    lru->access_time[frame_num] = lru->current_time;
}

int lru_find_victim(LRUTracker *lru, PhysicalMemory *pm) {
    int victim = 0;
    int min_time = pm->frames[0].last_access_time;
    
    for (int i = 1; i < pm->num_frames; i++) {
        if (pm->frames[i].last_access_time < min_time) {
            min_time = pm->frames[i].last_access_time;
            victim = i;
        }
    }
    return victim;
}

// ============ Backing Store Functions ============

bool load_backing_store(BackingStore *bs, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open backing store file %s\n", filename);
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    bs->size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    bs->data = (unsigned char *)malloc(bs->size);
    if (!bs->data) {
        fprintf(stderr, "Error: Cannot allocate memory for backing store\n");
        fclose(file);
        return false;
    }
    
    fread(bs->data, 1, bs->size, file);
    fclose(file);
    return true;
}

void read_page_from_backing_store(BackingStore *bs, int page_num, unsigned char *buffer) {
    int offset = page_num * PAGE_SIZE;
    memcpy(buffer, bs->data + offset, PAGE_SIZE);
}

// ============ OPT Algorithm Helper ============

int opt_find_victim(PhysicalMemory *pm, int *addresses, int total_addresses, int current_index) {
    int victim = 0;
    int farthest = current_index;
    
    for (int i = 0; i < pm->num_frames; i++) {
        int page = pm->frames[i].page_num;
        int next_use = -1;
        
        // Find when this page will be used next
        for (int j = current_index + 1; j < total_addresses; j++) {
            int future_page = (addresses[j] >> OFFSET_BITS) & 0xFF;
            if (future_page == page) {
                next_use = j;
                break;
            }
        }
        
        // If page is never used again, evict it immediately
        if (next_use == -1) {
            return i;
        }
        
        // Otherwise, track the page used farthest in the future
        if (next_use > farthest) {
            farthest = next_use;
            victim = i;
        }
    }
    
    return victim;
}

// ============ Simulator Functions ============

void init_simulator(MemorySimulator *sim, int num_frames, const char *algorithm) {
    init_tlb(&sim->tlb);
    init_page_table(&sim->page_table);
    init_physical_memory(&sim->physical_memory, num_frames);
    init_fifo_queue(&sim->fifo_queue, num_frames);
    init_lru_tracker(&sim->lru_tracker, num_frames);
    
    sim->page_faults = 0;
    strncpy(sim->algorithm, algorithm, sizeof(sim->algorithm) - 1);
    sim->algorithm[sizeof(sim->algorithm) - 1] = '\0';
}

int translate_address(MemorySimulator *sim, int logical_address, int *addresses, int total_addresses, int current_index) {
    time_counter++;
    
    int page_num = (logical_address >> OFFSET_BITS) & 0xFF;
    int offset = logical_address & OFFSET_MASK;
    
    // Check TLB first
    int frame_num = tlb_lookup(&sim->tlb, page_num);
    
    if (frame_num == -1) {
        // TLB miss - check page table
        frame_num = page_table_lookup(&sim->page_table, page_num);
        
        if (frame_num == -1) {
            // Page fault - need to load from backing store
            sim->page_faults++;
            
            unsigned char page_data[PAGE_SIZE];
            read_page_from_backing_store(&sim->backing_store, page_num, page_data);
            
            // Check if we have a free frame
            if (has_free_frame(&sim->physical_memory)) {
                frame_num = get_free_frame(&sim->physical_memory);
                if (strcmp(sim->algorithm, "fifo") == 0) {
                    fifo_enqueue(&sim->fifo_queue, frame_num);
                }
            } else {
                // Need to evict a page
                if (strcmp(sim->algorithm, "fifo") == 0) {
                    frame_num = fifo_dequeue(&sim->fifo_queue);
                    fifo_enqueue(&sim->fifo_queue, frame_num);
                } else if (strcmp(sim->algorithm, "lru") == 0) {
                    frame_num = lru_find_victim(&sim->lru_tracker, &sim->physical_memory);
                } else if (strcmp(sim->algorithm, "opt") == 0) {
                    frame_num = opt_find_victim(&sim->physical_memory, addresses, total_addresses, current_index);
                }
                
                // Evict the old page
                int old_page = sim->physical_memory.frames[frame_num].page_num;
                if (old_page != -1) {
                    page_table_evict(&sim->page_table, old_page);
                    tlb_invalidate(&sim->tlb, old_page);
                }
            }
            
            // Load the new page
            load_page_to_frame(&sim->physical_memory, frame_num, page_num, page_data);
            page_table_insert(&sim->page_table, page_num, frame_num);
        }
        
        // Update TLB
        tlb_insert(&sim->tlb, page_num, frame_num);
    }
    
    // Update access time for LRU
    sim->physical_memory.frames[frame_num].last_access_time = time_counter;
    sim->page_table.entries[page_num].last_access_time = time_counter;
    
    return (frame_num * FRAME_SIZE) + offset;
}

void print_stats(MemorySimulator *sim, int total_addresses) {
    double page_fault_rate = (double)sim->page_faults / total_addresses * 100.0;
    double tlb_miss_rate = (double)sim->tlb.misses / total_addresses * 100.0;
    
    printf("Number of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", sim->page_faults);
    printf("Page Fault Rate = %.3f\n", page_fault_rate);
    printf("TLB Hits = %d\n", sim->tlb.hits);
    printf("TLB Misses = %d\n", sim->tlb.misses);
    printf("TLB Hit Rate = %.3f\n", 100.0 - tlb_miss_rate);
}

void cleanup_simulator(MemorySimulator *sim) {
    free(sim->physical_memory.frames);
    free(sim->fifo_queue.queue);
    free(sim->lru_tracker.access_time);
    free(sim->backing_store.data);
}
