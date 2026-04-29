#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");

static short int x = 1;
static int y = 123;
static long int z = 9876;
static char *str = "Hello from argument";
static int arr[2] = {765, 567};
static int argc = 0;


module_param(x, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(x, "short int");
module_param(y, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(y, "int");
module_param(z, long, S_IRUSR);
MODULE_PARM_DESC(z, "long int");
module_param(str, charp, 0000);
MODULE_PARM_DESC(str, "string");
module_param_array(arr, int, &argc, 0000);
MODULE_PARM_DESC(arr, "int array");

static int __init init_func(void) {
    pr_info("Handling arguments\n");
    pr_info("x = %hd\n", x);
    pr_info("y = %d\n", y);
    pr_info("z = %ld\n", z);
    pr_info("string = %s\n", str);

    for (int i = 0; i < ARRAY_SIZE(arr); ++i) {
        pr_info("arr[%d] = %d\n", i, arr[i]);
    }
    pr_info("arr has %d arguments\n", argc);
    return 0;
}

static void __exit exit_func(void) {
    pr_info("Bye arguments");
}

module_init(init_func);
module_exit(exit_func);