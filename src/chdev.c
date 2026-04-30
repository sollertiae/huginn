#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/errno.h>

static int dv_open(struct inode *, struct file *);
static int dv_release(struct inode *, struct file *);
static ssize_t dv_read(struct file *, char __user *, size_t length, loff_t * offset);
static ssize_t dv_write(struct file *, const char __user *, size_t len, loff_t * off);

#define DEVICE_NAME "first_device"
#define BUF_LEN 128

static int major;

enum {
    CDEV_NOT_USED,
    CDEV_EXCLUSIVE_OPEN,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN + 1];

static struct class *cls;

static struct file_operations dv_fops = {
    .read = dv_read,
    .write = dv_write,
    .open = dv_open,
    .release = dv_release,
};

static int __init dv_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &dv_fops);
    if (major < 0) {
        pr_alert("Failed to register the device %d\n", major);
        return major;
    }

    pr_info("Successfully loaded on major number %d\n", major);
    
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
        cls = class_create(DEVICE_NAME);
    #else
        cls = class_create(THIS_MODULE, DEVICE_NAME);
    #endif

    if (IS_ERR(cls)) {
        pr_err("Failed to create class for device\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(cls);
    }
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Successfully created the device on /dev/%s\n", DEVICE_NAME);

    return 0;
}

static void __exit dv_exit(void) {
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    unregister_chrdev(major, DEVICE_NAME);
}

static int dv_open(struct inode *inode, struct file *file) {
    static int counter = 0;

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    sprintf(msg, "Device opened %d times\n",  ++counter);
    return 0;
}

static int dv_release(struct inode *inode, struct file *file) {
    atomic_set(&already_open, CDEV_NOT_USED);

    return 0;
}

static ssize_t dv_read(
    struct file *filp,
    char __user *buffer,
    size_t length,
    loff_t *offset
) {
    int bytes_read = 0;
    const char *msg_ptr = msg;

    if (!*(msg_ptr + *offset)) {
        *offset = 0;
        return 0;
    }

    msg_ptr += *offset;

    while (length && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }
    *offset += bytes_read;
    return bytes_read;
}

static ssize_t dv_write(struct file *filp, const char __user *buff, size_t len, loff_t *off) {
    pr_alert("Operation is not supported\n");
    return -EINVAL;
}

module_init(dv_init);
module_exit(dv_exit);

MODULE_LICENSE("GPL");