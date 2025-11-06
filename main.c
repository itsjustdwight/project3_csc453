#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memSim.h"

void print_frame_hex(unsigned char *data, int size) {
    for (int i = 0; i < size; i++) {
        printf("%02X", data[i]);
    }
}

int main(int argc, char *argv[]) {
    // Default values
    char *address_file = NULL;
    int num_frames = 256;
    char algorithm[10] = "fifo";
    
    // Parse command line arguments
    if (argc < 2) {
        fprintf(stderr, "usage: memSim <reference-sequence-file.txt> <FRAMES> <PRA>\n");
        return 1;
    }
    
    address_file = argv[1];
    
    if (argc >= 3) {
        num_frames = atoi(argv[2]);
        if (num_frames <= 0 || num_frames > 256) {
            fprintf(stderr, "Error: FRAMES must be between 1 and 256\n");
            return 1;
        }
    }
    
    if (argc >= 4) {
        strncpy(algorithm, argv[3], sizeof(algorithm) - 1);
        algorithm[sizeof(algorithm) - 1] = '\0';
        
        if (strcmp(algorithm, "fifo") != 0 && 
            strcmp(algorithm, "lru") != 0 && 
            strcmp(algorithm, "opt") != 0) {
            fprintf(stderr, "Error: PRA must be 'fifo', 'lru', or 'opt'\n");
            return 1;
        }
    }
    
    // Initialize simulator
    MemorySimulator sim;
    init_simulator(&sim, num_frames, algorithm);
    
    // Load backing store
    if (!load_backing_store(&sim.backing_store, "BACKING_STORE.bin")) {
        return 1;
    }
    
    // Read addresses from file
    FILE *addr_file = fopen(address_file, "r");
    if (!addr_file) {
        fprintf(stderr, "Error: Cannot open address file %s\n", address_file);
        cleanup_simulator(&sim);
        return 1;
    }
    
    // First pass: count addresses and store them (needed for OPT algorithm)
    int capacity = 1000;
    int *addresses = (int *)malloc(capacity * sizeof(int));
    int total_addresses = 0;
    int addr;
    
    while (fscanf(addr_file, "%d", &addr) == 1) {
        if (total_addresses >= capacity) {
            capacity *= 2;
            addresses = (int *)realloc(addresses, capacity * sizeof(int));
        }
        addresses[total_addresses++] = addr;
    }
    fclose(addr_file);
    
    // Second pass: process each address
    for (int i = 0; i < total_addresses; i++) {
        int logical_address = addresses[i];
        translate_address(&sim, logical_address, addresses, total_addresses, i);
        
        int page_num = (logical_address >> OFFSET_BITS) & 0xFF;
        int offset = logical_address & OFFSET_MASK;
        int frame_num = page_table_lookup(&sim.page_table, page_num);
        
        // Get the value at the physical address
        signed char value = (signed char)sim.physical_memory.frames[frame_num].data[offset];
        
        // Print output: address, value, frame_num, frame_contents
        printf("%d, %d, %d, ", logical_address, value, frame_num);
        print_frame_hex(sim.physical_memory.frames[frame_num].data, FRAME_SIZE);
        printf("\n");
    }
    
    // Print statistics
    print_stats(&sim, total_addresses);
    
    // Cleanup
    free(addresses);
    cleanup_simulator(&sim);
    
    return 0;
}