#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif
#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "ProcFSName"

static struct proc_dir_entry *my_proc_file;

static char procfs_buffer[PROCFS_MAX_SIZE];

static unsigned long procfs_buffer_size = 0;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset
                             ) {
	if (*offset >= procfs_buffer_size) {
		return 0;
	}
	
	size_t data_to_copy = min(buffer_length, procfs_buffer_size - (size_t)*offset);
	if (copy_to_user(buffer, procfs_buffer + *offset, data_to_copy))
		return -EFAULT;
	
	*offset += data_to_copy;
	pr_info("procfile read %zu bytes\n", data_to_copy);
	return data_to_copy;
}

static ssize_t procfile_write(struct file *file, 
                              const char __user *buff, 
                              size_t len, loff_t *off
	) {
	procfs_buffer_size = len;
	if (procfs_buffer_size >= PROCFS_MAX_SIZE)
		procfs_buffer_size = PROCFS_MAX_SIZE - 1;
	
	if(copy_from_user(procfs_buffer, buff, procfs_buffer_size))
		return -EFAULT;
	
	procfs_buffer[procfs_buffer_size] = '\0';
	*off += procfs_buffer_size;
	pr_info("procfile write %s\n", procfs_buffer);
	
	return procfs_buffer_size;
}

#ifdef HAVE_PROC_OPS

static const struct proc_ops proc_file_fops = {
	.proc_read = procfile_read,
	.proc_write = procfile_write,
};
#else
static const struct file_operations proc_file_ops = {
	.read = procfile_read,
	.write = procfile_write,
};
#endif

static int __init procfs2_init(void) {
	my_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
	if (my_proc_file == NULL) {
		pr_alert("Error initializing /proc/%s\n", PROCFS_NAME);
		return -ENOMEM;
	}
	
	pr_info("successfully initialized /proc/%s\n", PROCFS_NAME);
	return 0;
}

static void __exit procfs2_exit(void) {
	proc_remove(my_proc_file);
	pr_info("Removed /proc/%s\n", PROCFS_NAME);
}

module_init(procfs2_init);
module_exit(procfs2_exit);

MODULE_LICENSE("GPL");
