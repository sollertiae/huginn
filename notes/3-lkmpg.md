# Chapter 3
## Execution context and control flow

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

- `mutex` (mutual exclusions) - in most cases this is all we will need to prevent collision. It enforces ownership, only the owner of the mutex can release it. We must initialize it through `mutex_init` or using the macro `DEFINE_MUTEX`. During the period we wait for lock the task cannot be interrupted, we can use `mutex_lock_interruptible` to be able to interrupt with signals. 

- We also have functions like `mutex_lock_nexted` and `mutex_lock_interruptable_nexted` to handle nested 

- `spinlocks` will lock up the CPU and will take 100% of the resources. It should only be used on code that will execute in milliseconds. `atomic contexts` is a term used for when the kernel monopolizes CPU.

| Who shares the lock? | Use this |
|---------------------|----------|
| Process context only | `spin_lock()`, `mutex_lock()`|
| Process + softirq/tasklet | `spin_lock_bh()` |
| Process + hardware interrupt | `spin_lock_irqsave()` |
| Unknown / called from anywhere | `spin_lock_irqsave()` <- safe default |

- *Key deadlock risk*: if process context holds a plain `spin_lock()` and a hardware interrupt fires on the same CPU trying to acquire the same lock causing deadlock. Solution: use `spin_lock_irqsave()` in process context to disable interrupts while holding the lock.

- Mutex: can sleep, longer critical sections, process context only
- Spinlock: cannot sleep, short critical sections, safe in interrupt context

## read and write locks

These are specialized kinds of spinlocks, it is advised to keep anything we do within the lock short

`read_lock()` / `write_lock()` - multiple readers can hold simultaneously, writer blocks until all readers release

## atomic operations

`atomic_t` guarantees operations are indivisible, no partial state visible to other CPUs. Use for counters and flags shared between contexts.

