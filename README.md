# Huginn

Linux kernel module implementing an audit logging system. Userspace processes write log entries to `/dev/huginn`, the module tags the entry with PID, UID, timestamp and message, storing them in a ring buffer. The ring buffer holds 64 entries.

Logs are exposed via `/proc/huginn`

This is the result after studying kernel internals to understand how userpace and kernel interact and the meaning of this boundary from a security perspective.

## Usage:

```bash
# load the module
sudo insmod huginn.ko

# submit an event
echo "event" | sudo tee /dev/huginn

# read the logs
cat /proc/huginn
```

## Architecture

`/dev/huginn` - submit audit events from userspace
`/proc/huginn` - read the full timestamped audit log

## Log structure

Each log entry captures:
- Timestamp
- PID of the writing process
- UID of the writing process
- Message (up to 128 bytes)

## Known issues

Double print bug when log is full, pending.

## Goal

Huginn is meant to grow into a kernel module that exposes an 
interface between kernel space and userspace, a practical 
exploration of how the kernel and the programs running on top 
of it communicate. My goal is understanding that boundary 
deeply enough to reason about it from a security perspective.

## Notes

Documenting my [notes/](https://github.com/sollertiae/huginn/tree/main/notes) as I work through the 
[lkmpg](https://sysprog21.github.io/lkmpg/) written as understanding for future reference, not as summaries.

## Examples

Standalone kernel module examples from working through lkmpg are in [examples/](https://github.com/sollertiae/huginn/tree/main/examples).

