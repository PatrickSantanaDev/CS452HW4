Using the FLTK library, this application visually renders a number of moles on a lawn through the use of n threads defined in the main.c file. 
Moles are created (green), whacked(red), and then expire.
The level of simultaneous mole creation is adjusted by means of the mtqMax variable in main.c (0 = unbounded). 
If n is much larger than mtqMax, there will be more threads waiting to produce moles, increasing congestion.

The program leverages an MT-safe wrapper module (mtq.c) around a double-ended queue module (deq.c) to ensure that the queue operations 
in main are safe for concurrent access by multiple threads.
It includes a structure (Mrep) with a mutex lock and condition variables to ensure thread safety and manage queue congestion. 
This MTQ is crucial for synchronizing the production and consumption of moles in the multi-threaded application. 

The threads.c module provides utility functions for creating and synchronizing threads. 
It includes create_individual_thread to allocate and initiate a single thread running a specified function, and create_threads for generating multiple threads executing the function.
The wait_individual_thread function allows for waiting on a single thread's completion and handling its memory cleanup, while wait_threads extends this functionality to an array of 
multiple threads. These functions are important for ensuring proper initiation, execution, and resource deallocation.

Instructions for Execution:

To compile and execute the program, type 

$ make ./wam
$ make run 

in the command line.

For memory leak checks using Valgrind with FLTK-related leak suppression, you can use the command 
$ valgrind --leak-check=full --suppressions=./fltk.supp ./wam


VALGRIND report:
[patricksantana@onyx hw4]$ valgrind --leak-check=full --suppressions=./fltk.supp ./wam
==2077946== Memcheck, a memory error detector
==2077946== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==2077946== Using Valgrind-3.19.0 and LibVEX; rerun with -h for copyright info
==2077946== Command: ./wam
==2077946==
==2077946==
==2077946== HEAP SUMMARY:
==2077946==     in use at exit: 709,029 bytes in 1,121 blocks
==2077946==   total heap usage: 14,075 allocs, 12,954 frees, 2,802,141 bytes allocated
==2077946==
==2077946== LEAK SUMMARY:
==2077946==    definitely lost: 0 bytes in 0 blocks
==2077946==    indirectly lost: 0 bytes in 0 blocks
==2077946==      possibly lost: 0 bytes in 0 blocks
==2077946==    still reachable: 1,243 bytes in 3 blocks
==2077946==         suppressed: 707,786 bytes in 1,118 blocks
==2077946== Reachable blocks (those to which a pointer was found) are not shown.
==2077946== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==2077946==
==2077946== For lists of detected and suppressed errors, rerun with: -s

