# Chapter 1

## What is a kernel module?

A module is a program which can be dynamically loaded and unloaded within the kernel on demand. And their purpose is to extend the capabilities of the kernel without the need of rebooting the system. Modular design allows to keep the kernel lean, patch and extend functionality without rebuilding.

- There are kernel headers we need to use and we deviate from standard userspace functions such as printf.

- We can't observe kernel outputs directly through stdout as the output goes into the kernel ring buffer, it can be read with `dmesg`.

- Each kernel module must have an init function and exit function, nowadays we are able to name the functions as we like, we simply need to use the macros `module_init()` and `module_exit()`

## Surprised me: 
This is why Linux rarely needs reboots compared to Windows, most functionality lives in modules that can be swapped without touching the running kernel.