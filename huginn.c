#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David");
MODULE_DESCRIPTION("Hello World module");

struct proc_dir_entry *proc;

static int first_proc(struct seq_file *m, void *v) {
    seq_printf(m, "First proc\n");
    return 0;
}

static int proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, first_proc, NULL);
}

static const struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init huginn_init(void) {
    printk(KERN_INFO "Hello, World! Module Loaded");
    proc = proc_create("hello_world_proc", 0, NULL, &proc_ops);
    if (proc == NULL) {
        return -ENOMEM;
    }
    return 0;
}

static void __exit huginn_exit(void) {
    remove_proc_entry("hello_world_proc", NULL);
    printk(KERN_INFO "Module unloaded");
}

module_init(huginn_init);
module_exit(huginn_exit);