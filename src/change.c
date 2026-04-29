#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David");
MODULE_DESCRIPTION("Understanding how module_param_cb() works");

static int my_argument = 33;
MODULE_PARM_DESC(my_argument, "argument to be logged");

static int argument_set(const char *val, const struct kernel_param *kp) {
    int ret;
    ret = param_set_int(val, kp);
    if (ret)
        return ret;
    
    pr_info("the argument was updated to %d\n", my_argument);
    return 0;
}

static int argument_get(char* buffer, const struct kernel_param *kp) {
    int ret;
    ret = param_get_int(buffer, kp);
    if (ret >= 0)
        pr_info("the argument was read\n");
    return ret;
}

static const struct kernel_param_ops argument_ops = {
    .set = argument_set,
    .get = argument_get,
};

// 0644 = permissions r/w
module_param_cb(my_argument, &argument_ops, &my_argument, 0644);

static int __init init_func(void) {
    pr_info("Hello, logging argument: \n");
    pr_info("argument starts at %d\n", my_argument);
    return 0;
}

static void __exit exit_func(void) {
    pr_info("Log tracking ended");
}

module_init(init_func);
module_exit(exit_func);