#include <linux/completion.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/version.h>

static struct completion producer_completed;
static struct completion consumer_completed;

static int producer_thread(void *arg) {
	pr_info("producer is executing\n");
	complete_all(&producer_completed);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
	kthread_complete_and_exit(&consumer_completed, 0);
#else
	complete_and_exit(&consumer_completed, 0);
#endif
}

static int consumer_thread(void *arg) {
	wait_for_completion(&producer_completed);
	pr_info("consumer is executing\n");
	complete_all(&consumer_completed);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
	kthread_complete_and_exit(&consumer_completed, 0);
#else
	complete_and_exit(&consumer_completed, 0);
#endif
}

static int __init completion_init(void) {
	struct task_struct *producer_task;
	struct task_struct *consumer_task;

	pr_info("completion module example\n");

	init_completion(&producer_completed);
	init_completion(&consumer_completed);

	producer_task = kthread_create(producer_thread, NULL, "KThread producer");

	if (IS_ERR(producer_thread))
		goto ERROR_THREAD_1;

	consumer_task = kthread_create(consumer_thread, NULL, "KThread consumer");

	if (IS_ERR(consumer_thread))
		goto ERROR_THREAD_2;

	wake_up_process(consumer_task);
	wake_up_process(producer_task);

	return 0;

 ERROR_THREAD_2:
	kthread_stop(producer_task);
 ERROR_THREAD_1:
	return -1;
}

static void __exit completion_exit(void) {
	wait_for_completion(&producer_completed);
	wait_for_completion(&consumer_completed);

	pr_info("completion exit\n");
}

module_init(completion_init);
module_exit(completion_exit);

MODULE_DESCRIPTION("completions example");
MODULE_LICENSE("GPL");
