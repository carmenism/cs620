Carmen St. Jean
CS 620, fall 2012
Homework 11 - PART TWO

RUNNING:

My program runs using stdin until EOF is encountered.  Therefore, the best 
way to run my program when your input file is input.txt is:

% make
% ./firstfit < input.txt

DATA STRUCTURE:

I made a struct called memory.  A memory item can represent either a hole or a
process, therefore it is very easy to switch a whole to a process (or vice 
versa).

The memory items can easily be used to build a double linked list, with
pointers to the last block of memory and the next block of memory.  There are
also pointers to the last process and the next process.

Therefore, I was able to make two lists essentially from one list in memory.
- memoryList, which is to be traversed using the next memory item
- firstProcess, which is to be traversed using the next process

ALGORITHM:

My algorithm really is a traversal of a linked list, always starting from the
beginning of the list, until a sufficient hole is found.  The hole is split
into a process and a hole.  If no hole was found, then all holes are moved to 
the end and the memory holes are coalesced.

OUTPUT:

My output gives the same exact values, but it prints the processes in the order
that they occur in memory rather than their chronological order.  I figured
chronological order is easily derived from the pid so it is more meaningful to
print the processes in the order in which they appear in memory.

E.g., your output:
Process list:
process 1: id=4, start location=722, size=890
process 2: id=6, start location=70, size=30

Versus my output:
Process list:
process 1: id = 6, start location = 70, size = 30
process 2: id = 4, start location = 722, size = 890

I hope that this is alright but I found this to be more useful for debugging 
purposes too.
