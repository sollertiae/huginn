#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define procfs_name "first_proc"

static struct proc_dir_entry *my_proc_file;

static ssize_t procfile_read(struct file *file_pointer,
    char __user *buffer, size_t buffer_length, loff_t * offset
) {
    char str[11] = "FirstProc\n";
    int len = sizeof(str);
    ssize_t ret = len;

    if (*offset >= len || copy_to_user(buffer, str, len)) {
        pr_info("copy_to_user failed");
        ret = 0;
    } else {
        pr_info("procfile read %s\n", 
            file_pointer->f_path.dentry->d_name.name);
        *offset += len;
    }
    return ret;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init procfs_init(void) {
    my_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
    if (my_proc_file == NULL) {
        pr_alert("Error initializing /proc %s\n", procfs_name);
        return -ENOMEM;
    }
    pr_info("/proc/%s created successfully\n", procfs_name);
    return 0;
}

static void __exit procfs_exit(void) {
    proc_remove(my_proc_file);
    pr_info("/proc/%s removed\n", procfs_name);
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");