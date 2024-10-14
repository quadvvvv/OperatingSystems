# Operating Systems Course Projects

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Unix](https://img.shields.io/badge/Unix-1e1e1e?style=for-the-badge&logo=unix&logoColor=white)

This repository contains a collection of projects completed for the Operating Systems course at University of Wisconsin-Madison during the Summer 2021 semester, under the instruction of Professor Yuvraj Patel. These projects demonstrate proficiency in various aspects of operating systems design and implementation.

## Projects Overview

1. **Unix Utilities (P1)** [120/120]
   - Implementation of simplified versions of Unix utilities: `look` and `rev`
   - Demonstrates file I/O, command-line argument parsing, and string manipulation in C

2. **Custom Shell (P2-B)** [94/120]
   - A custom command-line interpreter (shell) implementation named `mysh`
   - Features include interactive and batch modes, command execution, output redirection, and basic built-in commands

3. **xv6 Custom Round-Robin Scheduler (P3)** [120/120]
   - Implementation of a Compensated Round-Robin (CRR) scheduler for the xv6 operating system
   - Introduces variable time-slice lengths, process compensation, and new system calls

4. **xv6 Kernel Memory Encryption Pager (P4)** [36/120 -> 14/15]
   - Extension of the xv6 operating system to implement a kernel-managed memory encryption pager
   - Features include page encryption/decryption, clock algorithm for working set management, and new system calls for encrypted memory management
   - This was the most challenging project, redone at the semester's end with TA assistance to thoroughly cover the material

5. **Multi-Threaded Web Server with Shared Memory Statistics (P5)** [114/120]
   - A multi-threaded web server implementation in C
   - Showcases thread synchronization, shared memory management, and inter-process communication

## Note on Project Inclusion

This repository includes all 5 course projects (P1, P2-B, P3, P4, and P5). P2-A is not included due to its simplicity compared to the other projects.

## Key Learning Outcomes

- Proficiency in C programming for systems development
- Understanding of Unix/Linux system calls and low-level I/O operations
- Experience with process and thread management
- Implementation of scheduling algorithms
- Practical application of concurrency and synchronization concepts
- Exposure to operating system internals through xv6 modification
- Implementation of virtual memory systems and page table manipulation
- Design and implementation of security-enhanced memory management

## Acknowledgements

Special thanks to Professor Yuvraj Patel and the TAs for their guidance and instruction throughout this fast-paced course.

## Project Details

For detailed information on each project, please refer to the `P*.md` files in the respective project directories.
