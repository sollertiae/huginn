#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>

static DEFINE_MUTEX(mutex);

static int __init mutexmod_init(void) {
	int ret;

	pr_info("example of mutex");

	ret = mutex_trylock(&mutex);

	if (ret != 0) {
		pr_info("mutex is locked\n");
		if (mutex_is_locked(&mutex) == 0) {
			pr_info("mutex failed to lock\n");
		}
		mutex_unlock(&mutex);
		pr_info("unlocked mutex\n");
	} else {
		pr_info("failed to lock\n");
	}
	return 0;	
}

static void __exit mutexmod_exit(void) {
	pr_info("exiting mutex\n");
}

module_init(mutexmod_init);
module_exit(mutexmod_exit);

MODULE_DESCRIPTION("mutex test");
MODULE_LICENSE("GPL");
