# Execution context and control flow

## system calls

The process behind real communication to kernel and the one used by all processes is system calls. Generally speaking processes are not supposed to be able to access the kernel nor call kernel functions, this is ensured by CPU hardware with `protected mode`/`page protection`; however, system calls are an exception this. 

General workflow: process fills the registers with the appropriate values -> calls a special function -> jumps to a defined location in kernel -> hardware understands once you jump there it should be running as system kernel. If we want to change the way a system call works we need to write our own function and implement it, adding our own code and calling the original function and then change the pointer at `sys_call_table` to point to ours. The cleanup must restore the table back, it is worth noting that for x86 cr0 register has multiple flags, and to modify the `sys_call_table` we must first disable the WP flag, this must be done with custom assembly.

-`control-flow integrity` is a technique to prevent redirection code execution, the kernel patched multiple `control-flow enforcement` (CET).

-`kprobes` is a dynamic tracing and debugging tool

Modern kernels block this via:
- CONFIG_STRICT_KERNEL_RWX - kernel pages not writable
- CONFIG_SECURITY_LOCKDOWN_LSM - explicitly prevents kernel modification

## blocking processes and threads

-`wait_event_interruptible` - the process sleeps but can be awaken by signals (SIGINT), returns `-ERESTARTSYS` if interrupted, provides better UX
-`wait_event` - the process ignores signals, it cannot be interrupted while waiting, the process cannot be killed
-`O_NONBLOCK` - process needs a result immediately or not at all, returns `-EAGAIN`

## completions

Completions allows specific things to happen before others on a module having multiple threads, this will keep the possibility to have timeouts and interrupts while synchronizing our code.

1. initialize struct sync object
2. waiting through `wait_for_completion()`
3. signaling through a `complete()` call

## synchronization

There are multiple types of mutual exclusion kernel functions to prevent processes on different CPUs or threads to access the same memory.

