#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>

MODULE_DESCRIPTION("export values through debugfs");
MODULE_LICENSE("GPL");

#define DEBUGFS_DIR "debugfs_test"

static struct dentry *debugfs_dir;
static u32 debug_value = 42;
static bool debug_flag = true;

module_param(debug_value, uint, 0600);
MODULE_PARM_DESC(debug_value, "value expored through sysfs and debugfs");

module_param(debug_flag, bool, 0600);
MODULE_PARM_DESC(debug_flag, "enable or disable the flag");

static int __init debugfs_init(void) {
	debugfs_dir = debugfs_create_dir(DEBUGFS_DIR, NULL);
	debugfs_create_u32("debug_val", 0600, debugfs_dir, &debug_value);
	debugfs_create_bool("debug_flag", 0600, debugfs_dir, &debug_flag);
	pr_info("debugfs interface registered /sys/kernel/debug/%s\n", DEBUGFS_DIR);

	return 0;
}

static void __exit debugfs_exit(void) {
	debugfs_remove_recursive(debugfs_dir);
	pr_info("debugfs example removed\n");
}

module_init(debugfs_init);
module_exit(debugfs_exit);
