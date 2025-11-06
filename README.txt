Authors: Dwight Anthony Taylor Jr, Amarbir Janda
Project 3: CSC 453


Overview:
This project implemets a virtual memory simulator and that translates logical addresses
to physical addresses using a tlb, page table, physical memory, and backing store. 
Supports fifo, lru, and opt. 

To run the project:


1. make
2. ./memSim addresses.txt
3. Different Configs:
    ./memSim fifo3.txt 5 fifo
    ./memSim lru2.txt 5 lru
    ./memSim opt2.txt 5 opt

