/**
 * Carmen St. Jean
 * CS 620, fall 2012
 * Homework 11 - PART ONE
 * December 3, 2012
 *
 * Simulating a simple memory manager based on the contiguous memory
 * allocation scheme.
 *
 */
#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

/**
 * Represents a block in memory - it can either represent a process or a hole.
 * The type is indicated with the isHole boolean.
 */
struct memory {
    int pid;
    int startLocation;
    int size;
    
    bool isHole;
    
    struct memory *prev;
    struct memory *next;
    
    struct memory *prevProcess;
    struct memory *nextProcess;
}; 

// Method declarations.
int memoryManager(int bytes);
void printMemoryState();
int allocate(int bytes, int pid);
int deallocate(int pid);
void coalesce();
void processCommand(char command);
void updateProcessLinks(struct memory *prevProcess, struct memory *newProcess);
void printAll();

// Variables to keep track of memory usage.
int memorySize = 0;
int usedBytes = 0;
int freeBytes = 0;

// Variables to keep track of number of holes and processes.
int numberHoles = 0;
int numberProcesses = 0;

// Pointers to keep track of lists.
struct memory *memoryList;
struct memory *firstProcess = NULL;

int main(void) {
    scanf("%d", &memorySize);

    if (memoryManager(memorySize)) { 
        char command;
        
        while (scanf("%c", &command) != EOF) {
            processCommand(command);
        }
    }

    return 0;
}

void processCommand(char command) {    
    if (command == 'A' || command == 'a') {
        int pid, size;
        
        scanf("%d %d", &size, &pid);
        allocate(pid, size);
    } else if (command == 'D' || command == 'd') {
        int pid;
        
        scanf("%d", &pid);
        deallocate(pid);
    } else if (command == 'P' || command == 'p') {
        printMemoryState();
    }    
}

/**
 * Creates a memory manager.
 */
int memoryManager(int bytes) {
    if (bytes <= 0) {
        perror("Specified bytes size should be greater than zero.\n");
        
        return -1;
    }
    
    memorySize = bytes;
    freeBytes = bytes;
    
    memoryList = malloc(sizeof(struct memory));    
    memoryList->pid = -1;
    memoryList->startLocation = 0;
    memoryList->size = bytes;    
    memoryList->isHole = true;    
    memoryList->prev = NULL;
    memoryList->next = NULL;  
    memoryList->prevProcess = NULL;
    memoryList->nextProcess = NULL;
    
    numberHoles = 1;
    
    return 1;
}

/**
 * Allocates these many bytes to the process with this id. Assumes that each
 * pid is unique to a process.
 * 
 * Returns 1 if successful.
 * Returns -1 if unsuccessful; prints an error indicating whether there 
 * wasn't sufficient memory or whether there you ran into external 
 * fragmentation.
 */
int allocate(int pid, int bytes) {
    if (bytes <= 0) {
        printf("Process size should be positive.\n");
        
        return -1;
    }
    
    if (bytes > freeBytes) {
        printf("Not enough memory space for process.\n");
        
        return -1;
    }
    
    struct memory *p = firstProcess;
    
    while (p) {
        if (p->pid == pid) {
            printf("Process with pid '%d' already exists.\n", pid);
                
            return -1;
        }
        
        p = p->nextProcess;
    }
    
    struct memory *m = memoryList;
    struct memory *prevProcess = NULL;
    
    while (m) {
        if (m->isHole) {
            if (m->size == bytes) {
                // this hole becomes the process
                m->pid = pid;
                m->isHole = false;                           
                
                updateProcessLinks(prevProcess, m);
                
                freeBytes = freeBytes - bytes;
                usedBytes = usedBytes + bytes;                
                numberHoles--;
                numberProcesses++;
                
                return 1;
            } else if (m->size > bytes) {
                // change this hole to the process and add a new hole
                struct memory *newHole = malloc(sizeof(struct memory));
                newHole->pid = -1;
                newHole->size = m->size - bytes;
                newHole->startLocation = m->startLocation + bytes;
                newHole->isHole = true;
                
                newHole->next = m->next;
                newHole->prev = m;
                
                if (newHole->next) {
                    newHole->next->prev = newHole;
                }
                
                m->next = newHole;
                m->pid = pid;
                m->isHole = false;
                m->size = bytes;
                
                updateProcessLinks(prevProcess, m);
                
                freeBytes = freeBytes - bytes;
                usedBytes = usedBytes + bytes;
                numberProcesses++;
                
                return 1;
            }        
        } else {
            prevProcess = m;
        }
        
        m = m->next;
    }
    
    printf("Due to external fragmentation, can't allocate");
    printf(" memory space to process %d.\n", pid);
    
    return -1;         
}

/**
 * Updates the links between processes.
 */
void updateProcessLinks(struct memory *prevProcess, struct memory *newProcess)
{
    if (!prevProcess) {
        // no processes occured before
        
        if (!firstProcess) {
            // this is very first and only process
            firstProcess = newProcess;
            
            firstProcess->nextProcess = NULL;
            firstProcess->prevProcess = NULL;
        } else {
            // this should be the first process, the current first process will
            // become the second process
            newProcess->nextProcess = firstProcess;
            firstProcess->prevProcess = newProcess;
            firstProcess = newProcess;
        }
    } else {
        // newProcess is inserted after first item
        newProcess->prevProcess = prevProcess;
        newProcess->nextProcess = prevProcess->nextProcess;
        
        prevProcess->nextProcess = newProcess;
        
        if (newProcess->nextProcess) {
            newProcess->nextProcess->prevProcess = newProcess;
        }
    }   
}

/**
 * Deallocates memory allocated to this process.
 * 
 * Returns 1 if successful, -1 otherwise with an error message.
 */
int deallocate(int pid) {
    struct memory *p = firstProcess;
    
    while (p) {
        if (!p->isHole && p->pid == pid) {
            p->isHole = true;
            
            if (firstProcess == p) {
                // this process is the first process
                firstProcess = p->nextProcess;
                
                if (firstProcess) {
                    firstProcess->prevProcess = NULL;
                }
            } else if (!p->nextProcess) {
                // this process is the last process
                p->prevProcess->nextProcess = NULL;
            } else {
                p->prevProcess->nextProcess = p->nextProcess;
                p->nextProcess->prevProcess = p->prevProcess;
            }
                      
            freeBytes = freeBytes + p->size;
            usedBytes = usedBytes - p->size;
            numberHoles++;
            numberProcesses--;
                      
            coalesce();
          
            return 1;
        }
        
        p = p->nextProcess;
    }    
    
    printf("Process with pid '%d' does not exist.\n", pid);
    
    return -1;
}

/**
 * Joins adjacent holes of memory together.
 */
void coalesce() {
    struct memory *h = memoryList;
    
    while (h) {
        if (h->isHole) {
            struct memory *next = h->next;
            
            while (next && next->isHole) {
                // this segment and the next are both holes, they should become
                // one hole
                h->size = h->size + next->size;
                
                h->next = h->next->next;
                
                if (h->next) {
                   h->next->prev = h;
                }
                
                free(next);
                
                numberHoles--;
                
                next = h->next;
            }
        }
        
        h = h->next;
    }
}

/**
 * Prints out current state of memory.
 *  Example: 
 *    Memory size = 1024 bytes, allocated bytes = 24, free = 1000
 *    There are currently 10 holes and 3 active process
 *     Hole list:
 *     hole 1: start location = 0, size = 202
 *     ...
 *     Process list:
 *     process  id=34, start location=203, size=35
 *     ...
 */
void printMemoryState() {
    printf("Memory size = %d bytes, allocated bytes = %d, free = %d\n", 
           memorySize, usedBytes, freeBytes);
    
    printf("There's currently %d holes and %d processes:\n", 
           numberHoles, numberProcesses);
    
    printf("Hole list:\n");
    
    struct memory *h = memoryList;
    int holeNum = 1;
    
    while (h) {
        if (h->isHole) {
            printf("hole %d: start location = %d, size = %d\n", 
                    holeNum, h->startLocation, h->size);
            
            holeNum++;     
        }
        
        h = h->next;
    }
    
    struct memory *p = firstProcess;
    int processNum = 1;    
    
    printf("Process list:\n");
    
    while (p) {
        printf("process %d: id = %d, start location = %d, size = %d\n", 
                processNum, p->pid, p->startLocation, p->size);
        
        processNum++;
        p = p->nextProcess;
    }
}

/**
 * For testing purposes, prints out all items in memory in order.
 */
void printAll() {
    printf("\n************\n");

    int prevEnd = 0;

    struct memory *h = memoryList;
    while (h) {
        if (h->isHole) {
            printf("hole: start location = %d, size = %d\n", 
                    h->startLocation, h->size);    
        } else {
            printf("process: id = %d, start location = %d, size = %d\n", 
                h->pid, h->startLocation, h->size);
        }
        
        if (prevEnd != h->startLocation) {
            perror("PREVIOUS END DOES NOT MATCH START!");
            exit(-1);
        }
        
        prevEnd = prevEnd + h->size;
        
        h = h->next;
    }
    
    printf("\n************\n");
}
