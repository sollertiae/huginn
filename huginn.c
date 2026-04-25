#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David");
MODULE_DESCRIPTION("Hello World module");

static int __init huginn_init(void) {
    printk(KERN_INFO "Hello, World! Module Loaded");
    return 0;
}

static void __exit huginn_exit(void) {
    printk(KERN_INFO "Module unloaded");
}

module_init(huginn_init);
module_exit(huginn_exit);