# Chapter 2

## Core Kernel Interfaces

This chpater is focused on basic execution flow and introduction to common entry points which are used to expose functionality to the userspace.

Unlike a "regular" program, which starts at main and will finish once all instructions have been completed. Kernel modules do have innit and exit calls,  which act as the entry point and the exit point respectively. Both are expected and should always be defined, regardless of the method we use to define them.

All acquired resources must be released on error to prevent memory leaks and system instability. Important to note the canonical kernel pattern uses `goto` clean up in **reverse** allocation order.

Benefits of this pattern:

- Avoids deeply nested if/else blocks
- Guarantees resource free in exact reverse order of acquisition

Register callbacks and interfaces only at the end of init, the kernel can invoke them immediately after registration, before the rest of the init completes.

Rule of thumb "register last, unregister first"

## Function availability

Compared to a regular program, where we import headers and then use functions which definitions do not enter the program until it gets dynamically linked on execution time, kernel modules operate differently, these get resolved upon running insmod or modprobe and the defintion for symbols comes from the kernel itself; therefore, the only external functions (system calls) we can use are the ones provided by the kernel `/proc/kallsyms` is a list of all exported kernel symbols with their addresses.

- Library functions -> users space, high level, practical interface, point to system calls

- System calls -> kernel mode, provided by kernel, user's behalf

- `strace` traces system calls made by a process, very useful for understanding what kernel interfaces a program uses.

## User space vs Kernel space

The kernel's role is to maintain order, organzing resource usage. This can be achieved using different modes, which vary depending on architecture, Intel architectures use four rings while Unix systems use two rings 0 and ring 3, ring 0 corresponds to kernel mode and ring 3 to user mode.

Think of functions mediating and executing system calls through an interface.

- Kernel code is inherently concurrent. 

- To avoid variable name clash we should declare global variables is to declare as static and use a prefix in lower case at start.

- Kernel modules share memory with the kernel itself, rather than having a space for itself

## Device Drivers

Device drivers provide functionality for hardware, connecting hardware and software and enabling the interaction between them without them knowing they interact with each other.

There are character devices and block devices, the latter have a buffer for requests and can determine the best order in which to respond to requests and blocks can only accept input and return output in chunks.

Using `ls -l` if the first character is c = character device, b = block device

Devices should be put in `/dev` as part of the convention, although you do not have to.