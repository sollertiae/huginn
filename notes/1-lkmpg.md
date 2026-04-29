# Chapter 1

## What is a kernel module?

A module is a program which can be dynamically loaded and unloaded within the kernel on demand. And their purpose is to extend the capabilities of the kernel without the need of rebooting the system. Modular design allows to keep the kernel lean, patch and extend functionality without rebuilding.

- There are kernel headers we need to use and we deviate from standard userspace functions such as printf.

- We can't observe kernel outputs directly through stdout as the output goes into the kernel ring buffer, it can be read with `dmesg`.

- Each kernel module must have an init function and exit function, nowadays we are able to name the functions as we like, we simply need to use the macros `module_init()` and `module_exit()`

- The macro `__init` is used to place the function in init only, once the function is executed the memory is freed and `__exit` omits the cleanup function for builtin drivers since they 
can never be unloaded. For loadable modules both macros have no 
meaningful effect.

**Surprised me:**

- This is why Linux rarely needs reboots compared to Windows, most functionality lives in modules that can be swapped without touching the running kernel.

Useful to keep output: `sudo dmesg -w`

## Arguments in a module

Modules can use arguments passed at `insmod` time, using the `module_param()` macro, exposed through sysfs `/sys/module/<module_name>/parameters/` with defined permissions. For callback based parameter monitoring use 
`module_param_cb()`, it triggers custom get/set functions when the parameter is read or written via sysfs.

## sysfs interaction
- Read: `cat /sys/module/<name>/parameters/<param>`
- Write: `echo <value> | sudo tee /sys/module/<name>/parameters/<param>`

## Multi-file modules
Modules can be split across multiple source files. We can achieve this by implementing the following in the Makefile `obj-m += module.o` and `module-objs := file1.o file2.o` instead 
of a single source file.