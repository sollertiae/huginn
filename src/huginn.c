#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/rwlock.h>
#include <linux/cdev.h>
#include <linux/pid.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define MODULE_NAME "huginn"
#define MAX_LOGS 64
#define LOG_SIZE 128

static dev_t dev_num;
static struct cdev huginn_cdev;
static struct class *huginn_class;

static struct proc_dir_entry *huginn_proc;

static atomic_t open_count = ATOMIC_INIT(0);

struct huginn_log {
	pid_t pid;
	uid_t uid;
	u64 timestamp;
	char message[LOG_SIZE];
};

static struct huginn_log events[MAX_LOGS];
static size_t head = 0;
static size_t count = 0;
static DEFINE_RWLOCK(events_lock);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David");
MODULE_DESCRIPTION("Logger device");

/*
  huginn proc functions
 */

/*
  TODO: fix MAX_LOGS being doubled
 */
static void *huginn_seq_strt(struct seq_file *s, loff_t *pos) {
	if (*pos >= count)
		return NULL;
	
	size_t idx = (head - count + *pos + MAX_LOGS) % MAX_LOGS;
	return &events[idx];
}

static void *huginn_seq_nxt(struct seq_file *s, void* v, loff_t *pos) {
	(*pos)++;
	if (*pos >= count)
		return NULL;
	
	size_t idx = (head - count + *pos + MAX_LOGS) % MAX_LOGS;
	return &events[idx];
}

static void huginn_seq_stp(struct seq_file *s, void *v) {
}

static int huginn_seq_shw(struct seq_file *s, void *v) {
	struct huginn_log *log = (struct huginn_log *)v;
	seq_printf(s, "[%llu] PID: %d UID: %d - %s\n",
		   log->timestamp,
		   log->pid,
		   log->uid,
		   log->message);
	return 0;
}

static struct seq_operations huginn_seq_ops = {
	.start = huginn_seq_strt,
	.next = huginn_seq_nxt,
	.stop = huginn_seq_stp,
	.show = huginn_seq_shw,
};

static int huginn_proc_o(struct inode *inode, struct file *file) {
	return seq_open(file, &huginn_seq_ops);
}

static struct proc_ops huginn_proc_ops = {
	.proc_open = huginn_proc_o,
	.proc_read = seq_read,
	.proc_release = seq_release,
	.proc_lseek = seq_lseek,
};

/*
  huginn device functions
 */

static int huginn_open(struct inode *inode, struct file *file) {
	atomic_inc(&open_count);
	pr_info("huginn: opened %d times\n", atomic_read(&open_count));
	return 0;
}

static int huginn_release(struct inode *inode, struct file *file) {
	atomic_dec(&open_count);
	return 0;
}

static ssize_t huginn_write(struct file *file,
			    const char __user *buff,
			    size_t len,
			    loff_t *offset)
{
	int ret;
	unsigned long flags;
	size_t to_copy = min(len, (size_t)LOG_SIZE - 1);
	
	write_lock_irqsave(&events_lock, flags);
	ret = copy_from_user(events[head].message, buff, to_copy);
	if (ret != 0) {
		pr_err("huginn: error inserting log");
		goto err_copy_usr;
	}
	events[head].pid = current->pid;
	events[head].uid = current_uid().val;
	events[head].timestamp = ktime_get_ns();
	events[head].message[to_copy] = '\0';
	head = (head + 1) % MAX_LOGS;
	if (count < MAX_LOGS)
		count++;
        write_unlock_irqrestore(&events_lock, flags);
	return to_copy;
	
 err_copy_usr:
	write_unlock_irqrestore(&events_lock, flags);
	return -EFAULT;
}

static const struct file_operations huginn_fops = {
	.owner = THIS_MODULE,
	.open = huginn_open,
	.write = huginn_write,
	.release = huginn_release,
};

/*
  init and exit 
 */

static int __init huginn_init(void) {
	int ret = 0;
	struct device *dev;

	pr_info("huginn: initializing module\n");

	huginn_proc = proc_create(MODULE_NAME, 0644, NULL, &huginn_proc_ops);

	if (huginn_proc == NULL) {
		pr_err("huginn: error initializing /proc/%s\n", MODULE_NAME);
		return -ENOMEM;
	}

	proc_set_size(huginn_proc, 80);
	proc_set_user(huginn_proc, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

	pr_info("huginn: successfully initialized /proc/%s\n", MODULE_NAME);
	
	ret = alloc_chrdev_region(&dev_num, 0, 1, MODULE_NAME);
	if (ret < 0) {
		pr_err("huginn: failed to register device %d\n", ret);
		goto err_chrdev;
	}
	cdev_init(&huginn_cdev, &huginn_fops);

	ret = cdev_add(&huginn_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("huginn: failed to add cdev %d\n", ret);
		goto err_cdev;
	}
	huginn_class = class_create(MODULE_NAME);
	if (IS_ERR(huginn_class)) {
		ret = PTR_ERR(huginn_class);
		pr_err("huginn: failed to create class %d\n", ret);
		goto err_class;
	}
	dev = device_create(
		huginn_class,
		NULL,
		dev_num,
		NULL,
		MODULE_NAME);
	
	if (IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		pr_err("huginn: failed to create device %d\n", ret);
		goto err_device;
	}
	
	return 0;
	
 err_device:
	class_destroy(huginn_class);
 err_class:
	cdev_del(&huginn_cdev);
 err_cdev:
	unregister_chrdev_region(dev_num, 1);
 err_chrdev:
	return ret;
}

static void __exit huginn_exit(void) {
	device_destroy(huginn_class, dev_num);
	class_destroy(huginn_class);
	cdev_del(&huginn_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("huginn: unloaded module\n");
}

module_init(huginn_init);
module_exit(huginn_exit);
