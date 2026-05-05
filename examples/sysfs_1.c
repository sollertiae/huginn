#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>

static struct kobject *module;

static int var = 0;

static ssize_t var_show(struct kobject *kobj,
			struct kobj_attribute *attr,
			char *buff
	) {
	return sprintf(buff, "%d\n", var);
}

static ssize_t var_store(struct kobject *kobj,
			 struct kobj_attribute *attr,
			 const char *buff,
			 size_t count
	) {
	sscanf(buff, "%d", &var);
	return count;
}

static struct kobj_attribute var_attr = __ATTR(var, 0660, var_show, var_store);

static int __init sysfsmod_init(void) {
	int error = 0;

	pr_info("Module initialized\n");

	module = kobject_create_and_add("sysfsmodule", kernel_kobj);

	if (!module)
		return -ENOMEM;

	error = sysfs_create_file(module, &var_attr.attr);
	if (error) {
		kobject_put(module);
		pr_info("failed to create variable /sys/kernel/sysfsmodule\n");
	}
	return error;
}

static void __exit sysfsmod_exit(void) {
	pr_info("Removed sysfsmod\n");
	kobject_put(module);
}

module_init(sysfsmod_init);
module_exit(sysfsmod_exit);

MODULE_LICENSE("GPL");
